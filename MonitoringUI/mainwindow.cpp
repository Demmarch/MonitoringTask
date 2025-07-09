#include <QMessageBox>
#include <QHBoxLayout>
#include <QDateTimeEdit>
#include <QLabel>
#include <QHeaderView>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "networkclient.h"
#include "devicetablemodel.h"
#include "statuslogmodel.h"
#include "devicedialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _networkClient = new NetworkClient(this);
    _deviceModel = new DeviceTableModel(this);
    _logModel = new StatusLogModel(this);

    ui->tblvwDevices->setModel(_deviceModel);
    ui->tblvwDevices->horizontalHeader()->setStretchLastSection(true);
    ui->tblvwDevices->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblvwDevices->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tblvwDevices->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tblvwLog->setModel(_logModel);
    ui->tblvwLog->horizontalHeader()->setStretchLastSection(true);
    ui->tblvwLog->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblvwLog->setEditTriggers(QAbstractItemView::NoEditTriggers);
    setupLogTabControls();

    connect(_networkClient, &NetworkClient::devicesReceived, this, &MainWindow::onDevicesReceived);
    connect(_networkClient, &NetworkClient::statusLogReceived, this, &MainWindow::onStatusLogReceived);
    connect(_networkClient, &NetworkClient::requestSucceeded, this, &MainWindow::onRequestSucceeded);
    connect(_networkClient, &NetworkClient::requestFailed, this, &MainWindow::onRequestFailed);

    on_btnRequestDevices_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupLogTabControls()
{
    QHBoxLayout* controlsLayout = new QHBoxLayout();

    QDateTimeEdit* dtFrom = new QDateTimeEdit(QDateTime::currentDateTime().addDays(-1));
    dtFrom->setObjectName("dtFrom");
    dtFrom->setCalendarPopup(true);
    dtFrom->setDisplayFormat("dd.MM.yyyy hh:mm");

    QDateTimeEdit* dtTo = new QDateTimeEdit(QDateTime::currentDateTime());
    dtTo->setObjectName("dtTo");
    dtTo->setCalendarPopup(true);
    dtTo->setDisplayFormat("dd.MM.yyyy hh:mm");

    QPushButton* btnRequestLog = new QPushButton("Запросить журнал");
    connect(btnRequestLog, &QPushButton::clicked, this, &MainWindow::on_btnRequestLog_clicked);

    controlsLayout->addWidget(new QLabel("С:"));
    controlsLayout->addWidget(dtFrom);
    controlsLayout->addWidget(new QLabel("По:"));
    controlsLayout->addWidget(dtTo);
    controlsLayout->addStretch();
    controlsLayout->addWidget(btnRequestLog);

    ui->verticalLayout_6->insertLayout(0, controlsLayout);
}

void MainWindow::on_btnRequestDevices_clicked()
{
    _networkClient->requestAllDevices();
}

void MainWindow::on_btnDeleteDevice_clicked()
{
    const QModelIndex currentIndex = ui->tblvwDevices->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "Внимание", "Пожалуйста, выберите устройство для удаления.");
        return;
    }
    Device selectedDevice = _deviceModel->getDevice(currentIndex.row());
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Подтверждение удаления",
                                                              QString("Вы уверены, что хотите удалить устройство '%1' (%2)?")
                                                                  .arg(selectedDevice.dev_name, selectedDevice.dev_num),
                                                              QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        _networkClient->requestDeleteDevice(selectedDevice.dev_num);
    }
}

void MainWindow::on_btnAddNewDevice_clicked()
{
    DeviceDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        Device newDevice = dialog.getDeviceData();
        _networkClient->requestAddDevice(newDevice);
    }
}

void MainWindow::on_btnChangeDevice_clicked()
{
    const QModelIndex currentIndex = ui->tblvwDevices->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "Внимание", "Пожалуйста, выберите устройство для изменения.");
        return;
    }
    Device deviceToEdit = _deviceModel->getDevice(currentIndex.row());
    DeviceDialog dialog(deviceToEdit, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        Device updatedDevice = dialog.getDeviceData();
        _networkClient->requestEditDevice(updatedDevice);
    }
}

void MainWindow::on_btnRequestLog_clicked()
{
    QDateTime from = this->findChild<QDateTimeEdit*>("dtFrom")->dateTime();
    QDateTime to = this->findChild<QDateTimeEdit*>("dtTo")->dateTime();
    _networkClient->requestStatusLog(from, to);
}

void MainWindow::onDevicesReceived(const QList<Device> &devices)
{
    _deviceModel->setDevices(devices);
}

void MainWindow::onStatusLogReceived(const QList<StatusLogEntry> &logEntries)
{
    _logModel->setLogEntries(logEntries);
    statusBar()->showMessage(QString("Загружено %1 записей журнала.").arg(logEntries.count()), 4000);
}

void MainWindow::onRequestSucceeded(const QString &message)
{
    QMessageBox::information(this, "Успех", message);
    on_btnRequestDevices_clicked();
}

void MainWindow::onRequestFailed(const QString &message)
{
    QMessageBox::critical(this, "Ошибка", message);
}
