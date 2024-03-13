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
        ]
    }

    title: "n body sim"
    pageStack.initialPage: Kirigami.Page {
        padding: 0
        contextualActions: [
            Kirigami.Action {
                icon.name: "media-playback-start"
                tooltip: "Pause/Play Simulation"
                onTriggered: ;//TODO
            },
            Kirigami.Action {
                icon.name: "media-playback-start"
                tooltip: "Tick Simulation"
                onTriggered: ;//TODO
            }
        ]

        Canvas {
            anchors.fill: parent
        }
    }
    Kirigami.ScrollablePage {
        id: statusPage
    }
}
