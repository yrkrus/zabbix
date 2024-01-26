//#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <stdio.h>
#include <windows.h>
#include <winreg.h>
#include <iostream>
#include <ctime>


using namespace std;

const char REG_DST[]	= { "SOFTWARE\\Zabbix" };   // ветка реестра
const char REG_VALUE[]	= { "U_ID" };				// значение
const char VERSION[]	= { "1.0" };				// версия агента



enum MyCommands
{
	create_maсhineID,
	create_maсhineID_force,
	show_machineID,
	show_version
};

// поиск отправляемых команд
MyCommands getCommands(char *ch) {	

	string tmp = static_cast<string>(ch);
	
	if (tmp == "create_mahineID")			return  create_maсhineID; 
	if (tmp == "create_maсhineID_force")	return  create_maсhineID_force;
	if (tmp == "show_machineID")			return  show_machineID;
	if (tmp == "show_version")				return  show_version;

	return create_maсhineID; // default;	
}


class MyRegistry
{
private:

	HKEY h;
	PBYTE pbBuff;
	DWORD cBuff = 0;
	DWORD Type = 0;

	// размер длинны int 
	int IntSize(int num)
	{
		int isize = 0;

		while (num)
		{
			num /= 10;
			++isize;
		}

		return isize;
	}

	// перевод из int в char
	void ItoA(int num, char *buffer, size_t buffer_size)
	{
		int num_order = IntSize(num);		

		buffer[num_order] = '\0';

		for (int i = num_order; i > 0; --i)
		{
			buffer[i - 1] = '0' + num % 10;

			num /= 10;
		}		
	}

	// создание unixtime
	char *getUnixTime() {
		
		const time_t t = time(NULL);  // текущее localhost unixt_time время
		int unixtime = static_cast<int>(t);

		// char массив
		char *str = new char[IntSize(unixtime)+1];

		ItoA(unixtime, str, IntSize(unixtime)+1);
		
		return str;
	}
	
	// перевод char -> int
	int char_to_int(const char *ch, int size) {
				
		char *stackbuf = new char[size+1];

		memcpy(stackbuf, ch, size);

		stackbuf[size] = '\0'; 
		
		return atoi(stackbuf);	
		
	}

public:
	MyRegistry();
	~MyRegistry();

	
	// проверим есть ли ветка реестра
	const bool isOpenKey(LPCSTR key) {
		bool isexist{false};
		
		if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, key, 0, KEY_QUERY_VALUE, &h) == ERROR_SUCCESS) {
			isexist = true;
			//cout << "OK";
			
			RegCloseKey(h);			
			return isexist;
		}
		else {
			RegCloseKey(h);
			return isexist;
		}	
	}

	// проверим есть ли параметр
	const bool isValueKey(LPCSTR key, LPCSTR value) {
		bool isexist{ false };
		
		if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, key, 0, KEY_QUERY_VALUE, &h) == ERROR_SUCCESS) {
			if (RegQueryValueExA(h, value, NULL, NULL, NULL, &cBuff) == ERROR_SUCCESS) {
				isexist = true;
				
				RegCloseKey(h);
				return isexist;
			}
		}
		
		RegCloseKey(h);
		return isexist;
	}

	// получаем занчение 
	const int getMachineID(LPCSTR key, LPCSTR value) {
	
		int machineID = -1;

		// открываем ветку
		RegOpenKeyExA(HKEY_LOCAL_MACHINE, key, 0, KEY_QUERY_VALUE, &h);		
		
		if ((pbBuff = new BYTE[cBuff]) == NULL) {
			return machineID; // не инициализировать память
		}
		
		// получаем параметр значения
		RegQueryValueExA(h, value, NULL, &Type, pbBuff, &cBuff);

		char *strmachineID = new char[cBuff];
		strmachineID[cBuff] = '\0';	// последний знак должен быть \0

		// пребразуем в pbyte -> char
		for (int i = 0; i < cBuff; i++) {
			//cout << pbBuff[i];
			strmachineID[i] = (char)pbBuff[i];	
		}			
		
		// преобразуем в char -> int
		machineID = char_to_int(strmachineID, cBuff);		
		
		//delete[] strmachineID;
		delete[] pbBuff;
			
		return machineID;		
	}


	// создание machineID
	void generateMachineID(LPCSTR key, LPCSTR value) {
		
		// узнаем текущее unix время 
		const char *unixTime = getUnixTime();		
		
		// создаем ветку
		RegCreateKeyExA(HKEY_LOCAL_MACHINE,
							key,
							0,
							NULL,
							0, 
							KEY_ALL_ACCESS,
							NULL,
							&h,
							NULL);

		RegSetValueExA(h, value, 0, REG_SZ, (LPBYTE)unixTime, sizeof(unixTime)+2);
		RegCloseKey(h);	
	}

};

MyRegistry::MyRegistry()
{
}

MyRegistry::~MyRegistry()
{
}


// создание machineID
void createMachineID() {
	
	MyRegistry reg;

	if (reg.isOpenKey(REG_DST))
	{
		if (reg.isValueKey(REG_DST, REG_VALUE))
		{
			cout << "MachineID EXIST";
		}
		else {

			// ветки нет, значит и параметра нет, создаем
			reg.generateMachineID(REG_DST, REG_VALUE);

			if (reg.isValueKey(REG_DST, REG_VALUE))
			{
				cout << "MachineID created";
			}
			else
			{
				cout << "MachineID NOT created";
			}

		}

	}
	else {
		
		// ветки нет, значит и параметра нет, создаем
		reg.generateMachineID(REG_DST, REG_VALUE);

		if (reg.isValueKey(REG_DST, REG_VALUE))
		{
			cout << "MachineID created";
		}
		else
		{
			cout << "MachineID NOT created";
		}		
	}
	
}

// создать machineID даже если он уже существует
void createMachineIDforce() {
	MyRegistry reg;

	reg.generateMachineID(REG_DST, REG_VALUE);

	if (reg.isValueKey(REG_DST, REG_VALUE))
	{
		cout << "MachineID created";
	}
	else
	{
		cout << "MachineID NOT created";
	}

}

// показать machineID
void showMachineID() {
	MyRegistry reg;
	
	if (reg.isOpenKey(REG_DST))
	{
		if (reg.isValueKey(REG_DST, REG_VALUE))
		{
			cout << reg.getMachineID(REG_DST, REG_VALUE);
		}
		else {
			cout << -1;
		}
	}
	else {
		cout << -1;
	}
}

// показать версию агента
void show_ver() {
	cout << VERSION;
}

int main(int argc, char *argv[])
{  
	
	setlocale(LC_ALL, "");

	if (argc == 1 || argc > 2) {
		cout << "no arguments!\n";
		return -1;
	}
	
	MyCommands commands = getCommands(argv[1]);

	// команды
	switch (commands)
	{
		case create_maсhineID: {       // создание machineID
			createMachineID();
			break;
		}	
		case create_maсhineID_force: { // создание machineID (даже если он уже сущесвтует)  
			createMachineIDforce();
			break;
		}
		case show_machineID: {         // показать machineID
			showMachineID();
			break;
		}
		case show_version:	 {	       // показать версию агента
			show_ver();
			break;
		}
	}
	
	return 0;	
}


