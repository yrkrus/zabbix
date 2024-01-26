# Мини агент for ZabbixAgent
Данный мини агент создает на основе unixtime времени уникальный ключ, благодаря которому автообнаруженные узлы имеющие динамический IP не задваиваются в мониторинге 


# Как поставить
В конфиг zabbix_agentd.conf \
добавить параметр \
UserParameter=machineID[*], C:\ПАПКА С ЗАББИКС АГЕНТОМ\machineID.exe "$1"

импортировать шаблон zbx_templates_machine.xml в zabbix 

Правила обнаружения \
![изображение](https://github.com/yrkrus/zabbix/assets/137649829/0cc39e17-0c05-4f5d-be45-0acd48229462)

Действия обнаружения \
![изображение](https://github.com/yrkrus/zabbix/assets/137649829/f83da12f-5944-4862-94b0-624bd5f0eb03)



## Поддерживаемые команды
создать уникальный ID (есть проверка на ранее созданный)
```bash
  create_maсhineID
```
создать уникальный ID (создаст новый ID, даже если уже существует)
```bash
  create_maсhineID_force
```
показать уникальный ID
```bash
  show_machineID
```
показать версию агента
```bash
  show_version
```
