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

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        // 1. Верхняя панель (ФИКСИРОВАННАЯ ВЫСОТА)
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
                    color: "#333"
                    radius: 4
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
                        controller.start(portName.text, 3.7, 2.5)
                    } else {
                        controller.stop()
                    }
                }
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

        // 2. ГРАФИК (МАКСИМАЛЬНОЕ ЗАПОЛНЕНИЕ)
        // Убираем Rectangle-обертку, используем ChartView напрямую с заполнением
        ChartView {
            id: mainChart
            Layout.fillWidth: true
            Layout.fillHeight: true // Занимает всё пространство между верхом и консолью
            Layout.minimumHeight: 300 // Чтобы не схлопнулся

            theme: ChartView.ChartThemeDark
            antialiasing: true
            backgroundColor: "#252525" // Цвет фона графика
            legend.alignment: Qt.AlignBottom

            ValueAxis {
                id: axisX
                min: 0; max: 300
                titleText: "Время (сек)"
                labelFormat: "%.0f"
                tickCount: 11
            }

            ValueAxis {
                id: axisY_V
                min: 3.0; max: 4.2
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
                id: vSeries; name: "Voltage"; axisX: axisX; axisY: axisY_V
                color: "#00ccff"; width: 2
            }

            LineSeries {
                id: ahSeries; name: "Capacity"; axisX: axisX; axisYRight: axisY_Ah
                color: "#ffaa00"; width: 2
            }
        }

        // 3. ТЕРМИНАЛ (ФИКСИРОВАННАЯ ВЫСОТА СНИЗУ)
        ColumnLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 200 // Увеличили немного для удобства
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
                        // Если RichText вызывает проблемы с отображением, можно заменить на PlainText
                        textFormat: TextEdit.AutoText
                        background: null
                    }
                }
            }

            // Строка ввода
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
                            controller.sendRawCommand(text);
                            logArea.append("[UI]: " + text);
                            text = "";
                        }
                    }
                }
            }
        }
    }

    Connections {
        target: controller
        function onPointsUpdated(t, v) {
            vSeries.append(t, v)
            ahSeries.append(t, controller.consumedAh)
            if (t > 300) {
                axisX.min = t - 300
                axisX.max = t
            }
            if (controller.consumedAh > axisY_Ah.max) {
                axisY_Ah.max = controller.consumedAh * 1.2
            }
        }
        function onLogReceived(msg) {
            logArea.append(msg)
            logArea.cursorPosition = logArea.length
        }
    }
}
