#include <QTcpSocket>
#include <QFile>
#include <QQmlFile>
#include <QUrl>
#include <QTcpSocket>
#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroidExtras/QtAndroid>
#endif
#include "tcpclienthandler.h"
#include "devicelist.h"
#include "udphandler.h"

TCPClientHandler::TCPClientHandler(DeviceList *list)
{
    this->list = list;
}

void TCPClientHandler::handleButton(QString fileURL, int index) {
    QUrl url(fileURL);
#ifdef Q_OS_ANDROID
    QFile *file = new QFile(QQmlFile::urlToLocalFileOrQrc(url));
    deviceInfo info = list->getDevice(index);
    sendToDevice(info.ip,info.port, file, fileURIToString(fileURL));
#else
    QFile *file = new QFile(url.toLocalFile());
    deviceInfo info = list->getDevice(index);
    sendToDevice(info.ip,info.port, file);
#endif

}

void TCPClientHandler::sendToDevice(QHostAddress address, quint16 port, QFile *file, QString fileName) {
    QTcpSocket *socket = new QTcpSocket(this);
    if (!file->open(QIODevice::ReadOnly))
        return;
    QString infoMessage;
    if (fileName != ""){
        infoMessage = (fileName + "\t" + QString::number(file->size()) + "\n");
    }
    else {
        infoMessage = (file->fileName().split("/").last() + "\t" + QString::number(file->size()) + "\n");
    }
    connect(socket, &QTcpSocket::connected, this, [infoMessage, socket]() {
        socket->write(infoMessage.toUtf8());
    });

    auto conn = std::make_shared<QMetaObject::Connection>();

    *conn = connect(socket, &QTcpSocket::readyRead, this, [socket, file, conn]() {
        QByteArray msg = socket->readAll().trimmed();
        if (msg == "ready"){
            while (!file->atEnd()){
                QByteArray arr = file->read(1400);
                if (file->atEnd()){
                    socket->write(arr);
                    break;
                }
                socket->write(arr);
            }
            socket->close();
            file->close();

            delete file;
            disconnect(*conn);
        }
        else if (msg =="reject"){
            socket->close();
            file->close();
            delete file;
            disconnect(*conn);
            return;
        }
    });

    connect(socket, &QTcpSocket::errorOccurred, this, [socket]() {
    });
    socket->connectToHost(address, port);
}


QString TCPClientHandler::fileURIToString(QString fileURI) {
#ifdef Q_OS_ANDROID
    if (fileURI.contains("com.android.externalstorage")){
        return  fileURI.split("%2F").last();
    }
    else
    {
        QAndroidJniObject contentResolver = QtAndroid::androidActivity().callObjectMethod("getContentResolver", "()Landroid/content/ContentResolver;");
        QAndroidJniObject uriString = QAndroidJniObject::fromString(fileURI);
        QAndroidJniObject uri = QAndroidJniObject::callStaticObjectMethod("android/net/Uri","parse","(Ljava/lang/String;)Landroid/net/Uri;",uriString.object<jstring>());
        QAndroidJniObject cursor = contentResolver.callObjectMethod("query", "(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)Landroid/database/Cursor;",
                                                                    uri.object<jobject>(), NULL, NULL, NULL, NULL);
        if (cursor != NULL) {
            QAndroidJniObject column = QAndroidJniObject::getStaticObjectField("android/provider/OpenableColumns", "DISPLAY_NAME", "Ljava/lang/String;");
            jint columnIndex = cursor.callMethod<jint>("getColumnIndex", "(Ljava/lang/String;)I", column.object<jstring>());
            cursor.callMethod<jboolean>("moveToFirst", "()Z");
            QAndroidJniObject result = cursor.callObjectMethod("getString", "(I)Ljava/lang/String;", columnIndex);
            return result.toString();
        }
    }
#endif
    return fileURI.split("/").last();
}
