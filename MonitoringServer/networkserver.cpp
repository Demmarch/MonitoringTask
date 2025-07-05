#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <QThread>

#include "networkserver.h"
#include "appconfig.h"
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
        qCritical() << "Не удалось запустить сервер на порту" << port
                    << ":" << _tcpServer->errorString();
        return;
    }
    qInfo() << "Network server started on port" << port
            << "in thread:" << QThread::currentThreadId();
}

void NetworkServer::onNewConnection()
{
    QTcpSocket *clientSocket = _tcpServer->nextPendingConnection();
    if (clientSocket) {
        qInfo() << "New client connected:" << clientSocket->peerAddress().toString();
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
        sendResponse(createErrorResponse("Invalid JSON format."), clientSocket);
        return;
    }

    handleRequest(doc.object(), clientSocket);
}

void NetworkServer::onDisconnected()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (clientSocket) {
        qInfo() << "Client disconnected:" << clientSocket->peerAddress().toString();
        clientSocket->deleteLater();
    }
}

void NetworkServer::handleRequest(const QJsonObject &request, QTcpSocket *clientSocket)
{
    if (!request.contains("cmd") || !request["cmd"].isString()) {
        sendResponse(createErrorResponse("Request missing 'cmd' field."), clientSocket);
        return;
    }

    const QString cmd = request["cmd"].toString();
    QJsonObject response;

    if (cmd == "getEquipment") {
        QList<Device> devices = DatabaseManager::instance().getAllDevices();
        QJsonArray devicesArray;
        for (const auto& device : devices) {
            QJsonObject deviceJson = device.toJson();
            deviceJson.remove("id");
            devicesArray.append(deviceJson);
        }
        response = createSuccessResponse(QJsonObject{{"devices", devicesArray}});
    }
    else if (cmd == "deleteEquipment") {
        if (!request.contains("number") || !request["number"].isString()) {
            response = createErrorResponse("Request missing 'number' field.");
        } else {
            QString devNum = request["number"].toString();
            if (DatabaseManager::instance().deleteDevice(devNum)) {
                response = createSuccessResponse();
            } else {
                response = createErrorResponse("Failed to delete device or device not found.");
            }
        }
    }
    else if (cmd == "addEquipment") {
        Device newDevice = Device::fromJson(request);
        if (newDevice.dev_num.isEmpty() || newDevice.dev_name.isEmpty()) {
            response = createErrorResponse("Request missing 'dev_num' or 'dev_name' fields.");
        } else {
            if (DatabaseManager::instance().addDevice(newDevice)) {
                response = createSuccessResponse();
            } else {
                response = createErrorResponse("Failed to add device. It may already exist.");
            }
        }
    }
    else {
        response = createErrorResponse("Unknown command: " + cmd);
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
