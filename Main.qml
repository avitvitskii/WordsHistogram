import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

import com.avitvitskii.wordsprocessor 1.0


Window {
    id: root

    width: 1366
    height: 768
    visible: true
    title: qsTr("Words Histogram")

    Row {
        id: row

        spacing: 5

        Button {
            text: "Открыть"
            onClicked: fileDialog.open()
        }
        Button {
            id: buttonStart

            enabled: false
            text: "Старт"
            onClicked: {
                if (!!fileDialog.selectedFile.toString()) {
                    presenter.isProcessing = true;
                    WordsProcessor.loadFile(fileDialog.selectedFile);
                }
            }
        }
    }
    ProgressBar {
        id: progressBar

        anchors {
            top: row.bottom
            horizontalCenter: parent.horizontalCenter
        }
        width: 0.5 * parent.width
        height: 20
        from: 0
        to: 100
        value: 0
    }
    FileDialog {
        id: fileDialog

        onAccepted: {
            buttonStart.enabled = true;
            wordsModel.clear();
            progressBar.value = 0;
        }
    }
    Row {
        anchors.centerIn: parent
        spacing: 10

        Repeater {
            delegate: Rectangle {
                anchors.bottom: parent.bottom
                width: 50
                height: presenter.heightPerUnit * model.count
                color: "cyan"

                Text {
                    anchors {
                        top: parent.bottom
                        topMargin: 0.5 * implicitWidth
                        horizontalCenter: parent.horizontalCenter
                    }
                    rotation: -90
                    text: model.word
                }
            }
            model: ListModel {
                id: wordsModel
            }
        }
    }
    QtObject {
        id: presenter

        readonly property int barHeight: 500
        property real heightPerUnit
        property bool isProcessing: false
    }
    Connections {
        target: WordsProcessor

        function onProcessingFinished(wordsCount) {
            presenter.heightPerUnit = presenter.barHeight / wordsCount[0].count;
            wordsCount.forEach(function(word) {
                wordsModel.append({"word": word.word, "count": word.count});
            });
            presenter.isProcessing = false;
            buttonStart.enabled = false;
        }
        function onProgressChanged(progress) {
            progressBar.value = progress;
        }
    }
}
