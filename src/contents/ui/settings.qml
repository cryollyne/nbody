import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.20 as Kirigami

Kirigami.ScrollablePage {
    id: settingsPage

    function open() {
        tickRateField.value = canvas.tickRate;
        frameUpdateRateField.value = canvas.frameUpdateRate;
        pageStack.push(settingsPage);
    }

    function applyChanges() {
        canvas.tickRate = tickRateField.value;
        canvas.frameUpdateRate = frameUpdateRateField.value;
    }

    ColumnLayout {
        anchors.fill: parent

        Kirigami.FormLayout {
            Layout.alignment: Qt.AlignTop
            Controls.SpinBox {
                id: tickRateField
                Kirigami.FormData.label: "Simulation update rate"
            }
            Controls.SpinBox {
                id: frameUpdateRateField
                Kirigami.FormData.label: "Frame update rate"
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
