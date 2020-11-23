#include "udphandler.h"
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QTimer>
#include <QObject>
#include <QString>
#include <QTextCodec>
#include <functional>
#include <QNetworkInterface>

const int UDP_PORT = 4947;

UDPHandler::UDPHandler(bool mobile, QString name, quint16 port){
    this->mobile = mobile;
    this->name = name;
    tcpServerPort = port;
    udp = new QUdpSocket(this);
    udp->bind(QHostAddress::AnyIPv4, UDP_PORT);

    connect(udp, &QUdpSocket::readyRead, this, &UDPHandler::onMessage);
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
             localIP = address;
    }
   discover();
    QTimer *timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, &UDPHandler::discover);
    timer->start(5000);

}

void UDPHandler::onMessage(){
    while (udp->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udp->receiveDatagram();
        if (datagram.senderAddress() == localIP)
            return;

        QString text = QString::fromUtf8(datagram.data());
        if (text == "offline") {
            emit offline(datagram.senderAddress());
        }
        else {
            QStringList list = text.split("\t");
            if (list.size() == 3) {
                bool mobile = list[0] == "m";
                QString name = list[1];
                quint16 port = list[2].toUShort();
                deviceInfo newDevice {
                    datagram.senderAddress(),
                    name,
                    mobile,
                    port
                };
                emit discovered(newDevice);
            }
        }

    }
}

void UDPHandler::discover(){
    QString deviceType(mobile ? "m" : "d");
    QByteArray message =  (deviceType + "\t" + name + "\t" + QString::number(tcpServerPort)).toUtf8();
    QNetworkDatagram datagram(message, QHostAddress(QHostAddress::Broadcast), UDP_PORT);
    udp->writeDatagram(datagram);
    udp->flush();
}

UDPHandler::~UDPHandler(){
    QByteArray message =  QByteArray("offline");
    QNetworkDatagram datagram(message, QHostAddress(QHostAddress::Broadcast), UDP_PORT);
    udp->writeDatagram(datagram);
    delete udp;
}

bool deviceInfo::operator==(const deviceInfo other)
{
    return ip == other.ip && name == other.name && mobile == other.mobile;
}
