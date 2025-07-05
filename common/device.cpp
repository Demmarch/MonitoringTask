#include "device.h"

Device::Device() {}

QJsonObject Device::toJson() const
{
    QJsonObject json;
    json["id"] = this->id;
    json["dev_num"] = this->dev_num;
    json["dev_name"] = this->dev_name;
    json["dev_description"] = this->dev_description;
    json["dev_status"] = this->dev_status;
    return json;
}

Device Device::fromJson(const QJsonObject &json)
{
    Device device;
    if (json.contains("id")) device.id = json["id"].toInt();
    if (json.contains("dev_num")) device.dev_num = json["dev_num"].toString();
    if (json.contains("dev_name")) device.dev_name = json["dev_name"].toString();
    if (json.contains("dev_description")) device.dev_description = json["dev_description"].toString();
    if (json.contains("dev_status")) device.dev_status = json["dev_status"].toInt();
    return device;
}
