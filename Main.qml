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

    readonly property real __scalingFactor: root.width / 1920

    function scaleDimen(px) {
        return Math.round(px * __scalingFactor);
    }

    Row {
        id: row

        spacing: scaleDimen(5)

        Button {
            width: scaleDimen(150)
            height: scaleDimen(50)
            text: "Открыть"
            onClicked: fileDialog.open()
        }
        Button {
            id: buttonStart

            width: scaleDimen(150)
            height: scaleDimen(50)
            enabled: false
            text: "Старт"
            onClicked: {
                if (!!fileDialog.selectedFile.toString()) {
                    presenter.isProcessing = true;
                    WordsProcessor.loadFile(fileDialog.selectedFile);
                }
            }
        }
        Button {
            enabled: presenter.isProcessing
            width: scaleDimen(150)
            height: scaleDimen(50)
            text: "Отмена"
            onClicked: WordsProcessor.cancelProcessing()
        }
    }
    ProgressBar {
        id: progressBar

        anchors {
            top: row.bottom
            horizontalCenter: parent.horizontalCenter
        }
        width: 0.5 * parent.width
        height: scaleDimen(20)
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
        spacing: scaleDimen(10)

        Repeater {
            delegate: Rectangle {
                anchors.bottom: parent.bottom
                width: scaleDimen(70)
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
                    font {
                        pixelSize: scaleDimen(30)
                    }
                }
            }
            model: ListModel {
                id: wordsModel
            }
        }
    }
    QtObject {
        id: presenter

        property int barHeight: scaleDimen(500)
        property real heightPerUnit
        property bool isProcessing: false
    }
    Connections {
        target: WordsProcessor

        function onProcessingCanceled() {
            presenter.isProcessing = false;
            wordsModel.clear();
            progressBar.value = 0;
        }
        function onProcessingFinished(wordsCount) {
            if (!wordsCount.length) {
                return;
            }

            presenter.heightPerUnit = Qt.binding(function barHeightBinding() {
                return presenter.barHeight / wordsCount[0].count;
            });
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
