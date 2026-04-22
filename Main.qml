import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtCharts

ApplicationWindow {
    id: window
    visible: true
    width: 1024
    height: 768
    title: "HiTeMu Battery Emulator — Контроль телеметрии"

    background: Rectangle { color: "#1e1e1e" }

    // ── State ──────────────────────────────────────────────────────────────
    property real windowSize: 300   // visible time range, seconds
    property real totalTime:  0     // max time seen so far
    property bool liveMode:   true  // auto-follow newest data

    // ── Model parameters ───────────────────────────────────────────────────
    property double paramE0:      3.781272
    property double paramK1:      0.093608
    property double paramK2:      0.058901
    property double paramA:       0.563978
    property double paramB:       7.504662
    property double paramR:       0.109
    property double paramCap:     0.5087
    property double paramVoutMin: 1.33
    property double paramVoutMax: 3.23

    function updateAxes() {
        if (liveMode) {
            var start = Math.max(0, totalTime - windowSize)
            axisX.min = start
            axisX.max = start + windowSize
            if (totalTime > windowSize)
                timeSlider.value = start
        } else {
            axisX.min = timeSlider.value
            axisX.max = timeSlider.value + windowSize
        }
    }

    // ── Model parameters popup ─────────────────────────────────────────────
    Popup {
        id: modelParamsPopup
        anchors.centerIn: parent
        width: 340
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape

        background: Rectangle {
            color: "#2a2a2a"
            border.color: "#555"
            radius: 6
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 10

            Label {
                text: "Параметры батарейной модели"
                color: "white"
                font.pixelSize: 14
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
            }

            Rectangle { height: 1; Layout.fillWidth: true; color: "#555" }

            component ParamRow: RowLayout {
                property string label: ""
                property string paramName: ""
                property alias fieldText: field.text
                Layout.fillWidth: true
                spacing: 8

                Label {
                    text: parent.label
                    color: "#aaa"
                    font.family: "Courier"
                    font.pixelSize: 13
                    Layout.preferredWidth: 80
                }
                TextField {
                    id: field
                    Layout.fillWidth: true
                    Layout.preferredHeight: 28
                    color: "white"
                    font.family: "Courier"
                    font.pixelSize: 13
                    horizontalAlignment: TextInput.AlignRight
                    background: Rectangle { color: "#333"; radius: 3; border.color: "#555" }
                    validator: DoubleValidator { notation: DoubleValidator.ScientificNotation; locale: "C" }
                }
            }

            ParamRow { id: rowE0;  label: "E₀ (V)";   fieldText: paramE0.toString() }
            ParamRow { id: rowK1;  label: "k₁";        fieldText: paramK1.toString() }
            ParamRow { id: rowK2;  label: "k₂";        fieldText: paramK2.toString() }
            ParamRow { id: rowA;   label: "A";          fieldText: paramA.toString()  }
            ParamRow { id: rowB;   label: "B";          fieldText: paramB.toString()  }
            ParamRow { id: rowR;   label: "R (Ω)";     fieldText: paramR.toString()  }
            ParamRow { id: rowCap; label: "Q (Ah)";    fieldText: paramCap.toString() }

            Rectangle { height: 1; Layout.fillWidth: true; color: "#444" }

            Label {
                text: "Калибровка Vout (для данной платы)"
                color: "#888"
                font.pixelSize: 11
                Layout.alignment: Qt.AlignHCenter
            }

            ParamRow { id: rowVoutMin; label: "Vout мин (V)"; fieldText: paramVoutMin.toString() }
            ParamRow { id: rowVoutMax; label: "Vout макс (V)"; fieldText: paramVoutMax.toString() }

            Rectangle { height: 1; Layout.fillWidth: true; color: "#555" }

            RowLayout {
                Layout.fillWidth: true
                spacing: 8

                Item { Layout.fillWidth: true }

                Button {
                    text: "Отмена"
                    implicitHeight: 30
                    onClicked: modelParamsPopup.close()
                }

                Button {
                    text: "Принять"
                    implicitHeight: 30
                    highlighted: true
                    onClicked: {
                        paramE0      = parseFloat(rowE0.fieldText)      || paramE0
                        paramK1      = parseFloat(rowK1.fieldText)      || paramK1
                        paramK2      = parseFloat(rowK2.fieldText)      || paramK2
                        paramA       = parseFloat(rowA.fieldText)       || paramA
                        paramB       = parseFloat(rowB.fieldText)       || paramB
                        paramR       = parseFloat(rowR.fieldText)       || paramR
                        paramCap     = parseFloat(rowCap.fieldText)     || paramCap
                        paramVoutMin = parseFloat(rowVoutMin.fieldText) || paramVoutMin
                        paramVoutMax = parseFloat(rowVoutMax.fieldText) || paramVoutMax
                        modelParamsPopup.close()
                    }
                }
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        // 1. Toolbar
        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            spacing: 15

            TextField {
                id: portName
                text: "/dev/ttyACM0"
                placeholderText: "Порт"
                Layout.preferredWidth: 200
                Layout.preferredHeight: 35
                color: "white"
                font.family: "Courier"
                background: Rectangle {
                    color: "#333"; radius: 4
                    border.color: portName.activeFocus ? "#00ff00" : "#555"
                }
            }

            Button {
                text: controller.active ? "СТОП" : "СТАРТ"
                Layout.preferredHeight: 35
                onClicked: {
                    if (!controller.active) {
                        vSeries.clear()
                        ahSeries.clear()
                        totalTime  = 0
                        liveMode   = true
                        axisX.min  = 0
                        axisX.max  = windowSize
                        axisY_Ah.max = 0.005
                        timeSlider.value = 0
                        controller.start(portName.text,
                                         paramE0, paramK1, paramK2,
                                         paramA,  paramB,  paramR,
                                         paramCap,
                                         paramVoutMin, paramVoutMax)
                    } else {
                        controller.stop()
                    }
                }
            }

            Button {
                text: controller.emulating ? "■ Эмуляция" : "▶ Эмуляция"
                Layout.preferredHeight: 35
                highlighted: controller.emulating
                enabled: controller.active
                onClicked: controller.setEmulating(!controller.emulating)
            }

            Button {
                text: "⚙ Модель"
                Layout.preferredHeight: 35
                onClicked: modelParamsPopup.open()
            }

            Label {
                text: "U: " + controller.modelVoltage.toFixed(3) + " V"
                color: "#00ccff"; font.pixelSize: 18; font.bold: true; font.family: "Courier"
            }
            Label {
                text: "Q: " + controller.consumedAh.toFixed(4) + " Ah"
                color: "#ffaa00"; font.pixelSize: 18; font.bold: true; font.family: "Courier"
            }
            Item { Layout.fillWidth: true }
        }

        // 2. Chart
        ChartView {
            id: mainChart
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 300
            theme: ChartView.ChartThemeDark
            antialiasing: true
            backgroundColor: "#252525"
            legend.alignment: Qt.AlignBottom

            ValueAxis {
                id: axisX
                min: 0; max: windowSize
                titleText: "Время (сек)"
                labelFormat: "%.0f"
                tickCount: 11
            }
            ValueAxis {
                id: axisY_V
                min: 1.0; max: 4.2
                titleText: "Напряжение (V)"
                color: "#00ccff"
            }
            ValueAxis {
                id: axisY_Ah
                min: 0; max: 0.005
                titleText: "Ёмкость (Ah)"
                color: "#ffaa00"
            }

            LineSeries {
                id: vSeries; name: "Voltage"
                axisX: axisX; axisY: axisY_V
                color: "#00ccff"; width: 2
            }
            LineSeries {
                id: ahSeries; name: "Capacity"
                axisX: axisX; axisYRight: axisY_Ah
                color: "#ffaa00"; width: 2
            }
        }

        // 3. Timeline scrubber (appears when data > window)
        RowLayout {
            Layout.fillWidth: true
            spacing: 6
            visible: totalTime > windowSize

            Label {
                text: "0"
                color: "#888"; font.pixelSize: 11
                Layout.preferredWidth: 30
            }

            Slider {
                id: timeSlider
                Layout.fillWidth: true
                from: 0
                to: Math.max(1, totalTime - windowSize)
                stepSize: 1
                value: 0

                background: Rectangle {
                    x: timeSlider.leftPadding
                    y: timeSlider.topPadding + timeSlider.availableHeight / 2 - height / 2
                    width: timeSlider.availableWidth
                    height: 4
                    radius: 2
                    color: "#333"
                    Rectangle {
                        width: timeSlider.visualPosition * parent.width
                        height: parent.height
                        color: liveMode ? "#00ccff" : "#ffaa00"
                        radius: 2
                    }
                }

                onMoved: {
                    liveMode = false
                    updateAxes()
                }
            }

            Label {
                text: totalTime.toFixed(0) + "s"
                color: "#888"; font.pixelSize: 11
                Layout.preferredWidth: 40
            }

            Label { text: "Окно:"; color: "#888"; font.pixelSize: 11 }
            TextField {
                id: winSizeField
                text: "300"
                implicitWidth: 55
                implicitHeight: 28
                color: "white"; font.pixelSize: 12
                background: Rectangle { color: "#333"; radius: 3; border.color: "#555" }
                validator: IntValidator { bottom: 10; top: 36000 }
                onAccepted: {
                    windowSize = parseInt(text) || 300
                    updateAxes()
                }
            }
            Label { text: "с"; color: "#888"; font.pixelSize: 11 }

            Button {
                text: liveMode ? "● Live" : "○ Live"
                implicitHeight: 28
                highlighted: liveMode
                onClicked: {
                    liveMode = true
                    updateAxes()
                }
            }
        }

        // 4. Terminal
        ColumnLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            spacing: 0

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "black"
                border.color: "#444"

                ScrollView {
                    anchors.fill: parent
                    clip: true
                    TextArea {
                        id: logArea
                        readOnly: true
                        font.family: "Courier"
                        font.pixelSize: 12
                        color: "#00ff00"
                        textFormat: TextEdit.AutoText
                        background: null
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 35
                spacing: 0
                Rectangle {
                    width: 30; height: 35; color: "#222"
                    Text { text: ">"; color: "#00ff00"; anchors.centerIn: parent; font.bold: true }
                }
                TextField {
                    id: commandInput
                    Layout.fillWidth: true
                    Layout.preferredHeight: 35
                    color: "white"
                    font.family: "Courier"
                    verticalAlignment: TextInput.AlignVCenter
                    background: Rectangle { color: "#222"; border.color: "#444" }
                    onAccepted: {
                        if (text.trim() !== "") {
                            controller.sendRawCommand(text)
                            logArea.append("[UI]: " + text)
                            text = ""
                        }
                    }
                }
            }
        }
    }

    // ── Connections ────────────────────────────────────────────────────────
    Connections {
        target: controller

        function onPointsUpdated(t, v) {
            vSeries.append(t, v)
            ahSeries.append(t, controller.consumedAh)

            totalTime = t

            if (liveMode) {
                updateAxes()
            }

            if (controller.consumedAh > axisY_Ah.max)
                axisY_Ah.max = controller.consumedAh * 1.2
        }

        function onLogReceived(msg) {
            logArea.append(msg)
            logArea.cursorPosition = logArea.length
        }
    }
}
