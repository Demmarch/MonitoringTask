#include "devicetablemodel.h"
#include <QColor>

DeviceTableModel::DeviceTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int DeviceTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return _devices.count();
}

int DeviceTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return _columnHeaders.count();
}

QVariant DeviceTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= _devices.count())
        return QVariant();

    const Device &device = _devices.at(index.row());

    // Возвращаем данные в зависимости от роли
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0: return device.dev_num;
        case 1: return device.dev_name;
        case 2: return device.dev_description;
        case 3: // Преобразуем статус в читаемый текст
            switch (device.dev_status) {
            case 0: return "Неизвестно";
            case 1: return "Не работает";
            case 2: return "Авария";
            case 3: return "Работает";
            default: return "Invalid Status";
            }
        }
    }
    // Возвращаем цвет фона для всей строки в зависимости от статуса
    else if (role == Qt::BackgroundRole) {
        switch (device.dev_status) {
        case 0: return QColor("#E0E0E0"); // Серый
        case 1: return QColor("#FFF59D"); // Желтый
        case 2: return QColor("#EF9A9A"); // Красный
        case 3: return QColor("#A5D6A7"); // Зеленый
        default: return QVariant();
        }
    }

    return QVariant();
}

QVariant DeviceTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        return _columnHeaders.value(section, "");
    }
    return QVariant();
}

void DeviceTableModel::setDevices(const QList<Device> &devices)
{
    beginResetModel();
    _devices = devices;
    endResetModel();
}

Device DeviceTableModel::getDevice(int row) const
{
    if (row >= 0 && row < _devices.count()) {
        return _devices.at(row);
    }
    return Device();
}
