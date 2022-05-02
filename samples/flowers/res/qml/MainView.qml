import QtQuick 2.6
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

ApplicationWindow {
	id: window
	title: qsTr("兔叽渲染器(内测版)")
	minimumWidth: 1280
	minimumHeight: 752
	flags: Qt.Window | Qt.FramelessWindowHint

	property alias timer: timer
    property bool isMoveWindow: false
    property point rootWindowTitleMousePos: Qt.point(x,y)

	onTitleChanged: titleText.text = title

	ColumnLayout {
		Rectangle {
			id : titleWindow
			width : window.width
			height : 32
			Layout.fillWidth : window.width

			color: "#1e1e1e"

			Image {
                id: logo
                source: "images/rabbit.png"
                sourceSize.width : 24
                sourceSize.height : 24
                anchors.leftMargin : 10
                anchors.left: titleWindow.left
                anchors.verticalCenter: parent.verticalCenter
            }

            Text {
            	id : titleText
                text: window.title
                color:"#c8c8c8"
                font.pixelSize : 12
                font.family: "Microsoft YaHei"
                anchors.leftMargin : 5
                anchors.left: logo.right
                anchors.verticalCenter: parent.verticalCenter
            }

 			MouseArea {
	            anchors.fill: titleWindow
	            acceptedButtons: Qt.LeftButton

	            onPressed: {
	                window.rootWindowTitleMousePos = Qt.point(mouseX,mouseY)
	                window.isMoveWindow = true
	            }

	            onReleased: {
	                if (mouse.button === Qt.LeftButton){
	                    window.isMoveWindow = false
	                }
	            }

	            onMouseXChanged: {
	                if (window.isMoveWindow) {
	                    window.x += mouseX - window.rootWindowTitleMousePos.x;
	                }
	            }
	            onMouseYChanged: {
	                window.y += mouseY - window.rootWindowTitleMousePos.y;
	            }
	        }

			ToolButton {
                id: close
                width : titleWindow.height
                height : titleWindow.height
                icon.color: "transparent"
                icon.width : titleWindow.height
                icon.height : titleWindow.height
                icon.source: "images/close.png"
                anchors.rightMargin : 5
                anchors.right: titleWindow.right
                anchors.verticalCenter: parent.verticalCenter
				background: Rectangle {
					color :"transparent"
				}
				onClicked: {
                    Qt.quit()
                }
            }

			ToolButton {
                id: maximize
                enabled: false
                width : titleWindow.height
                height : titleWindow.height
                icon.color: "transparent"
                icon.width : titleWindow.height
                icon.height : titleWindow.height
                icon.source: "images/maximize.png"
                anchors.rightMargin : 5
                anchors.right: close.left
                anchors.verticalCenter: parent.verticalCenter
				background: Rectangle {
					color :"transparent"
				}
				onClicked: {
                    if (window.visibility === Window.Maximized)
                    	window.visibility = Window.Windowed
                    else
                    	window.visibility = Window.Maximized
                }
            }

			ToolButton {
                id: minimize
                width : titleWindow.height
                height : titleWindow.height
                icon.source: "images/minimize.png"
                icon.width : titleWindow.height
                icon.height : titleWindow.height
                icon.color: "transparent"
                anchors.rightMargin : 5
                anchors.right: maximize.left
                anchors.verticalCenter: parent.verticalCenter
				background: Rectangle {
					color :"transparent"
				}
				onClicked: {
                    window.visibility = Window.Minimized
                }
            }

			ToolButton {
                id: setting
                icon.source: "images/setting.png"
                width : titleWindow.height
                height : titleWindow.height
                icon.width : titleWindow.height
                icon.height : titleWindow.height
                icon.color: "transparent"
                anchors.rightMargin : 5
                anchors.right: minimize.left
                anchors.verticalCenter: parent.verticalCenter
				background: Rectangle {
					color :"transparent"
				}
            }
		}

		Rectangle {
			Layout.preferredWidth : 1280
			Layout.preferredHeight: 720
			objectName: "viewPlane"
		}
	}

	Component.onCompleted: {
		$global.open("viewPlane")
		window.show();
		timer.start()
	}

	Timer {
		id: timer
		running: false;
		onTriggered: {
			$global.update()
		}
	}
}