#include "appconfig.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>

AppConfig& AppConfig::instance()
{
    static AppConfig an_instance;
    return an_instance;
}

bool AppConfig::loadFromFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "Невозможно открыть файл конфигурации:" << filePath;
        return false;
    }

    QXmlStreamReader xml(&file);
    QString currentParentTag;
    while (!xml.atEnd() && !xml.hasError())
    {
        xml.readNext();
        if (xml.isStartElement()) {
            if (xml.name() == "server" || xml.name() == "database" || xml.name() == "monitoring") {
                currentParentTag = xml.name().toString();
            }
            if (currentParentTag == "server") {
                if (xml.name() == "port")        _serverPort = xml.readElementText().toUShort();
            } else if (currentParentTag == "database") {
                if (xml.name() == "host")        _dbHost = xml.readElementText();
                if (xml.name() == "port")        _dbPort = xml.readElementText().toInt();
                if (xml.name() == "dbname")      _dbName = xml.readElementText();
                if (xml.name() == "user")        _dbUser = xml.readElementText();
                if (xml.name() == "password")    _dbPassword = xml.readElementText();
            } else if (currentParentTag == "monitoring") {
                if (xml.name() == "interval_ms") _pollIntervalMs = xml.readElementText().toInt();
            }
        }
    }
    currentParentTag.clear();
    file.close();

    if (xml.hasError()) {
        qCritical() << "XML ошибка: " << xml.errorString();
        return false;
    }

    return true;
}

AppConfig::AppConfig() : _serverPort(0), _dbPort(0) {}
AppConfig::~AppConfig() {}

quint16 AppConfig::serverPort() const { return _serverPort; }
const QString& AppConfig::dbHost() const { return _dbHost; }
int AppConfig::dbPort() const { return _dbPort; }
const QString& AppConfig::dbName() const { return _dbName; }
const QString& AppConfig::dbUser() const { return _dbUser; }
const QString& AppConfig::dbPassword() const { return _dbPassword; }
int AppConfig::pollIntervalMs() const { return _pollIntervalMs; }
