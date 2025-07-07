#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <QObject>
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QJsonObject>

#include "../common/device.h"

class NetworkClient : public QObject
{
    Q_OBJECT

public:
    explicit NetworkClient(QObject *parent = nullptr);

    void requestAllDevices();
    void requestAddDevice(const Device& device);
    void requestDeleteDevice(const QString& devNum);

signals:
    void devicesReceived(const QList<Device>& devices);
    void requestSucceeded(const QString& message);
    void requestFailed(const QString& message);

private slots:
    void onReadyRead();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);

private:
    void sendRequest(const QJsonObject& request);

    QTcpSocket* _socket;
};

#endif // NETWORKCLIENT_H
