#include <QCoreApplication>
#include <QFileInfo>
#include <QDebug>
#include <QThread>

#include "../common/appconfig.h"
#include "databasemanager.h"
#include "monitoringworker.h"
#include "networkserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setApplicationName("MonitoringService");

    QString configPath = QCoreApplication::applicationDirPath() + "/config.xml";
    QFileInfo configInfo(configPath);

    if (!configInfo.exists()) {
        qCritical() << "Файл не найден" << configPath;
        return 1;
    }
    if (!AppConfig::instance().loadFromFile(configPath)) {
        qCritical() << "Ошибка при загрузке из конфигурационного файла.";
        return 1;
    }

    qInfo() << "Успешная загрузка из файла конфигурации";

    if (!DatabaseManager::instance().openDatabase()) {
        qCritical() << "Ошибка при подключении к базе данных.";
        return 1;
    }

    QThread* workerThread = new QThread(&a);
    MonitoringWorker* worker = new MonitoringWorker();
    worker->moveToThread(workerThread);
    QObject::connect(workerThread, &QThread::started, worker, &MonitoringWorker::startMonitoring);
    QObject::connect(&a, &QCoreApplication::aboutToQuit, workerThread, &QThread::quit);
    QObject::connect(workerThread, &QThread::finished, worker, &MonitoringWorker::deleteLater);
    QObject::connect(workerThread, &QThread::finished, workerThread, &QThread::deleteLater);
    workerThread->start();

    QThread* networkThread = new QThread(&a);
    NetworkServer* networkServer = new NetworkServer();
    networkServer->moveToThread(networkThread);
    QObject::connect(networkThread, &QThread::started, networkServer, &NetworkServer::startServer);
    QObject::connect(&a, &QCoreApplication::aboutToQuit, networkThread, &QThread::quit);
    QObject::connect(networkThread, &QThread::finished, networkServer, &NetworkServer::deleteLater);
    QObject::connect(networkThread, &QThread::finished, networkThread, &QThread::deleteLater);
    networkThread->start();

    return a.exec();
}
