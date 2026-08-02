import QtQuick

Item {
    id: root

    property string entityName: ""
    property string profileId: ""
    property string dataSource: ""
    property var engineModel: []
    property real fuelCapacityKilograms: 0
    property real fuelRemainingKilograms: 0
    property real fuelPercent: 0
    property real totalFuelFlowKilogramsPerHour: 0
    property real estimatedEnduranceSeconds: 0
    property bool fuelAvailable: false
    property bool enduranceAvailable: false

    function enduranceText() {
        if (!root.enduranceAvailable)
            return "--:--"
        var totalMinutes = Math.floor(root.estimatedEnduranceSeconds / 60)
        var hours = Math.floor(totalMinutes / 60)
        var minutes = totalMinutes % 60
        return (hours < 10 ? "0" : "") + hours + ":" +
               (minutes < 10 ? "0" : "") + minutes
    }

    readonly property color background: "#070b0d"
    readonly property color panel: "#10181b"
    readonly property color lineColor: "#53646a"
    readonly property color green: "#57e389"
    readonly property color cyan: "#48d9ff"
    readonly property color amber: "#ffc857"
    readonly property color red: "#ff5c5c"
    readonly property color ink: "#edf4f5"

    implicitWidth: 680
    implicitHeight: 470

    Rectangle {
        anchors.fill: parent
        color: root.background
    }

    Column {
        anchors.fill: parent
        anchors.margins: 14
        spacing: 10

        Item {
            width: parent.width
            height: 38

            Text {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                text: "ECAM  ENG"
                color: root.green
                font.family: "monospace"
                font.pixelSize: 20
                font.bold: true
            }

            Text {
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                text: root.entityName
                color: root.ink
                font.family: "monospace"
                font.pixelSize: 13
            }

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: 1
                color: root.lineColor
            }
        }

        Grid {
            id: engineGrid
            width: parent.width
            height: parent.height - 166
            columns: root.engineModel.length <= 2 ? Math.max(1, root.engineModel.length) : 2
            spacing: 8

            Repeater {
                model: root.engineModel

                delegate: Rectangle {
                    required property var modelData
                    width: (engineGrid.width - engineGrid.spacing * (engineGrid.columns - 1)) /
                           engineGrid.columns
                    height: root.engineModel.length <= 2
                            ? engineGrid.height
                            : (engineGrid.height - engineGrid.spacing) / 2
                    color: root.panel
                    border.color: root.lineColor
                    border.width: 1
                    radius: 4

                    readonly property color stateColor:
                        modelData.state === "FAILED" ? root.red
                        : modelData.state === "RUNNING" ? root.green
                        : root.amber

                    Column {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 7

                        Row {
                            width: parent.width
                            height: 24

                            Text {
                                width: parent.width * 0.55
                                text: modelData.engineId
                                color: root.ink
                                font.family: "monospace"
                                font.pixelSize: 16
                                font.bold: true
                            }
                            Text {
                                width: parent.width * 0.45
                                text: modelData.state
                                horizontalAlignment: Text.AlignRight
                                color: stateColor
                                font.family: "monospace"
                                font.pixelSize: 13
                                font.bold: true
                            }
                        }

                        Canvas {
                            id: gauge
                            width: Math.min(parent.width, parent.height * 1.15)
                            height: Math.max(72, parent.height * 0.38)
                            anchors.horizontalCenter: parent.horizontalCenter

                            function requestGaugePaint() { requestPaint() }

                            onPaint: {
                                var ctx = getContext("2d")
                                var cx = width / 2
                                var cy = height * 0.88
                                var radius = Math.min(width * 0.42, height * 0.72)
                                var start = Math.PI * 0.82
                                var span = Math.PI * 1.36
                                var value = modelData.n1Available
                                        ? Math.max(0, Math.min(110, modelData.n1Percent))
                                        : 0
                                ctx.reset()
                                ctx.clearRect(0, 0, width, height)
                                ctx.lineWidth = 8
                                ctx.strokeStyle = "#263337"
                                ctx.beginPath()
                                ctx.arc(cx, cy, radius, start, start + span)
                                ctx.stroke()
                                ctx.strokeStyle = value > 100 ? root.red : root.green
                                ctx.beginPath()
                                ctx.arc(cx, cy, radius, start, start + span * value / 110)
                                ctx.stroke()
                                ctx.fillStyle = root.ink
                                ctx.textAlign = "center"
                                ctx.font = "bold 24px monospace"
                                ctx.fillText(modelData.n1Available ? Math.round(value) : "--", cx, cy - 12)
                                ctx.fillStyle = root.cyan
                                ctx.font = "12px monospace"
                                ctx.fillText("N1 %", cx, cy + 10)
                            }

                            Component.onCompleted: requestPaint()
                        }

                        Grid {
                            width: parent.width
                            columns: 2
                            columnSpacing: 10
                            rowSpacing: 5

                            Text { text: "N2"; color: root.cyan; font.family: "monospace" }
                            Text { text: modelData.n2Available ? Math.round(modelData.n2Percent) + " %" : "--"; color: root.ink; font.family: "monospace" }
                            Text { text: "EGT"; color: root.cyan; font.family: "monospace" }
                            Text { text: modelData.exhaustTemperatureAvailable ? Math.round(modelData.exhaustTemperatureCelsius) + " C" : "--"; color: root.ink; font.family: "monospace" }
                            Text { text: "FF"; color: root.cyan; font.family: "monospace" }
                            Text { text: modelData.fuelFlowAvailable ? Math.round(modelData.fuelFlowKilogramsPerHour) + " KG/H" : "--"; color: root.ink; font.family: "monospace" }
                            Text { text: "THR"; color: root.cyan; font.family: "monospace" }
                            Text { text: modelData.thrustAvailable ? modelData.thrustKilonewtons.toFixed(1) + " KN" : "--"; color: root.ink; font.family: "monospace" }
                        }

                        Text {
                            width: parent.width
                            visible: modelData.estimated
                            text: "ESTIMATED"
                            color: root.amber
                            horizontalAlignment: Text.AlignHCenter
                            font.family: "monospace"
                            font.pixelSize: 10
                        }
                    }
                }
            }

            Text {
                width: engineGrid.width
                height: engineGrid.height
                visible: root.engineModel.length === 0
                text: "NO ENGINE TELEMETRY"
                color: root.amber
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.family: "monospace"
                font.pixelSize: 18
            }
        }

        Rectangle {
            id: fuelPanel
            width: parent.width
            height: 58
            color: root.panel
            border.color: root.lineColor
            border.width: 1
            radius: 4

            readonly property color fuelColor:
                !root.fuelAvailable ? root.amber
                : root.fuelPercent <= 10 ? root.red
                : root.fuelPercent <= 20 ? root.amber
                : root.green

            Row {
                anchors.fill: parent
                anchors.margins: 9
                spacing: 24

                Column {
                    width: (parent.width - parent.spacing * 3) * 0.28
                    Text { text: "FUEL"; color: root.cyan; font.family: "monospace"; font.pixelSize: 10 }
                    Text {
                        text: root.fuelAvailable ? Math.round(root.fuelRemainingKilograms) + " KG" : "--"
                        color: fuelPanel.fuelColor
                        font.family: "monospace"
                        font.pixelSize: 17
                        font.bold: true
                    }
                }
                Column {
                    width: (parent.width - parent.spacing * 3) * 0.18
                    Text { text: "LEVEL"; color: root.cyan; font.family: "monospace"; font.pixelSize: 10 }
                    Text {
                        text: root.fuelAvailable ? Math.round(root.fuelPercent) + " %" : "--"
                        color: fuelPanel.fuelColor
                        font.family: "monospace"
                        font.pixelSize: 17
                        font.bold: true
                    }
                }
                Column {
                    width: (parent.width - parent.spacing * 3) * 0.28
                    Text { text: "TOTAL FLOW"; color: root.cyan; font.family: "monospace"; font.pixelSize: 10 }
                    Text {
                        text: root.fuelAvailable ? Math.round(root.totalFuelFlowKilogramsPerHour) + " KG/H" : "--"
                        color: root.ink
                        font.family: "monospace"
                        font.pixelSize: 15
                    }
                }
                Column {
                    width: (parent.width - parent.spacing * 3) * 0.26
                    Text { text: "ENDURANCE"; color: root.cyan; font.family: "monospace"; font.pixelSize: 10 }
                    Text {
                        text: root.enduranceText()
                        color: root.ink
                        font.family: "monospace"
                        font.pixelSize: 17
                    }
                }
            }
        }

        Row {
            width: parent.width
            spacing: 16
            Text {
                text: root.profileId
                color: root.cyan
                font.family: "monospace"
                font.pixelSize: 10
            }
            Text {
                text: root.dataSource
                color: root.dataSource.indexOf("estimate") >= 0 ? root.amber : root.green
                font.family: "monospace"
                font.pixelSize: 10
            }
        }
    }
}
