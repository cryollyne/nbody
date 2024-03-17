import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.20 as Kirigami

Kirigami.ScrollablePage {
    id: statusPage

    Component {
        id: objectListDelegate
        Kirigami.AbstractCard {
            contentItem: Item {
                implicitWidth: delegateLayout.implicitWidth
                implicitHeight: delegateLayout.implicitHeight
                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                }
                ColumnLayout {
                    anchors {
                        left: parent.left
                        right: parent.right
                        top: parent.top
                    }
                    id: delegateLayout
                    Kirigami.Heading {
                        text: `Object ${index}`
                    }
                    Kirigami.Separator {
                        Layout.fillWidth: true
                    }

                    Kirigami.Heading {
                        level: 2
                        text: "Position"
                    }
                    GridLayout {
                        visible: canvas.isSimulationRunning
                        implicitHeight: posGridInner.implicitHeight
                        Item {
                            id: posGridInner
                            implicitHeight: posGridInnerInner.implicitHeight
                            Layout.fillWidth: true;
                            Controls.Label {
                                id: posGridInnerInner
                                text: canvas.objects[index].position.x.toPrecision(4);
                            }
                        }
                        Item {
                            Layout.fillWidth: true;
                            Layout.fillHeight: true;
                            Controls.Label {
                                text: canvas.objects[index].position.y.toPrecision(4);
                            }
                        }
                        Item {
                            Layout.fillWidth: true;
                            Layout.fillHeight: true;
                            Controls.Label {
                                text: canvas.objects[index].position.z.toPrecision(4);
                            }
                        }
                    }
                    RowLayout {
                        visible: !canvas.isSimulationRunning
                        Controls.TextField {
                            Layout.fillWidth: true
                            text: canvas.objects[index].position.x.toPrecision(8);
                        }
                        Controls.TextField {
                            Layout.fillWidth: true
                            text: canvas.objects[index].position.y.toPrecision(8);
                        }
                        Controls.TextField {
                            Layout.fillWidth: true
                            text: canvas.objects[index].position.z.toPrecision(8);
                        }
                    }
                    Kirigami.Separator {
                        Layout.fillWidth: true
                    }

                    Kirigami.Heading {
                        level: 2
                        text: "Velocity"
                    }
                    GridLayout {
                        visible: canvas.isSimulationRunning
                        implicitHeight: velGridInner.implicitHeight
                        Item {
                            id: velGridInner
                            implicitHeight: velGridInnerInner.implicitHeight
                            Layout.fillWidth: true;
                            Controls.Label {
                                id: velGridInnerInner
                                text: canvas.objects[index].velocity.x.toPrecision(4);
                            }
                        }
                        Item {
                            Layout.fillWidth: true;
                            Layout.fillHeight: true;
                            Controls.Label {
                                text: canvas.objects[index].velocity.y.toPrecision(4);
                            }
                        }
                        Item {
                            Layout.fillWidth: true;
                            Layout.fillHeight: true;
                            Controls.Label {
                                text: canvas.objects[index].velocity.z.toPrecision(4);
                            }
                        }
                    }
                    RowLayout {
                        visible: !canvas.isSimulationRunning
                        Controls.TextField {
                            Layout.fillWidth: true
                            text: canvas.objects[index].velocity.x.toPrecision(8);
                        }
                        Controls.TextField {
                            Layout.fillWidth: true
                            text: canvas.objects[index].velocity.y.toPrecision(8);
                        }
                        Controls.TextField {
                            Layout.fillWidth: true
                            text: canvas.objects[index].velocity.z.toPrecision(8);
                        }
                    }
                    Kirigami.Separator {
                        Layout.fillWidth: true
                    }


                    Kirigami.Heading {
                        text: "Mass"
                    }
                    Controls.Label {
                        visible: canvas.isSimulationRunning
                        text: canvas.objects[index].mass.toPrecision(4);
                    }
                    Controls.TextField {
                        visible: !canvas.isSimulationRunning
                        Layout.fillWidth: true
                        text: canvas.objects[index].mass.toPrecision(8);
                    }
                }
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent

        Kirigami.CardsListView {
            Layout.fillWidth: true;
            Layout.fillHeight: true;
            id: listView
            model: canvas.objects
            delegate: objectListDelegate
        }

        Controls.Button {
            Layout.alignment: Qt.AlignRight | Qt.AlignBottom
            text: "Close"
            onClicked: pageStack.pop()
        }
    }
}
