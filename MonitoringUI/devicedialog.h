#ifndef DEVICEDIALOG_H
#define DEVICEDIALOG_H

#include <QDialog>

#include "../common/device.h"

namespace Ui {
class DeviceDialog;
}

class DeviceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceDialog(QWidget *parent = nullptr);
    explicit DeviceDialog(const Device& deviceToEdit, QWidget *parent = nullptr);
    ~DeviceDialog();
    Device getDeviceData() const;

private slots:
    void on_btnSave_clicked();

private:
    Ui::DeviceDialog *ui;
    Device _device;
};

#endif // DEVICEDIALOG_H
