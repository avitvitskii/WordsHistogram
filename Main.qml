import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

import com.avitvitskii.wordsprocessor 1.0


Window {
    width: 1366
    height: 768
    visible: true
    title: qsTr("Words Histogram")

    Row {
        Button {
            text: "Открыть"
            onClicked: fileDialog.open()
        }
        Button {
            text: "Старт"
            onClicked: {
                if (!!fileDialog.selectedFile.toString()) {
                    WordsProcessor.loadFile(fileDialog.selectedFile);
                }
            }
        }
    }
    FileDialog {
        id: fileDialog
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

        property int barHeight: 500
        property real heightPerUnit
    }
    Connections {
        target: WordsProcessor

        function onProcessingFinished(wordsCount) {
            wordsModel.clear();
            presenter.heightPerUnit = presenter.barHeight / wordsCount[0].count;
            wordsCount.forEach(function(word) {
                wordsModel.append({"word": word.word, "count": word.count});
            });
        }
    }
}
