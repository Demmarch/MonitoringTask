#include "statuslogmodel.h"
#include <QColor>

StatusLogModel::StatusLogModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int StatusLogModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_logEntries.count();
}

int StatusLogModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_columnHeaders.count();
}

QVariant StatusLogModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_logEntries.count())
        return QVariant();

    const StatusLogEntry &entry = m_logEntries.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0: return entry.change_time.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
        case 1: return entry.dev_num;
        case 2: return statusToString(entry.old_status);
        case 3: return statusToString(entry.new_status);
        }
    }
    else if (role == Qt::BackgroundRole && index.column() == 3) {
        switch (entry.new_status) {
        case 0: return QColor("#E0E0E0");
        case 1: return QColor("#FFF59D");
        case 2: return QColor("#EF9A9A");
        case 3: return QColor("#A5D6A7");
        }
    }

    return QVariant();
}

QVariant StatusLogModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        return m_columnHeaders.value(section, "");
    }
    return QVariant();
}

void StatusLogModel::setLogEntries(const QList<StatusLogEntry> &entries)
{
    beginResetModel();
    m_logEntries = entries;
    endResetModel();
}

QString StatusLogModel::statusToString(int status) const
{
    switch (status) {
    case 0: return "Неизвестно";
    case 1: return "Не работает";
    case 2: return "Авария";
    case 3: return "Работает";
    default: return "Invalid Status";
    }
}
