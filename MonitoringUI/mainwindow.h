#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>

#include "../common/device.h"
#include "../common/statuslogentry.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class NetworkClient;
class DeviceTableModel;
class StatusLogModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnRequestDevices_clicked();
    void on_btnDeleteDevice_clicked();
    void on_btnAddNewDevice_clicked();
    void on_btnChangeDevice_clicked();

    void on_btnRequestLog_clicked();

    void onDevicesReceived(const QList<Device>& devices);
    void onStatusLogReceived(const QList<StatusLogEntry>& logEntries);
    void onRequestSucceeded(const QString& message);
    void onRequestFailed(const QString& message);

private:
    void setupLogTabControls();

    Ui::MainWindow *ui;
    NetworkClient* _networkClient;
    DeviceTableModel* _deviceModel;
    StatusLogModel* _logModel;
};
#endif // MAINWINDOW_H
