#include <QDebug>
#include <QRandomGenerator>
#include <QThread>

#include "monitoringworker.h"
#include "databasemanager.h"
#include "../common/appconfig.h"

MonitoringWorker::MonitoringWorker(QObject *parent)
    : QObject(parent), _pollTimer(new QTimer(this))
{
    connect(_pollTimer, &QTimer::timeout, this, &MonitoringWorker::pollDevices);
}

void MonitoringWorker::startMonitoring()
{
    qInfo() << "Процесс мониторинга находиться на потоке:" << QThread::currentThreadId();
    int interval = AppConfig::instance().pollIntervalMs();
    _pollTimer->start(interval);

    pollDevices();
}

void MonitoringWorker::pollDevices()
{
    qInfo() << "Начало опроса устройств...";

    QList<Device> devices = DatabaseManager::instance().getAllDevices();

    if (devices.isEmpty()) {
        qInfo() << "Не найдено устройств.";
        return;
    }

    for (Device& device : devices)
    {
        int newStatus = getDeviceStatus();
        if (device.dev_status != newStatus)
        {
            qInfo() << "Изменился статус для устройства" << device.dev_num
                    << ": с" << device.dev_status << "на" << newStatus;
            DatabaseManager::instance().logStatusChange(device.id, device.dev_num, device.dev_status, newStatus);
            DatabaseManager::instance().updateDeviceStatus(device.dev_num, newStatus);
        }
    }
}

int MonitoringWorker::getDeviceStatus()
{
    return QRandomGenerator::global()->bounded(0, 4);
}
