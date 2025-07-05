#include <QCoreApplication>
#include <QFileInfo>
#include <QDebug>
#include <QThread>

#include "appconfig.h"
#include "databasemanager.h"
#include "monitoringworker.h"

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
        qCritical() << "FATAL: Could not connect to the database. Exiting.";
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

    return a.exec();
}
