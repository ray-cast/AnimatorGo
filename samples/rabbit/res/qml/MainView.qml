import QtQuick 2.6
import QtQuick.Window 2.3
import QtQuick.Controls 2.2

ApplicationWindow {
	id: window
	flags: Qt.Window
	title: qsTr("兔叽渲染器")
	width: 800
	height: 600

	property alias timer: timer

	Rectangle {
		width: window.width
		height: window.height
	    objectName: "viewPlane"
	}

	Component.onCompleted: {
		$global.open("viewPlane")
		window.show();
	}

	Timer {
		id: timer
		running: true;
		onTriggered: {
			$global.update()
		}
	}
}