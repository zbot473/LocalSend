#include "devicelist.h"

DeviceList::DeviceList()
{

}

int DeviceList::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return deviceList.size();
}

QVariant DeviceList::data(const QModelIndex &index, int role) const {
    if (index.row() < 0 || index.row() >= deviceList.size())
        return QVariant::Invalid;

    deviceInfo item = deviceList[index.row()];
    switch (role){
        case MobileRole:
            return QVariant::fromValue(item.mobile);
        case IPRole:
            return QVariant::fromValue(item.ip.toString());
        case NameRole:
            return QVariant::fromValue(item.name);
        case PortRole:
            return QVariant::fromValue(item.port);
    }
    return QVariant::Invalid;
}

void DeviceList::addDevice(deviceInfo device) {
    QListIterator<deviceInfo> iter(deviceList);
    if (deviceExists(device.ip)) return;
    beginInsertRows(QModelIndex(), rowCount(),rowCount());
    deviceList.append(device);
    endInsertRows();
}

bool DeviceList::removeDevice(QHostAddress ip){
    for (int i = 0; i < deviceList.size() ;i++ ) {
        if (deviceList[i].ip.isEqual(ip,QHostAddress::ConvertV4MappedToIPv4)) {
            beginRemoveRows(QModelIndex(), i,i);
            deviceList.removeAt(i);
            endRemoveRows();
            return true;
        }
    }
    return false;
}

bool DeviceList::deviceExists(QHostAddress ip){
    for (int i = 0; i < deviceList.size() ;i++ ) {
        if (deviceList[i].ip.isEqual(ip,QHostAddress::ConvertV4MappedToIPv4)) {
            return true;
        }
    }
    return false;
}

deviceInfo DeviceList::getDevice(int index) {
    if (index < 0 || index >= deviceList.size()){
        return deviceInfo {

        };
    }
    else
        return deviceList.at(index);
}


deviceInfo DeviceList::getDevice(int index, bool *ok) {
    if (index < 0 || index >= deviceList.size()){
        *ok = true;
        return deviceInfo {

        };
    }

    else {
        *ok = false;
        return deviceList.at(index);
    }
}

deviceInfo DeviceList::getDevice(QHostAddress ip) {
    for (int i = 0; i < deviceList.size() ;i++ ) {
        if (deviceList[i].ip.isEqual(ip,QHostAddress::ConvertV4MappedToIPv4)) {
            return deviceList[i];
        }
    }
    return deviceInfo {

    };
}


deviceInfo DeviceList::getDevice(QHostAddress ip, bool *ok) {
    for (int i = 0; i < deviceList.size() ;i++ ) {
        if (deviceList[i].ip.isEqual(ip,QHostAddress::ConvertV4MappedToIPv4)) {
            *ok = true;
            return deviceList[i];
        }
    }
    *ok = false;
    return deviceInfo{

    };
}


QHash<int, QByteArray> DeviceList::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[MobileRole] = "mobile";
    roles[IPRole] = "ip";
    roles[NameRole] = "name";
    roles[PortRole] = "port";
    return roles;
}
