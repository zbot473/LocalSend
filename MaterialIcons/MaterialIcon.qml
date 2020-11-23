import QtQuick 2.15

Item {
    property int size: 24
    property string name
    property color color

    width: size
    height: size

    FontLoader {
        id: materialIcons
        source: "MaterialIcons-Regular.ttf"
    }

    Text {
        anchors.fill: parent
        color: parent.color
        font.family: materialIcons.name
        font.pixelSize: parent.height
        text: parent.name
    }

}
