#include "devicedialog.h"
#include "ui_devicedialog.h"
#include <QMessageBox>

DeviceDialog::DeviceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceDialog)
{
    ui->setupUi(this);

    connect(ui->btCancel, &QPushButton::clicked, this, &QDialog::reject);

    setModal(true);
}

DeviceDialog::DeviceDialog(const Device &deviceToEdit, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceDialog)
{
    ui->setupUi(this);
    setWindowTitle("Изменение устройства");
    ui->lbTitle->setText("Изменение устройства");

    ui->ldDevNum->setText(deviceToEdit.dev_num);
    ui->ldDevName->setText(deviceToEdit.dev_name);
    ui->txtDevDesc->setPlainText(deviceToEdit.dev_description);

    ui->ldDevNum->setReadOnly(true);
}

DeviceDialog::~DeviceDialog()
{
    delete ui;
}

Device DeviceDialog::getDeviceData() const
{
    return _device;
}

void DeviceDialog::on_btnSave_clicked()
{
    const QString devNum = ui->ldDevNum->text().trimmed();
    const QString devName = ui->ldDevName->text().trimmed();
    const QString devDesc = ui->txtDevDesc->toPlainText().trimmed();

    if (devNum.isEmpty() || devName.isEmpty()) {
        QMessageBox::warning(this, "Ошибка ввода", "Поля 'Серийный номер' и 'Наименование' являются обязательными для заполнения.");
        return;
    }

    _device.dev_num = devNum;
    _device.dev_name = devName;
    _device.dev_description = devDesc;
    accept();
}
