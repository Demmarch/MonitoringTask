#ifndef MONITORINGWORKER_H
#define MONITORINGWORKER_H

#include <QObject>
#include <QTimer>

class MonitoringWorker : public QObject
{
    Q_OBJECT

public:
    explicit MonitoringWorker(QObject *parent = nullptr);

public slots:
    void startMonitoring();

private slots:
    void pollDevices();

private:
    int getDeviceStatus();

    QTimer* _pollTimer;
};

#endif // MONITORINGWORKER_H
