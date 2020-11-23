#ifndef DEVICELIST_H
#define DEVICELIST_H

#include <QAbstractListModel>
#include <QHostAddress>
#include "udphandler.h"

class DeviceList : public QAbstractListModel
{
    Q_OBJECT
public:
    DeviceList();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    enum roles {
        IPRole = Qt::UserRole + 1,
        NameRole,
        MobileRole,
        PortRole
    };
    bool removeDevice(QHostAddress ip);
    deviceInfo getDevice(int index);
    deviceInfo getDevice(int index, bool *ok);
    deviceInfo getDevice(QHostAddress ip);
    deviceInfo getDevice(QHostAddress ip, bool *ok);
    bool deviceExists(QHostAddress ip);
private:
    QList<deviceInfo> deviceList;
protected:
    QHash<int, QByteArray> roleNames() const;
public slots:
    void addDevice(deviceInfo device);

};


#endif // DEVICELIST_H
