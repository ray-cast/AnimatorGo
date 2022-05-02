import QtQuick 2.7
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Window {
    id: splash
    title: "Splash Window"
    modality: Qt.ApplicationModal
    flags: Qt.SplashScreen | Qt.WindowStaysOnTopHint
    width: 500
    height: 300
    color: "#1e1e1e"
    x: (Screen.width - splash.width) / 2
    y: (Screen.height - splash.height) / 2

    ColumnLayout {
        width: splash.width
        height: splash.height

        Text {
            Layout.rightMargin : 10
            Layout.alignment: Qt.AlignRight
            id: version;
            text: "Version: 1.0.0"
            color:"#a5a5a5"
            font.pixelSize : 12
            font.family: "Microsoft YaHei"
        }

        RowLayout {
            Layout.minimumWidth : 400
            Layout.minimumHeight :  100
            Layout.maximumWidth : 400
            Layout.maximumHeight :  100
            Layout.alignment: Qt.AlignCenter
            spacing: 20

            Image {
                id: logo
                source: "images/splash.png"
                sourceSize.width : 110
                sourceSize.height : 110
                Layout.alignment: Qt.AlignRight
            }

            ColumnLayout  {
                spacing: 10
                Layout.alignment: Qt.AlignTop
                Layout.topMargin : 10

                Text {
                    text: "Rabbit Toolbox 2021"
                    color:"#c78ced"
                    font.pixelSize : 18
                    font.family: "Microsoft YaHei"
                    Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                }

                Text {
                    text: "© 2019-2021 Rabbit-Softwares. All Rights Reserved."
                    color:"#a5a5a5"
                    font.pixelSize : 12
                    font.family: "Microsoft YaHei"
                    Layout.alignment: Qt.AlignLeft
                }
            }
        }

        Text {
            objectName : "loadingMessage"
            text: "Loading..."
            width : 500
            color:"#a5a5a5"
            font.pixelSize : 12
            font.family: "Microsoft YaHei"
            Layout.leftMargin : 10
            Layout.alignment: Qt.AlignLeft
        }
    }

    function delay(){
        splash.visible = false;
    }

    Component.onCompleted: visible = true
}