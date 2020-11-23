#ifndef TCPCLIENTHANDLER_H
#define TCPCLIENTHANDLER_H

#include <QObject>
#include <QFile>
#include "devicelist.h"
#include <QTcpSocket>

class TCPClientHandler : public QObject
{
    Q_OBJECT
public:
    TCPClientHandler(DeviceList *list);
    Q_INVOKABLE QString fileURIToString(QString fileURI);
private:
    DeviceList *list;
    void sendToDevice(QHostAddress address, quint16 port, QFile *file, QString fileName="");
public slots:
    void handleButton(QString fileURL, int index);

};

#endif // TCPCLIENTHANDLER_H
