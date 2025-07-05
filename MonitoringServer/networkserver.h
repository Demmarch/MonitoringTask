#ifndef NETWORKSERVER_H
#define NETWORKSERVER_H

#include <QObject>
#include <QJsonObject>
#include <QTcpServer>
#include <QTcpSocket>

class NetworkServer : public QObject
{
    Q_OBJECT

public:
    explicit NetworkServer(QObject *parent = nullptr);
    ~NetworkServer();

public slots:
    void startServer();

private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();

private:
    void handleRequest(const QJsonObject& request, QTcpSocket* clientSocket);
    void sendResponse(const QJsonObject& response, QTcpSocket* clientSocket);
    QJsonObject createErrorResponse(const QString& message);
    QJsonObject createSuccessResponse(const QJsonObject& data = QJsonObject());

    QTcpServer* _tcpServer;
};

#endif // NETWORKSERVER_H
