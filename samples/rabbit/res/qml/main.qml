import QtQuick 2.7
import QtQuick.Window 2.3

QtObject {
    id: root
    objectName: "root"
    property QtObject $splashScreen: Splash {}

    property var loader: Loader{
        asynchronous: true
        source: "MainView.qml"
        active: false
        onLoaded: {
            $splashScreen.delay();
        }
    }

    Component.onCompleted:{
        loader.active = true;
    }
}