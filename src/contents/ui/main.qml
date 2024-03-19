import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.20 as Kirigami
import Renderer 1.0

Kirigami.ApplicationWindow {
    id: root

    globalDrawer: Kirigami.GlobalDrawer {
        isMenu: true
        actions: [
            Kirigami.Action {
                text: "settings"
                onTriggered: settingsPage.open()
            }
        ]
    }

    title: "n body sim"
    pageStack.initialPage: Kirigami.Page {
        padding: 0
        contextualActions: [
            Kirigami.Action {
                icon.name: canvas.isSimulationRunning ? "media-playback-pause" : "media-playback-start"
                tooltip: canvas.isSimulationRunning ? "Pause Simulation" : "Play Simulation"
                onTriggered: {
                    canvas.isSimulationRunning = !canvas.isSimulationRunning;
                    if (!canvas.isSimulationRunning) {
                        canvas.synchronizeObjects();
                    }
                }
            },
            Kirigami.Action {
                // TODO: gray out action when simulation is running
                icon.name: "media-playback-start"
                tooltip: "Tick Simulation"
                onTriggered: {
                    if (!canvas.isSimulationRunning) {
                        canvas.tickSimulator();
                        canvas.updateRenderer();
                        canvas.synchronizeObjects();
                    }
                }
            }
        ]

        MouseArea {
            anchors.fill: parent
            property real oldX;
            property real oldY;
            onPressed: {
                oldX = mouse.x;
                oldY = mouse.y;
            }
            onPositionChanged: {
                if (!containsPress)
                    return;
                canvas.moveCamera(oldX-mouse.x, oldY-mouse.y)
                oldX = mouse.x;
                oldY = mouse.y;
            }
        }

        Canvas {
            id: canvas
            anchors.fill: parent
        }

        Kirigami.ActionToolBar {
            anchors.right: parent.right
            anchors.top: parent.top
            actions: [
                Kirigami.Action {
                    visible: true
                    icon.name: pageStack.lastItem === statusPage ? "arrow-right-double" : "arrow-left-double"
                    onTriggered: pageStack.lastItem === statusPage ? pageStack.pop() : pageStack.push(statusPage)
                }
            ]
        }
    }
    SettingsPage {
        id: settingsPage
    }
    StatusPage {
        id: statusPage
    }
}
