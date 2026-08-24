import QtQuick

Item {
    id: root

    property string entityName: ""
    property string statusText: ""
    property real rollDegrees: 0
    property real pitchDegrees: 0
    property real headingDegrees: 0
    property real airspeedKnots: 0
    property real altitudeFeet: 0
    property real verticalSpeedFpm: 0
    property real selectedHeadingDegrees: 0
    property real selectedAirspeedKnots: 0
    property real selectedAltitudeFeet: 0
    property bool flightDirectorActive: false
    property bool controlActive: false

    signal takeControlRequested(real headingDegrees,
                                real altitudeFeet,
                                real speedKnots)
    signal setpointsRequested(real headingDegrees,
                              real altitudeFeet,
                              real speedKnots)
    signal releaseControlRequested()

    readonly property color ink: "#f2f5f7"
    readonly property color cyan: "#43d9ff"
    readonly property color magenta: "#ff4fd8"
    readonly property color amber: "#ffc857"

    implicitWidth: 680
    implicitHeight: 470

    Rectangle {
        anchors.fill: parent
        color: "#090d10"
    }

    Canvas {
        id: display
        anchors.fill: parent
        anchors.margins: 8
        anchors.bottomMargin: 58

        function normalizedHeading(value) {
            var result = value % 360
            return result < 0 ? result + 360 : result
        }

        function headingDelta(value, reference) {
            return ((value - reference + 540) % 360) - 180
        }

        function line(ctx, x1, y1, x2, y2, color, width) {
            ctx.beginPath()
            ctx.moveTo(x1, y1)
            ctx.lineTo(x2, y2)
            ctx.strokeStyle = color
            ctx.lineWidth = width
            ctx.stroke()
        }

        function label(ctx, value, x, y, size, color, align) {
            ctx.fillStyle = color
            ctx.font = "600 " + size + "px monospace"
            ctx.textAlign = align || "center"
            ctx.textBaseline = "middle"
            ctx.fillText(value, x, y)
        }

        onPaint: {
            var ctx = getContext("2d")
            var w = width
            var h = height
            var top = 34
            var bottom = h - 58
            var leftTape = Math.max(70, w * 0.13)
            var rightTape = w - leftTape
            var cx = w / 2
            var cy = (top + bottom) / 2
            var attitudeLeft = leftTape + 12
            var attitudeRight = rightTape - 12
            var attitudeWidth = attitudeRight - attitudeLeft
            var attitudeHeight = bottom - top
            var pitchScale = Math.max(3.2, attitudeHeight / 72)

            ctx.reset()
            ctx.clearRect(0, 0, w, h)

            ctx.save()
            ctx.beginPath()
            ctx.rect(attitudeLeft, top, attitudeWidth, attitudeHeight)
            ctx.clip()
            ctx.translate(cx, cy)
            ctx.rotate(-root.rollDegrees * Math.PI / 180)
            ctx.translate(0, root.pitchDegrees * pitchScale)

            ctx.fillStyle = "#176b96"
            ctx.fillRect(-w, -h * 2, w * 2, h * 2)
            ctx.fillStyle = "#6a5135"
            ctx.fillRect(-w, 0, w * 2, h * 2)
            line(ctx, -w, 0, w, 0, root.ink, 2.5)

            for (var pitch = -30; pitch <= 30; pitch += 5) {
                if (pitch === 0) continue
                var py = -pitch * pitchScale
                var major = pitch % 10 === 0
                var half = major ? 42 : 24
                line(ctx, -half, py, half, py, root.ink, major ? 2 : 1)
                if (major) {
                    label(ctx, Math.abs(pitch).toString(), -half - 17, py, 11, root.ink)
                    label(ctx, Math.abs(pitch).toString(), half + 17, py, 11, root.ink)
                }
            }
            ctx.restore()

            ctx.strokeStyle = "#26333a"
            ctx.lineWidth = 2
            ctx.strokeRect(attitudeLeft, top, attitudeWidth, attitudeHeight)

            var rollRadius = Math.min(attitudeWidth * 0.34, attitudeHeight * 0.42)
            for (var r = -60; r <= 60; r += 10) {
                var angle = (r - 90) * Math.PI / 180
                var inner = rollRadius - (r % 30 === 0 ? 10 : 6)
                line(ctx,
                     cx + Math.cos(angle) * inner,
                     cy + Math.sin(angle) * inner,
                     cx + Math.cos(angle) * rollRadius,
                     cy + Math.sin(angle) * rollRadius,
                     root.ink,
                     r === 0 ? 2.5 : 1.5)
            }
            ctx.fillStyle = root.ink
            ctx.beginPath()
            ctx.moveTo(cx, cy - rollRadius - 2)
            ctx.lineTo(cx - 7, cy - rollRadius + 10)
            ctx.lineTo(cx + 7, cy - rollRadius + 10)
            ctx.closePath()
            ctx.fill()

            line(ctx, cx - 63, cy, cx - 18, cy, root.amber, 4)
            line(ctx, cx + 18, cy, cx + 63, cy, root.amber, 4)
            line(ctx, cx - 18, cy, cx - 7, cy + 8, root.amber, 4)
            line(ctx, cx + 18, cy, cx + 7, cy + 8, root.amber, 4)
            ctx.strokeStyle = root.amber
            ctx.lineWidth = 3
            ctx.beginPath()
            ctx.arc(cx, cy, 6, 0, Math.PI * 2)
            ctx.stroke()

            if (root.flightDirectorActive) {
                var headingError = headingDelta(root.selectedHeadingDegrees,
                                                root.headingDegrees)
                var altitudeError = root.selectedAltitudeFeet - root.altitudeFeet
                var fdX = Math.max(-46, Math.min(46, headingError * 1.2))
                var fdY = Math.max(-38, Math.min(38, -altitudeError / 100))
                line(ctx, cx + fdX, cy - 35, cx + fdX, cy + 35,
                     root.magenta, 4)
                line(ctx, cx - 42, cy + fdY, cx + 42, cy + fdY,
                     root.magenta, 4)
            }

            ctx.fillStyle = "#11191d"
            ctx.fillRect(0, top, leftTape, attitudeHeight)
            ctx.fillRect(rightTape, top, w - rightTape, attitudeHeight)
            line(ctx, leftTape, top, leftTape, bottom, "#56656d", 1)
            line(ctx, rightTape, top, rightTape, bottom, "#56656d", 1)

            var speedSpacing = 3.2
            for (var ds = -50; ds <= 50; ds += 10) {
                var speedValue = Math.max(0, Math.round(root.airspeedKnots / 10) * 10 + ds)
                var sy = cy - (speedValue - root.airspeedKnots) * speedSpacing
                if (sy > top + 8 && sy < bottom - 8) {
                    line(ctx, leftTape - 9, sy, leftTape, sy, root.ink, 1)
                    label(ctx, speedValue.toString(), leftTape - 14, sy, 12,
                          root.ink, "right")
                }
            }
            ctx.fillStyle = "#050708"
            ctx.fillRect(4, cy - 19, leftTape - 2, 38)
            ctx.strokeStyle = root.cyan
            ctx.lineWidth = 2
            ctx.strokeRect(4, cy - 19, leftTape - 2, 38)
            label(ctx, Math.round(root.airspeedKnots).toString(), leftTape / 2,
                  cy, 18, root.cyan)
            label(ctx, "SPD " + Math.round(root.selectedAirspeedKnots), 7,
                  top - 18, 11, root.magenta, "left")

            // Keep 200 ft labels at least 18 px apart so the tape remains
            // readable when the cockpit panel is vertically compressed.
            var altitudeSpacing = Math.max(0.09, attitudeHeight / 3600)
            var roundedAltitude = Math.round(root.altitudeFeet / 100) * 100
            for (var da = -1000; da <= 1000; da += 200) {
                var altitudeValue = roundedAltitude + da
                var ay = cy - (altitudeValue - root.altitudeFeet) * altitudeSpacing
                if (altitudeValue >= 0 && ay > top + 8 && ay < bottom - 8) {
                    line(ctx, rightTape, ay, rightTape + 9, ay, root.ink, 1)
                    label(ctx, altitudeValue.toString(), rightTape + 14, ay,
                          12, root.ink, "left")
                }
            }
            ctx.fillStyle = "#050708"
            ctx.fillRect(rightTape + 2, cy - 19, w - rightTape - 6, 38)
            ctx.strokeStyle = root.cyan
            ctx.lineWidth = 2
            ctx.strokeRect(rightTape + 2, cy - 19, w - rightTape - 6, 38)
            label(ctx, Math.round(root.altitudeFeet).toString(),
                  (rightTape + w) / 2, cy, 16, root.cyan)
            label(ctx, "ALT " + Math.round(root.selectedAltitudeFeet), w - 7,
                  top - 18, 11, root.magenta, "right")

            var vsiX = w - 8
            line(ctx, vsiX, cy - 72, vsiX, cy + 72, "#56656d", 2)
            var vsiOffset = Math.max(-68, Math.min(68,
                                                  -root.verticalSpeedFpm / 30))
            line(ctx, vsiX - 12, cy + vsiOffset, vsiX, cy + vsiOffset,
                 root.cyan, 4)

            ctx.fillStyle = "#11191d"
            ctx.fillRect(attitudeLeft, bottom + 5, attitudeWidth, h - bottom - 5)
            for (var hd = -30; hd <= 30; hd += 5) {
                var headingValue = normalizedHeading(root.headingDegrees + hd)
                var hx = cx + hd * (attitudeWidth / 70)
                var tickHeight = hd % 10 === 0 ? 10 : 6
                line(ctx, hx, bottom + 7, hx, bottom + 7 + tickHeight,
                     root.ink, 1)
                if (hd % 10 === 0) {
                    label(ctx, Math.round(headingValue / 10).toString().padStart(2, "0"),
                          hx, bottom + 27, 12, root.ink)
                }
            }
            var selectedDelta = headingDelta(root.selectedHeadingDegrees,
                                             root.headingDegrees)
            if (Math.abs(selectedDelta) <= 35) {
                var selectedX = cx + selectedDelta * (attitudeWidth / 70)
                ctx.fillStyle = root.magenta
                ctx.beginPath()
                ctx.moveTo(selectedX, bottom + 5)
                ctx.lineTo(selectedX - 6, bottom - 4)
                ctx.lineTo(selectedX + 6, bottom - 4)
                ctx.closePath()
                ctx.fill()
            }
            ctx.strokeStyle = root.cyan
            ctx.lineWidth = 2
            ctx.strokeRect(cx - 29, bottom + 3, 58, 34)
            label(ctx, Math.round(normalizedHeading(root.headingDegrees))
                          .toString().padStart(3, "0"),
                  cx, bottom + 20, 18, root.cyan)

            label(ctx, root.entityName, 7, 14, 12, root.ink, "left")
            label(ctx, root.statusText, w - 7, 14, 10, "#9bacb5", "right")
        }

        Component.onCompleted: requestPaint()
        onWidthChanged: requestPaint()
        onHeightChanged: requestPaint()
    }

    Rectangle {
        id: controlStrip
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 6
        height: 46
        color: "#11191d"
        border.color: root.controlActive ? root.cyan : "#56656d"
        border.width: 1

        component StepControl: Rectangle {
            property string caption: ""
            property string valueText: ""
            signal decrease()
            signal increase()

            width: 96
            height: 34
            color: "#090d10"
            border.color: "#3a474d"

            Rectangle {
                width: 24
                height: parent.height
                color: minusArea.pressed ? "#26333a" : "transparent"
                Text {
                    anchors.centerIn: parent
                    text: "-"
                    color: root.ink
                    font.pixelSize: 18
                }
                MouseArea {
                    id: minusArea
                    anchors.fill: parent
                    onClicked: parent.parent.decrease()
                }
            }

            Column {
                anchors.centerIn: parent
                spacing: 0
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: parent.parent.caption
                    color: "#9bacb5"
                    font.pixelSize: 8
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: parent.parent.valueText
                    color: root.magenta
                    font.family: "monospace"
                    font.pixelSize: 11
                    font.bold: true
                }
            }

            Rectangle {
                anchors.right: parent.right
                width: 24
                height: parent.height
                color: plusArea.pressed ? "#26333a" : "transparent"
                Text {
                    anchors.centerIn: parent
                    text: "+"
                    color: root.ink
                    font.pixelSize: 16
                }
                MouseArea {
                    id: plusArea
                    anchors.fill: parent
                    onClicked: parent.parent.increase()
                }
            }
        }

        Row {
            anchors.centerIn: parent
            spacing: 5

            StepControl {
                caption: "HDG"
                valueText: Math.round(root.selectedHeadingDegrees)
                           .toString().padStart(3, "0")
                onDecrease: {
                    root.selectedHeadingDegrees =
                            (root.selectedHeadingDegrees + 355) % 360
                    if (root.controlActive)
                        root.setpointsRequested(root.selectedHeadingDegrees,
                                                root.selectedAltitudeFeet,
                                                root.selectedAirspeedKnots)
                }
                onIncrease: {
                    root.selectedHeadingDegrees =
                            (root.selectedHeadingDegrees + 5) % 360
                    if (root.controlActive)
                        root.setpointsRequested(root.selectedHeadingDegrees,
                                                root.selectedAltitudeFeet,
                                                root.selectedAirspeedKnots)
                }
            }

            StepControl {
                caption: "ALT FT"
                valueText: Math.round(root.selectedAltitudeFeet).toString()
                onDecrease: {
                    root.selectedAltitudeFeet =
                            Math.max(0, root.selectedAltitudeFeet - 500)
                    if (root.controlActive)
                        root.setpointsRequested(root.selectedHeadingDegrees,
                                                root.selectedAltitudeFeet,
                                                root.selectedAirspeedKnots)
                }
                onIncrease: {
                    root.selectedAltitudeFeet += 500
                    if (root.controlActive)
                        root.setpointsRequested(root.selectedHeadingDegrees,
                                                root.selectedAltitudeFeet,
                                                root.selectedAirspeedKnots)
                }
            }

            StepControl {
                caption: "SPD KT"
                valueText: Math.round(root.selectedAirspeedKnots).toString()
                onDecrease: {
                    root.selectedAirspeedKnots =
                            Math.max(0, root.selectedAirspeedKnots - 10)
                    if (root.controlActive)
                        root.setpointsRequested(root.selectedHeadingDegrees,
                                                root.selectedAltitudeFeet,
                                                root.selectedAirspeedKnots)
                }
                onIncrease: {
                    root.selectedAirspeedKnots += 10
                    if (root.controlActive)
                        root.setpointsRequested(root.selectedHeadingDegrees,
                                                root.selectedAltitudeFeet,
                                                root.selectedAirspeedKnots)
                }
            }

            Rectangle {
                width: 80
                height: 34
                color: controlArea.pressed
                       ? "#26333a"
                       : (root.controlActive ? "#14343d" : "#172126")
                border.color: root.controlActive ? root.cyan : "#72838b"
                Text {
                    anchors.centerIn: parent
                    text: root.controlActive ? "RELEASE" : "TAKE"
                    color: root.controlActive ? root.cyan : root.ink
                    font.pixelSize: 11
                    font.bold: true
                }
                MouseArea {
                    id: controlArea
                    anchors.fill: parent
                    onClicked: {
                        if (root.controlActive) {
                            root.releaseControlRequested()
                        } else {
                            root.takeControlRequested(
                                        root.selectedHeadingDegrees,
                                        root.selectedAltitudeFeet,
                                        root.selectedAirspeedKnots)
                        }
                    }
                }
            }
        }
    }

    function refresh() {
        display.requestPaint()
    }

    onEntityNameChanged: refresh()
    onStatusTextChanged: refresh()
    onRollDegreesChanged: refresh()
    onPitchDegreesChanged: refresh()
    onHeadingDegreesChanged: refresh()
    onAirspeedKnotsChanged: refresh()
    onAltitudeFeetChanged: refresh()
    onVerticalSpeedFpmChanged: refresh()
    onSelectedHeadingDegreesChanged: refresh()
    onSelectedAirspeedKnotsChanged: refresh()
    onSelectedAltitudeFeetChanged: refresh()
    onFlightDirectorActiveChanged: refresh()
}
