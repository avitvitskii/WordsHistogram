import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

import com.avitvitskii.wordsprocessor 1.0


Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Words Histogram")

    Button {
        text: "Choose file"
        onClicked: fileDialog.open()
    }
    FileDialog {
        id: fileDialog

        onAccepted: WordsProcessor.loadFile(fileDialog.selectedFile)
    }
    Connections {
        target: WordsProcessor

        function onProcessingFinished(wordsCount) {}
    }
}
