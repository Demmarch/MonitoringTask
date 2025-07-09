#ifndef STATUSLOGENTRY_H
#define STATUSLOGENTRY_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

struct StatusLogEntry
{
    int log_id = 0;
    QString dev_num;
    QDateTime change_time;
    int old_status = 0;
    int new_status = 0;

    static StatusLogEntry fromJson(const QJsonObject& json)
    {
        StatusLogEntry entry;
        entry.log_id = json.value("log_id").toInt();
        entry.dev_num = json.value("dev_num").toString();
        if (json.contains("change_time") && json["change_time"].isString()) {
            entry.change_time = QDateTime::fromString(json["change_time"].toString(), Qt::ISODate).toLocalTime();
        }
        entry.old_status = json.value("old_status").toInt();
        entry.new_status = json.value("new_status").toInt();
        return entry;
    }
};

#endif // STATUSLOGENTRY_H
