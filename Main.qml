import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs


Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    Button {
        text: "Choose file"
        onClicked: fileDialog.open()
    }
    FileDialog {
        id: fileDialog
    }
}
