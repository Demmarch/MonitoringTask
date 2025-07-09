#ifndef DEVICETABLEMODEL_H
#define DEVICETABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>

#include "../common/device.h"

class DeviceTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit DeviceTableModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void setDevices(const QList<Device> &devices);

    Device getDevice(int row) const;

private:
    const QStringList _columnHeaders = {"Серийный номер", "Наименование", "Описание", "Статус"};
    QList<Device> _devices;
};

#endif // DEVICETABLEMODEL_H
