import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
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
            onClicked: {
                if (WordsProcessor.isProcessing) {
                    WordsProcessor.cancelProcessing();
                }

                fileDialog.open();
            }
        }
        Button {
            id: buttonStart

            width: scaleDimen(150)
            height: scaleDimen(50)
            enabled: false
            text: "Старт"
            onClicked: {
                if (!!fileDialog.selectedFile.toString()) {
                    wordsModel.clear();
                    WordsProcessor.loadFile(fileDialog.selectedFile);
                }
            }
        }
        Button {
            enabled: WordsProcessor.isProcessing
            width: scaleDimen(150)
            height: scaleDimen(50)
            text: "Пауза"
            onClicked: WordsProcessor.pauseProcessing()
        }
        Button {
            enabled: WordsProcessor.isProcessing
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
            topMargin: scaleDimen(30)
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
    Rectangle {
        id: scaleIndicator

        anchors {
            bottom: histogram.bottom
            right: histogram.left
            rightMargin: scaleDimen(50)
        }
        visible: !!repeaterHistogram.count
        height: presenter.barHeight + (presenter.maxHistogramLevel > presenter.maxCount
                                       ? (presenter.maxHistogramLevel - presenter.maxCount) * presenter.heightPerUnit
                                       : 0)
        width: scaleDimen(1)
        color: "black"

        Repeater {
            id: repeater

            anchors {
                bottom: parent.bottom
                right: parent.left
                rightMargin: scaleDimen(20)
            }
            delegate: Rectangle {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    bottom: parent.bottom
                    bottomMargin: index !== 0 ? index * scaleIndicator.height / (repeater.count - 1) - 0.5 * height : 0
                }
                color: "black"
                height: scaleDimen(1)
                width: scaleDimen(20)

                Text {
                    anchors {
                        baseline: parent.baseline
                        right: parent.left
                    }
                    text: index !== 0 ? presenter.maxHistogramLevel / (repeater.count - 1) * index : "0"
                }
                Rectangle {
                    height: scaleDimen(1)
                    width: histogram.width + scaleIndicator.anchors.rightMargin + repeater.anchors.rightMargin
                    color: "grey"
                }
            }
            model: (presenter.maxCount <= 10) ? presenter.maxCount + 1 : presenter.scaleDivisionsCount + 1
        }
    }
    Row {
        id: histogram

        anchors.centerIn: parent
        spacing: scaleDimen(10)

        Repeater {
            id: repeaterHistogram

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
                Text {
                    anchors {
                        bottom: parent.top
                        horizontalCenter: parent.horizontalCenter
                    }
                    text: model.count
                    font.pixelSize: scaleDimen(15)
                    color: "grey"
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
        property int maxCount
        property int maxHistogramLevel
        readonly property int scaleDivisionsCount: 4

        onMaxCountChanged: {
            if (maxCount < 10) {
                maxHistogramLevel = maxCount;
            } else {
                var divider = 1;
                for (var i = 0; i < maxCount.toString().length - 2; ++i) {
                    divider *= 10;
                }

                var levelNumber = Math.ceil(maxCount / divider) * divider;
                maxHistogramLevel = levelNumber + (scaleDivisionsCount - (levelNumber % scaleDivisionsCount)) % scaleDivisionsCount;
            }
        }
    }
    Connections {
        target: WordsProcessor

        function onProcessingCanceled() {
            wordsModel.clear();
            progressBar.value = 0;
        }
        function onProcessingFinished(wordsCount) {
            if (!wordsCount.length) {
                return;
            }

            presenter.maxCount = wordsCount[0].count;
            presenter.heightPerUnit = Qt.binding(function barHeightBinding() {
                return presenter.barHeight / presenter.maxCount;
            });
            wordsCount.forEach(function(word) {
                wordsModel.append({"word": word.word, "count": word.count});
            });
        }
        function onProgressChanged(progress) {
            progressBar.value = progress;
        }
    }
}
