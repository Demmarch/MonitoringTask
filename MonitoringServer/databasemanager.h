#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QMutex>
#include <QList>

#include "../common/device.h"

// Singleton
class DatabaseManager
{
public:
    static DatabaseManager& instance();
    bool openDatabase();
    QList<Device> getAllDevices();
    bool updateDeviceStatus(const QString& devNum, int newStatus);
    bool addDevice(const Device& device);
private:
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QSqlDatabase _db;
    QMutex _mutex;
};

#endif // DATABASEMANAGER_H
