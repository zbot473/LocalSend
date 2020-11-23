#include "tcpserverhandler.h"

#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QDataStream>
#include <QLocale>

TCPServerHandler::TCPServerHandler(DeviceList *list)
{
    this->list = list;

    if (!server.listen(QHostAddress::Any,localPort)){
        while (!server.listen(QHostAddress::Any,localPort)) {
            localPort++;
        }
    }
    connect(&server, &QTcpServer::newConnection, this, &TCPServerHandler::onConnect);
    connect(this, &TCPServerHandler::update_progress, this, [](float progress){
        qDebug() << "progress: " << progress;
    });
}


quint16 TCPServerHandler::getPort() {
    return localPort;
}


void TCPServerHandler::onConnect(){
    QTcpSocket *socket = server.nextPendingConnection();
        QLocale locale(QLocale::English);
    auto conn = std::make_shared<QMetaObject::Connection>();


    *conn = connect(socket, &QTcpSocket::readyRead, this, [this, socket, conn,locale]() {
        QString raw(socket->readAll().trimmed());
        connectionInfo info;
        QStringList strList = raw.split("\t");
        if (strList.length() == 2) {
            info.ip = socket->peerAddress();

            info.fileName = strList[0];
            bool ok = true;

            info.fileSize = strList[1].toULongLong(&ok);

            qDebug() << "file size: " << info.fileSize;
            this->remaining_size = info.fileSize;
            this->chunk_size = info.fileSize / 250;
            this->current_size = 0;
            this->size_at_last_emit = 0;
            if (!ok){
                emit initError("Unable to read file size.");
                return;
            }

            deviceInfo senderInfo = list->getDevice(info.ip, &ok);
            if (!ok){
                emit initError("Device not ready yet");
                return;
            }
            infoVector.append(info);
            socketVector.append(socket);
            QString fileSize = locale.formattedDataSize(info.fileSize,2, QLocale::DataSizeTraditionalFormat);
            QString ip = QHostAddress(info.ip.toIPv4Address()).toString();
            emit connectionEvent(info.fileName, fileSize, senderInfo.name, ip, socketIdx);
            disconnect(*conn);
            socketIdx++;
        }
        else {
            emit initError("Unable to read handshake message.");
        }
    });
}


void TCPServerHandler::acceptTransfer(int id) {
    QTcpSocket *socket = socketVector[id];
    connectionInfo info = infoVector[id];
    QString path = QDir(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)).filePath(info.fileName);
    if (QFile::exists(path))
    {
        int x = 1;
        int leftDotIdx = path.lastIndexOf(".");
        int rightDotIdx = path.size() - leftDotIdx;
        QString pathRename = path.left(leftDotIdx) + " ("+QString::number(x)+")" + path.right(rightDotIdx);
        while (QFile::exists(pathRename)){
            pathRename = path.left(leftDotIdx) + " ("+QString::number(x)+")" + path.right(rightDotIdx);
            x++;
        }
        path = pathRename;
    }
    QFile *file = new QFile(path);

    if (!file->open(QIODevice::WriteOnly)) return;
    socket->write(QString("ready").toUtf8());
    auto conn = std::make_shared<QMetaObject::Connection>();

    *conn = connect(socket, &QTcpSocket::readyRead, this, [this, socket, file, conn]() {
        QByteArray buffer = socket->readAll();
        this->current_size += buffer.length();
        if(this->current_size - this->size_at_last_emit >= this->chunk_size){
            this->size_at_last_emit = this->current_size;
            emit update_progress(((float)current_size) / ((float)this->remaining_size));
        }
        //file->write(buffer);
    });

    connect(socket, &QTcpSocket::disconnected, this, [file, conn, this](){
        file->close();
        delete file;
        disconnect(*conn);
        emit update_progress(1.0f);
        qDebug() << "disconnected";
    });


}

void TCPServerHandler::stopServer() {
    server.pauseAccepting();
}


void TCPServerHandler::handleTransfer() {

}

void TCPServerHandler::rejectTransfer(int id) {
    QTcpSocket *socket = socketVector[id];
    connectionInfo info = infoVector[id];
    socket->write(QString("reject").toUtf8());
}

