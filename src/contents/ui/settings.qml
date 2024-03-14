import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.20 as Kirigami

Kirigami.ScrollablePage {
    id: settingsPage

    function open() {
        tickRateField.text = canvas.tickRate;
        frameUpdateRateField.text = canvas.frameUpdateRate;
        pageStack.push(settingsPage);
    }

    function applyChanges() {
        canvas.tickRate = tickRateField.text;
        canvas.frameUpdateRate = frameUpdateRateField.text;
    }

    ColumnLayout {
        anchors.fill: parent

        Kirigami.FormLayout {
            Layout.alignment: Qt.AlignTop
            Controls.TextField {
                id: tickRateField
                Kirigami.FormData.label: "Simulation update rate"
                validator: DoubleValidator{bottom: 1}
            }
            Controls.TextField {
                id: frameUpdateRateField
                Kirigami.FormData.label: "Frame update rate"
                validator: DoubleValidator{bottom: 1}
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignBottom | Qt.AlignRight
            Controls.Button {
                icon.name: "checkmark"
                text: "Ok"
                onClicked: {
                    applyChanges();
                    pageStack.pop();
                }
            }
            Controls.Button {
                icon.name: "checkmark"
                text: "Apply"
                onClicked: applyChanges()
            }
            Controls.Button {
                icon.name: "dialog-cancel"
                text: "Cancel"
                onClicked: pageStack.pop()
            }
        }
    }

}
