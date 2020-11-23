#ifndef UDPHandler_H
#define UDPHandler_H

#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QString>
#include <QObject>
#include <functional>
#include <QHostAddress>
#include <QTimer>
#include <QAbstractListModel>


struct deviceInfo {
    QHostAddress ip;
    QString name;
    bool mobile;
    quint16 port;
    bool operator==(const deviceInfo other);
};

Q_DECLARE_METATYPE(deviceInfo);

class UDPHandler : public QObject {
    Q_OBJECT
private:
    QUdpSocket* udp;
    void onMessage();
    bool mobile;
    QString name;
    QByteArray generate_response();
    QHostAddress localIP;
    quint16 tcpServerPort;
    QTimer timer;
    QList<QHostAddress> seenIPs;
public:
    UDPHandler(bool mobile, QString name, quint16 serverPort);
    ~UDPHandler();
public slots:
    void discover();

signals:
    void discovered(deviceInfo info);
    void offline(QHostAddress ip);
};

#endif // UDPHandler_H
