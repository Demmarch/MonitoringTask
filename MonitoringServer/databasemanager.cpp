#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QMutexLocker>

#include "databasemanager.h"
#include "../common/appconfig.h"

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager an_instance;
    return an_instance;
}

DatabaseManager::DatabaseManager()
{
    _db = QSqlDatabase::addDatabase("QPSQL", "monitoring_connection");
}

DatabaseManager::~DatabaseManager()
{
    if (_db.isOpen()) {
        _db.close();
    }
}

bool DatabaseManager::openDatabase()
{
    QMutexLocker locker(&_mutex);

    if (_db.isOpen()) {
        return true;
    }

    const AppConfig& config = AppConfig::instance();
    _db.setHostName(config.dbHost());
    _db.setPort(config.dbPort());
    _db.setDatabaseName(config.dbName());
    _db.setUserName(config.dbUser());
    _db.setPassword(config.dbPassword());

    if (!_db.open()) {
        qCritical() << "Ошибка соединения с базой данных:" << _db.lastError().text();
        return false;
    }

    qInfo() << "Соединение успешно.";
    return true;
}

QList<Device> DatabaseManager::getAllDevices()
{
    QMutexLocker locker(&_mutex);
    QList<Device> devices;
    QSqlQuery query(_db);

    query.prepare("SELECT id, dev_num, dev_name, dev_description, dev_status FROM devices");

    if (!query.exec()) {
        qWarning() << "Ошибка выполнения DatabaseManager::getAllDevices():" << query.lastError().text();
        return devices;
    }

    while (query.next()) {
        Device device;
        device.id = query.value("id").toInt();
        device.dev_num = query.value("dev_num").toString();
        device.dev_name = query.value("dev_name").toString();
        device.dev_description = query.value("dev_description").toString();
        device.dev_status = query.value("dev_status").toInt();
        devices.append(device);
    }

    return devices;
}

bool DatabaseManager::updateDeviceStatus(const QString &devNum, int newStatus)
{
    QMutexLocker locker(&_mutex);

    QSqlQuery query(_db);
    query.prepare("UPDATE devices SET dev_status = :status WHERE dev_num = :dev_num");
    query.bindValue(":status", newStatus);
    query.bindValue(":dev_num", devNum);

    if (!query.exec()) {
        qWarning() << "Ошибка обновления статусов:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool DatabaseManager::addDevice(const Device &device)
{
    QMutexLocker locker(&_mutex);

    QSqlQuery checkQuery(_db);
    checkQuery.prepare("SELECT COUNT(*) FROM devices WHERE dev_num = :dev_num");
    checkQuery.bindValue(":dev_num", device.dev_num);
    if (!checkQuery.exec() || !checkQuery.next() || checkQuery.value(0).toInt() > 0) {
        qWarning() << "Не удалось добавить новое устройство: устройство с номером" << device.dev_num << "уже существует, или другая ошибка в БД.";
        return false;
    }

    QSqlQuery query(_db);
    query.prepare("INSERT INTO devices (dev_num, dev_name, dev_description, dev_status) "
                  "VALUES (:dev_num, :dev_name, :dev_description, 0)");
    query.bindValue(":dev_num", device.dev_num);
    query.bindValue(":dev_name", device.dev_name);
    query.bindValue(":dev_description", device.dev_description);

    if (!query.exec()) {
        qWarning() << "Не удалось добавить устройство:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::deleteDevice(const QString &devNum)
{
    QMutexLocker locker(&_mutex);

    if (devNum.isEmpty()) {
        qWarning() << "Не удалось удалить устройство: серийник пустой.";
        return false;
    }

    QSqlQuery query(_db);
    query.prepare("DELETE FROM devices WHERE dev_num = :dev_num");
    query.bindValue(":dev_num", devNum);

    if (!query.exec()) {
        qWarning() << "Не удалось удалить устройство:" << query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0) {
        qWarning() << "Устройство с серийником" << devNum << "не найден для удаления.";
    }

    return query.numRowsAffected() > 0;
}

bool DatabaseManager::updateDevice(const Device &device)
{
    QMutexLocker locker(&_mutex);

    QSqlQuery query(_db);
    query.prepare("UPDATE devices SET dev_name = :dev_name, dev_description = :dev_description "
                  "WHERE dev_num = :dev_num");
    query.bindValue(":dev_name", device.dev_name);
    query.bindValue(":dev_description", device.dev_description);
    query.bindValue(":dev_num", device.dev_num);

    if (!query.exec()) {
        qWarning() << "Failed to update device data:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

void DatabaseManager::logStatusChange(int deviceId, const QString& devNum, int oldStatus, int newStatus)
{
    QMutexLocker locker(&_mutex);

    QSqlQuery query(_db);
    query.prepare("INSERT INTO status_log (device_id, dev_num, old_status, new_status) "
                  "VALUES (:device_id, :dev_num, :old_status, :new_status)");
    query.bindValue(":device_id", deviceId);
    query.bindValue(":dev_num", devNum);
    query.bindValue(":old_status", oldStatus);
    query.bindValue(":new_status", newStatus);

    if (!query.exec()) {
        qWarning() << "Failed to log status change:" << query.lastError().text();
    }
}

QList<StatusLogEntry> DatabaseManager::getStatusLog(const QDateTime &from, const QDateTime &to)
{
    QMutexLocker locker(&_mutex);
    QList<StatusLogEntry> logEntries;

    QSqlQuery query(_db);
    query.prepare("SELECT log_id, dev_num, TO_CHAR(change_time AT TIME ZONE 'UTC', 'YYYY-MM-DD\"T\"HH24:MI:SS\"Z\"') as change_time_iso, old_status, new_status "
                  "FROM status_log WHERE change_time BETWEEN :from AND :to "
                  "ORDER BY change_time DESC");

    query.bindValue(":from", from);
    query.bindValue(":to", to);

    if (!query.exec()) {
        qWarning() << "Failed to get status log:" << query.lastError().text();
        return logEntries;
    }

    while (query.next()) {
        StatusLogEntry entry;
        entry.log_id = query.value("log_id").toInt();
        entry.dev_num = query.value("dev_num").toString();
        // --- ИЗМЕНЕНИЕ: Читаем отформатированную строку и создаем QDateTime ---
        entry.change_time = QDateTime::fromString(query.value("change_time_iso").toString(), Qt::ISODate);
        entry.old_status = query.value("old_status").toInt();
        entry.new_status = query.value("new_status").toInt();
        logEntries.append(entry);
    }

    return logEntries;
}
