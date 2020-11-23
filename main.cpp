#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QtDebug>
#include <QStandardPaths>
#include <QBluetoothLocalDevice>
#include <QAbstractItemModel>
#include "udphandler.h"
#include "tcpserverhandler.h"
#include "devicelist.h"
#include "statusbar.h"
#include "tcpclienthandler.h"

#include <QQmlContext>
int main(int argc, char *argv[])
{
    bool mobile = false;
    QUrl url;
#if defined (Q_OS_ANDROID) || defined(Q_OS_IOS)
    url = QUrl("qrc:/mobile.qml");
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Round);
    mobile = true;
#else
    url = QUrl("qrc:/desktop.qml");
#endif
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QQuickStyle::setStyle("Material");


    qmlRegisterType<StatusBar>("StatusBar", 0, 1, "StatusBar");




    QBluetoothLocalDevice device;

    DeviceList list;
    TCPClientHandler clientHandler(&list);
    TCPServerHandler serverHandler(&list);
    UDPHandler udp(mobile, device.name(), serverHandler.getPort());


    QObject::connect(&udp, &UDPHandler::discovered, &list, &DeviceList::addDevice);
    QObject::connect(&udp, &UDPHandler::offline, &list, &DeviceList::removeDevice);

    engine.rootContext()->setContextProperty("deviceList", &list);
    engine.rootContext()->setContextProperty("clientHandler", &clientHandler);
    engine.rootContext()->setContextProperty("serverHandler", &serverHandler);

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url,&list](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
        QObject *destinationListView = obj->findChild<QObject*>("availableDestinations");
        destinationListView->setProperty("model", QVariant::fromValue(&list));
    }, Qt::QueuedConnection);

    engine.load(url);


    return app.exec();
}
