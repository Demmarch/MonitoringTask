#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <QThread>

#include "networkserver.h"
#include "../common/appconfig.h"
#include "databasemanager.h"
#include "../common/device.h"

NetworkServer::NetworkServer(QObject *parent)
    : QObject(parent), _tcpServer(new QTcpServer(this))
{
    connect(_tcpServer, &QTcpServer::newConnection, this, &NetworkServer::onNewConnection);
}

NetworkServer::~NetworkServer() {}

void NetworkServer::startServer()
{
    const quint16 port = AppConfig::instance().serverPort();
    if (!_tcpServer->listen(QHostAddress::Any, port)) {
        qCritical() << "Не удалось запустить сервер на портe:" << port << ":" << _tcpServer->errorString();
        return;
    }
    qInfo() << "Сервер работает на порте:" << port << "в потоке:" << QThread::currentThreadId();
}

void NetworkServer::onNewConnection()
{
    QTcpSocket *clientSocket = _tcpServer->nextPendingConnection();
    if (clientSocket) {
        qInfo() << "Новое клиентское соединение:" << clientSocket->peerAddress().toString();
        connect(clientSocket, &QTcpSocket::readyRead, this, &NetworkServer::onReadyRead);
        connect(clientSocket, &QTcpSocket::disconnected, this, &NetworkServer::onDisconnected);
    }
}

void NetworkServer::onReadyRead()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    QByteArray data = clientSocket->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (doc.isNull() || !doc.isObject()) {
        sendResponse(createErrorResponse("Неверный JSON."), clientSocket);
        return;
    }

    handleRequest(doc.object(), clientSocket);
}

void NetworkServer::onDisconnected()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (clientSocket) {
        qInfo() << "Клиентское соединение разорвано:" << clientSocket->peerAddress().toString();
        clientSocket->deleteLater();
    }
}

void NetworkServer::handleRequest(const QJsonObject &request, QTcpSocket *clientSocket)
{
    if (!request.contains("cmd") || !request["cmd"].isString()) {
        sendResponse(createErrorResponse("В запросе нет 'cmd'."), clientSocket);
        return;
    }

    QString cmd = request["cmd"].toString();
    QJsonObject response;

    if (cmd == "getEquipment") {
        QList<Device> devices = DatabaseManager::instance().getAllDevices();
        QJsonArray devicesArray;
        for (Device& device : devices) {
            QJsonObject deviceJson = device.toJson();
            deviceJson.remove("id");
            devicesArray.append(deviceJson);
        }
        response = createSuccessResponse(QJsonObject{{"devices", devicesArray}});
    }
    else if (cmd == "deleteEquipment") {
        if (!request.contains("number") || !request["number"].isString()) {
            response = createErrorResponse("Отсутствует серийник в запросе.");
        } else {
            QString devNum = request["number"].toString();
            if (DatabaseManager::instance().deleteDevice(devNum)) {
                response = createSuccessResponse();
            } else {
                response = createErrorResponse("Не удалось удалить устройство, либо его нет изначально.");
            }
        }
    }
    else if (cmd == "addEquipment") {
        Device newDevice = Device::fromJson(request);
        if (newDevice.dev_num.isEmpty() || newDevice.dev_name.isEmpty()) {
            response = createErrorResponse("Имя устройства и его серийник отсутствуют в запросе.");
        } else {
            if (DatabaseManager::instance().addDevice(newDevice)) {
                response = createSuccessResponse();
            } else {
                response = createErrorResponse("Не удалось добавить новое устройство. Возможно оно уже есть с таким серийником");
            }
        }
    }
    else if (cmd == "editEquipment") {
        Device deviceToUpdate = Device::fromJson(request);
        if (deviceToUpdate.dev_num.isEmpty() || deviceToUpdate.dev_name.isEmpty()) {
            response = createErrorResponse("Имя устройства и его серийник отсутствуют в запросе.");
        } else {
            if (DatabaseManager::instance().updateDevice(deviceToUpdate)) {
                response = createSuccessResponse();
            } else {
                response = createErrorResponse("Не удалось изменить устройство.");
            }
        }
    }
    else if (cmd == "getStatusLog") {
        if (!request.contains("from") || !request.contains("to")) {
            response = createErrorResponse("Отсутствуют поля 'from' и 'to'.");
        } else {
            QDateTime from = QDateTime::fromString(request.value("from").toString(), Qt::ISODate);
            QDateTime to = QDateTime::fromString(request.value("to").toString(), Qt::ISODate);

            if (!from.isValid() || !to.isValid()) {
                response = createErrorResponse("Неверный формат даты. Нужен формат ISO 8601 (YYYY-MM-DDTHH:MM:SS).");
            } else {
                QList<StatusLogEntry> logs = DatabaseManager::instance().getStatusLog(from, to);
                QJsonArray logArray;
                for (const StatusLogEntry& entry : logs) {
                    logArray.append(QJsonObject{
                        {"log_id", entry.log_id},
                        {"dev_num", entry.dev_num},
                        {"change_time", entry.change_time.toString(Qt::ISODate)},
                        {"old_status", entry.old_status},
                        {"new_status", entry.new_status}
                    });
                }
                response = createSuccessResponse(QJsonObject{{"logs", logArray}});
            }
        }
    }
    else {
        response = createErrorResponse("Неизвестная команда: " + cmd);
    }

    sendResponse(response, clientSocket);
}

void NetworkServer::sendResponse(const QJsonObject &response, QTcpSocket *clientSocket)
{
    if (clientSocket && clientSocket->isOpen()) {
        clientSocket->write(QJsonDocument(response).toJson(QJsonDocument::Compact));
    }
}

QJsonObject NetworkServer::createErrorResponse(const QString &message)
{
    return {{"status", "error"}, {"message", message}};
}

QJsonObject NetworkServer::createSuccessResponse(const QJsonObject &data)
{
    QJsonObject response{{"status", "success"}};
    if (!data.isEmpty()) {
        response["data"] = data;
    }
    return response;
}
