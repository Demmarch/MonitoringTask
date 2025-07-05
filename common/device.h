#ifndef DEVICE_H
#define DEVICE_H

#include <QString>
#include <QJsonObject>

class Device
{
public:
    Device();

    int id;
    QString dev_num;
    QString dev_name;
    QString dev_description;
    int dev_status;

    QJsonObject toJson() const;

    static Device fromJson(const QJsonObject &json);
};

#endif // DEVICE_H
