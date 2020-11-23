import QtQml.Models 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Dialogs 1.3
import QtGraphicalEffects 1.15
import StatusBar 0.1
import "MaterialIcons"

ApplicationWindow {
    FontLoader
    {
        id: roboto

        source: "qrc:/Roboto/Roboto-Regular.ttf"
    }
    visible: true
    id: window

    minimumWidth: 640*.8
    minimumHeight: 480*.8

    title: qsTr("LocalSend")
    Material.accent: Material.Red
    Material.primary: Material.Blue
    Material.theme: Material.System
    function clearSelection() {
        for (var i = 0; i < availableDestinations.contentItem.children.length; i++)
        {
            let child = availableDestinations.contentItem.children[i]
            if (child.objectName === "delegate") {
                  child.children[0].opacity = 0
            }
        }
    }
    onClosing: {
        if (pageWrapper.currentIndex == 1) {
            pageWrapper.currentIndex = 0
            fileDialogLabel.text = qsTr("No File Selected")
            fileDialogLabel.Material.foreground = Material.Grey
            clearSelection()
            availableDestinations.startSelected = false
            close.accepted = false;
        }
     }

    StatusBar {
        theme: Material.theme
        color: Material.color(Material.Blue, Material.Shade700)
    }

    ToolBar {
        id: toolBar
        x: 0
        y: 0
        width: parent.width
        height: 64

        ToolButton {
            MaterialIcon {
                name:"arrow_back"
                color: "white"
                anchors.centerIn: parent
            }
            visible: pageWrapper.currentIndex == 1
            onClicked: {
                pageWrapper.currentIndex = 0
                fileDialogLabel.text = qsTr("No File Selected")
                fileDialogLabel.Material.foreground = Material.Grey
                clearSelection()
                availableDestinations.startSelected = false

            }
            anchors.verticalCenter: parent.verticalCenter
        }
        Text {
            id: element
            x: 0
            y: 0
            width: parent.width
            height: 64
            text: "LocalSend"
            verticalAlignment: Text.AlignVCenter
            font.family: "Roboto"
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 30
            font.weight: Font.Normal
            color: "white"
        }


    }
    SwipeView {
        x:0
        y: toolBar.height
        width:window.width
        height: window.height - toolBar.height
        clip: true
        interactive: false
        currentIndex: 0
        id: pageWrapper
        //        enabled: false

        Pane {
            id: homeScreen
            width: pageWrapper.width
            height: pageWrapper.height


            Label {
                id: recentTransfersLabel
                text: qsTr("Recently downloaded files")
                font.weight: Font.Medium
                font.pointSize: 20
                font.family: "Roboto"
            }



            ListView {
                id: recentTransfers
                objectName: "recentTransfers"
                width: window.width - 32
                height: window.height - 64 - sendFAB.height - toolBar.height
                anchors.top: recentTransfersLabel.bottom
                onContentYChanged: scrollbar.visible = true
                boundsBehavior: Flickable.StopAtBounds
                clip: true
                //                required model
                ScrollBar.vertical: ScrollBar {
                    width: 10
                    contentItem: Rectangle {
                        id: scrollbar
                        radius: width/2
                        color: "grey"
                        visible: false

                    }
                }

                delegate: Item {
                    x: 5
                    width: 80
                    height: 50

                    Row {
                        anchors.fill:parent
                        id: row1
                        MaterialIcon {
                            id: fileIcon
                            name: "description"
                            color: (Material.theme) ? Material.color(Material.Grey, Material.Shade500) : Material.color(Material.Grey, Material.Shade700)
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        Item {
                            id: element1
                            transformOrigin: Item.Center

                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: fileIcon.right
                            anchors.leftMargin: 16
                            Label {
                                font.family: "Roboto"
                                id: fileNameLabel
                                text: modelData
                                anchors.verticalCenter: parent.verticalCenter
                                color: Material.foreground

                            }


                        }

                        spacing: 10
                    }

                }
            }

        }
        Pane {
            id: sendScreen
            width: pageWrapper.width
            height: pageWrapper.height
            Label {
                id: fileSectionHeader
                text: qsTr("File")
                font.pointSize: 20
                font.family: "Roboto"

            }
            Button {
                id: fileDialogButton
                text: qsTr("Select File")
                Material.background: Material.accent
                Material.foreground: "white"
                onClicked: fileDialog.open()
                anchors.top: fileSectionHeader.bottom
            }

            Label {
                id: fileDialogLabel
                text: qsTr("No File Selected")
                anchors.left: fileDialogButton.right
                width: parent.width - fileDialogButton.width - 16
//                anchors.right: parent.right
                elide: Text.ElideRight
//                clip: true
                Material.foreground: Material.Grey
                font.pointSize: 16
                font.family: "Roboto"
                anchors.leftMargin: 16
                anchors.verticalCenter: fileDialogButton.verticalCenter

            }

            Label {
                id: destSectionHeader
                text: qsTr("Destinations")
                font.pointSize: 20
                font.family: "Roboto"
                anchors.top: fileDialogButton.bottom
                anchors.topMargin: 8
            }
            FileDialog {
                id: fileDialog
                title: "Please choose a file"
                folder: shortcuts.home
                onAccepted: {
                    fileDialogLabel.text = clientHandler.fileURIToString(fileDialog.fileUrl.toString())
                    fileDialogLabel.Material.foreground = Material.Black
                }

            }

            ListView {
                id: availableDestinations
                objectName: "availableDestinations"
                width: window.width - 32
                height: window.height - 64 - sendFAB.height - toolBar.height
                anchors.top: destSectionHeader.bottom
                onContentYChanged: scrollbar2.visible = true
                boundsBehavior: Flickable.StopAtBounds
                property bool startSelected: false
                ScrollBar.vertical: ScrollBar {
                    width: 10
                    contentItem: Rectangle {
                        id: scrollbar2
                        radius: width/2
                        color: "grey"
                        visible: false

                    }
                }

                delegate: Item {
                    x: 5
                    width: window.width - 32
                    height: 50
                    objectName: "delegate"
                    Rectangle {
                        MouseArea {
                            id: mouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered: destRowBg.opacity = 0.05
                            onExited: if (!availableDestinations.startSelected || availableDestinations.currentIndex != index) destRowBg.opacity = 0
                            onPressed: {
                                availableDestinations.currentIndex = index
                                availableDestinations.startSelected = true
                                clearSelection()
                                destRowBg.opacity = 0.05
                            }

                        }
                        id: destRowBg
                        anchors.fill: parent
                        color: "black"
                        opacity: 0

                        Behavior on opacity  {

                            PropertyAnimation {
                                easing: Easing.InOutQuad
                                properties: "opacity"
                                duration: 100
                            }
                        }
                    }


                    Item {
                        anchors.fill:parent
                        MaterialIcon {
                            id: deviceIcon
                            name: mobile ? "smartphone" : "laptop"
                            color: (Material.theme) ? Material.color(Material.Grey, Material.Shade500) : Material.color(Material.Grey, Material.Shade700)
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            anchors.leftMargin: 8
                        }

                        Item {
                            id: deviceInfo
                            transformOrigin: Item.Center

                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: deviceIcon.right
                            anchors.leftMargin: 16
                            Label {
                                font.family: "Roboto"
                                id: deviceNameLabel
                                text: name
                                font.pointSize: 16
                                anchors.verticalCenterOffset: -8
                                anchors.verticalCenter: parent.verticalCenter
                                color: Material.foreground
                            }
                            Label {
                                font.family: "Roboto"
                                id: deviceIPLable
                                text: ip
                                font.pointSize: 12
                                anchors.verticalCenterOffset: 8
                                anchors.verticalCenter: parent.verticalCenter
                                Material.foreground: Material.Grey
                            }
                        }
//                        spacing: 10
                    }
                }

            }
        }

    }


    RoundButton {
        id: sendFAB
        x: window.width - this.width - 16
        y: window.height - this.height -  16
        width: 64
        height: 64
        transformOrigin: Item.Center
        objectName: "sendFAB"
        z: 999
        Material.background: Material.Red
        enabled: (pageWrapper.currentIndex == 0 || availableDestinations.startSelected && fileDialog.fileUrls.length === 1)
        MaterialIcon {
            id: sendFabIcon
            name: pageWrapper.currentIndex == 0 ? "call_made" : "send"
            anchors.centerIn: parent
            color: "white"
        }

        onClicked: {
            if (pageWrapper.currentIndex == 0) {
                pageWrapper.currentIndex = 1
            }
            else {
                clientHandler.handleButton(fileDialog.fileUrl, availableDestinations.currentIndex)
            }
        }
        Material.elevation:1
    }

    Popup {
        id: popup
        anchors.centerIn: parent
        width: window.width - 48
        height: 160
        modal: true
        focus: true
        padding: 0
        Connections {
            target: serverHandler
            onConnectionEvent:  {
                dialogLabel.text = `${deviceName} (${ip}) would like to share ${fileName} (${fileSize}) with you.`
                popup.visible = true
            }
        }

        property var textList: []
        property var socketList: []
        Connections {
            target: serverHandler
            onConnectionEvent:  {
                popup.textList.push(`${deviceName} (${ip}) would like to share ${fileName} (${fileSize}) with you.`)
                dialogLabel.text = popup.textList[0]
                popup.socketList.push(socketIdx)
                popup.visible = true
            }
        }

        Button {
            id: accept
            flat: true
            text: "Accept"
            font.family: "Roboto"
            Material.foreground: Material.primary
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.rightMargin: 8
            anchors.bottomMargin: 3
            onClicked: {
                serverHandler.rejectTransfer(popup.socketList.shift())
                popup.textList.shift()
                if (popup.textList.length === 0){
                    popup.visible = false
                }
            }
        }
        Button {
            id: decline
            flat: true
            text: "Decline"
            font.family: "Roboto"
            Material.foreground: Material.primary
            anchors.right: accept.left
            anchors.bottom: parent.bottom
            anchors.rightMargin: 8
            anchors.bottomMargin: 3
            onClicked: {
                serverHandler.rejectTransfer(popup.socketList.shift())
                popup.textList.shift()
                if (popup.textList.length === 0){
                    popup.visible = false
                }
            }
        }

        Column  {
            anchors.fill: parent
            padding: 24
            Label {
                id: title
                text: qsTr("Accept File")
                font.bold: true
                font.family: "Roboto"
                font.pointSize: 25
            }
            Label {
                id: dialogLabel
                font.family: "Roboto"
                width: popup.width - 48
                font.pointSize: 15
                color: "#666666"
                wrapMode: Text.WordWrap
            }
            spacing: 10
        }


    }
}







/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

