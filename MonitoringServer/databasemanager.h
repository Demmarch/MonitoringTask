#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QMutex>
#include <QList>

#include "../common/device.h"
#include "../common/statuslogentry.h"

// Singleton
class DatabaseManager
{
public:
    static DatabaseManager& instance();
    bool openDatabase();
    QList<Device> getAllDevices();
    bool updateDeviceStatus(const QString& devNum, int newStatus);
    bool addDevice(const Device& device);
    bool deleteDevice(const QString& devNum);
    bool updateDevice(const Device& device);
    void logStatusChange(int deviceId, const QString& devNum, int oldStatus, int newStatus);
    QList<StatusLogEntry> getStatusLog(const QDateTime& from, const QDateTime& to);
private:
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QSqlDatabase _db;
    QMutex _mutex;
};

#endif // DATABASEMANAGER_H
