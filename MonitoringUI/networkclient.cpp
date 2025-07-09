#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

#include "networkclient.h"
#include "../common/appconfig.h"

NetworkClient::NetworkClient(QObject *parent)
    : QObject(parent),
    _socket(new QTcpSocket(this))
{
    connect(_socket, &QTcpSocket::readyRead, this, &NetworkClient::onReadyRead);
    connect(_socket, &QTcpSocket::errorOccurred, this, &NetworkClient::onErrorOccurred);
}

void NetworkClient::requestAllDevices()
{
    QJsonObject request{ {"cmd", "getEquipment"} };
    sendRequest(request);
}

void NetworkClient::requestAddDevice(const Device &device)
{
    QJsonObject request = device.toJson();
    request["cmd"] = "addEquipment";
    request.remove("id");
    request.remove("dev_status");
    sendRequest(request);
}

void NetworkClient::requestDeleteDevice(const QString &devNum)
{
    QJsonObject request{
        {"cmd", "deleteEquipment"},
        {"number", devNum}
    };
    sendRequest(request);
}

void NetworkClient::requestEditDevice(const Device &device)
{
    QJsonObject request = device.toJson();
    request["cmd"] = "editEquipment";
    request.remove("id");
    request.remove("dev_status");
    sendRequest(request);
}

void NetworkClient::requestStatusLog(const QDateTime &from, const QDateTime &to)
{
    QJsonObject request{
        {"cmd", "getStatusLog"},
        {"from", from.toUTC().toString(Qt::ISODate)},
        {"to", to.toUTC().toString(Qt::ISODate)}
    };
    sendRequest(request);
}

void NetworkClient::sendRequest(const QJsonObject &request)
{
    const AppConfig& config = AppConfig::instance();
    const QString serverHost = config.dbHost();
    const quint16 serverPort = config.serverPort();

    if (serverHost.isEmpty() || serverPort == 0) {
        emit requestFailed("Адрес сервера и порт не заданы в конфигурационном файле.");
        return;
    }

    if (_socket->state() != QAbstractSocket::ConnectedState) {
        _socket->connectToHost(serverHost, serverPort);
        if (!_socket->waitForConnected(3000)) {
            emit requestFailed("Не удалось подключится к серверу:" + _socket->errorString());
            return;
        }
    }

    _socket->write(QJsonDocument(request).toJson());
}
void NetworkClient::onReadyRead()
{
    QByteArray data = _socket->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (doc.isNull() || !doc.isObject()) {
        emit requestFailed("Получена неправильная информация от сервера.");
        return;
    }

    QJsonObject response = doc.object();
    if (!response.contains("status") || !response["status"].isString()) {
        emit requestFailed("Неправильный ответ сервера.");
        return;
    }

    QString status = response["status"].toString();
    if (status == "error") {
        if (response.contains("message")) {
            QString message = response.value("message").toString();
            emit requestFailed(message);
        } else {
            emit requestFailed("Неизвестная ошибка");
        }
        return;
    }

    if (response.contains("data")) {
        QJsonObject dataObj = response["data"].toObject();
        if (dataObj.contains("devices")) {
            QList<Device> devices;
            QJsonArray devicesArray = dataObj["devices"].toArray();
            for (const QJsonValue& val : devicesArray) {
                devices.append(Device::fromJson(val.toObject()));
            }
            emit devicesReceived(devices);
        }
        else if (dataObj.contains("logs")) {
            QList<StatusLogEntry> logEntries;
            QJsonArray logArray = dataObj["logs"].toArray();
            for (const QJsonValue& val : logArray) {
                logEntries.append(StatusLogEntry::fromJson(val.toObject()));
            }
            emit statusLogReceived(logEntries);
        }
    } else {
        emit requestSucceeded("Операция прошла успешно.");
    }
}

void NetworkClient::onErrorOccurred(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    emit requestFailed("Ошибка сети: " + _socket->errorString());
}
