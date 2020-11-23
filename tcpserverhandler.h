#ifndef TCPServerHandler_H
#define TCPServerHandler_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "devicelist.h"
#include <QAtomicInt>

struct connectionInfo {
    QHostAddress ip;
    QString fileName;
    quint64 fileSize;
};

class TCPServerHandler : public QObject
{
    Q_OBJECT
public:
    TCPServerHandler(DeviceList *list);
    quint16 getPort();
    Q_INVOKABLE void acceptTransfer(int id);
    void stopServer();
    //    ~TCPServerHandler();
    Q_INVOKABLE void rejectTransfer(int id);
private:
    QTcpServer server;
    QList<QTcpSocket*> socketVector;
    QList<connectionInfo> infoVector;
    quint16 socketIdx = 0;
    quint16 localPort = 8950;
    DeviceList *list;
    QAtomicInt remaining_size, current_size, size_at_last_emit, chunk_size;
signals:
    void connectionEvent(QString fileName, QString fileSize, QString deviceName, QString ip, int socketIdx);
    void initError(QString reason);
    void transferError(QString reason);
    void update_progress(float progress);
private slots:
    void onConnect();
    void handleTransfer();
};




#endif // TCPServerHandler_H

