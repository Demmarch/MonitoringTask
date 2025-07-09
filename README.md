# Monitoring Task
Задание сделано в соотвествии с небольшой вольной интерпретацией следующего задания https://disk.yandex.ru/d/QqH_Lt0aWCHIww
Ссылка неактуальна, так что оставляю свою - https://docs.google.com/document/d/19Q7mL21yqtfozl6CJo6PIcEd16sBN67X/edit?usp=drive_link&ouid=101132078387149984559&rtpof=true&sd=true (но без отдельных картинок и без файла что должен знать программист)

## Конфигурация сервера
И клиент и сервер использует одну и ту же струтуру конфигурации, описанную в xml файле, пример файла есть в репозитории
xml файл для сервера необходимо поместить рядом с исполняемым файлом, а на клиенте просто его выбрать. У меня это был следующий файл
https://drive.google.com/file/d/1bCBYeZ5YuckDftcDEVbpVysJzYHZ2Olu/view?usp=sharing

### Драйвер PostreSQL
Всё делалось под Windows и для этого я копировал файлы драйверов PostreSQL рядом с исполняемым файлом
"libiconv-2.dll"
"libintl-9.dll"
"libpq.dll"
"libssl-3-x64.dll"