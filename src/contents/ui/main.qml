import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.20 as Kirigami
import Renderer 1.0

Kirigami.ApplicationWindow {
    
    id: root

    title: "n body sim"
    pageStack.initialPage: Canvas {

    }
}
