#ifndef STATUSLOGMODEL_H
#define STATUSLOGMODEL_H

#include <QAbstractTableModel>
#include "../common/statuslogentry.h"

class StatusLogModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit StatusLogModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void setLogEntries(const QList<StatusLogEntry> &entries);

private:
    QString statusToString(int status) const;
    const QStringList m_columnHeaders = {"Время", "Серийный номер", "Старое состояние", "Новое состояние"};
    QList<StatusLogEntry> m_logEntries;
};

#endif // STATUSLOGMODEL_H
