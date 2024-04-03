import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.20 as Kirigami

Kirigami.ScrollablePage {
    id: settingsPage

    function open() {
        tickRateField.text = canvas.tickRate;
        frameUpdateRateField.text = canvas.frameUpdateRate;
        objectUpdateRateField.text = canvas.objectUpdateRate;
        cameraSensitivitySlider.value = canvas.sensitivity;
        cameraInvertCheckBox.checked = canvas.cameraInvert;
        cameraZoomSensitivitySlider.value = canvas.zoomSensitivity;
        cameraZoomInvertCheckBox.checked = canvas.zoomInvert;
        orthographicField.checked = canvas.orthographic;
        fovField.text = canvas.fov;
        pageStack.push(settingsPage);
    }

    function applyChanges() {
        canvas.tickRate = tickRateField.text;
        canvas.frameUpdateRate = frameUpdateRateField.text;
        canvas.objectUpdateRate = objectUpdateRateField.text;
        canvas.sensitivity = cameraSensitivitySlider.value;
        canvas.cameraInvert = cameraInvertCheckBox.checked;
        canvas.zoomSensitivity = cameraZoomSensitivitySlider.value;
        canvas.zoomInvert = cameraZoomInvertCheckBox.checked;
        canvas.orthographic = orthographicField.checked;
        canvas.fov = fovField.text;
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
            Controls.TextField {
                id: objectUpdateRateField
                Kirigami.FormData.label: "Object update rate"
                validator: DoubleValidator{bottom: 1}
            }
            Controls.Slider {
                Kirigami.FormData.label: "Camera sensitivity"
                id: cameraSensitivitySlider
                from: 0
                to: 0.01
            }
            Controls.CheckBox {
                id: cameraInvertCheckBox
                Kirigami.FormData.label: "Invert camera movement"
            }
            Controls.Label {
                text: `${cameraSensitivitySlider.value.toPrecision(4)} rad/px`
            }
            Controls.Slider {
                Kirigami.FormData.label: "Zoom sensitivity"
                id: cameraZoomSensitivitySlider
                from: 0
                to: 0.005
            }
            Controls.CheckBox {
                id: cameraZoomInvertCheckBox
                Kirigami.FormData.label: "Invert zoom"

            }

            Controls.Switch {
                id: orthographicField
                Kirigami.FormData.label: "Orthographic Projection"
            }
            Controls.TextField {
                id: fovField
                Kirigami.FormData.label: "FOV"
                validator: DoubleValidator{bottom: 1}
                enabled: orthographicField.position < 0.5
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
