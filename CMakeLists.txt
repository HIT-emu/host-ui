cmake_minimum_required(VERSION 3.16)

project(untitled4 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MACOSX_RPATH 1)

# 1. Сначала ищем зависимости
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick Charts SerialPort Widgets)

qt_standard_project_setup()

# 2. Создаем цель (executable)
qt_add_executable(appuntitled4
    main.cpp
    batteryengine.cpp
    batteryengine.h
    emdevicecommunicator.cpp
    emdevicecommunicator.h
    emulationcontroller.cpp
    emulationcontroller.h
    TelemetryFrame.h
)

# 3. Добавляем QML модуль
qt_add_qml_module(appuntitled4
    URI untitled4
    VERSION 1.0
    QML_FILES Main.qml
)

# 4. ПРИВЯЗЫВАЕМ БИБЛИОТЕКИ (Теперь это сработает, так как appuntitled4 уже объявлен выше)
target_link_libraries(appuntitled4
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::Charts
    Qt6::SerialPort
    Qt6::Widgets
)
