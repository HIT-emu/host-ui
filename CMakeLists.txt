cmake_minimum_required(VERSION 3.16)
project(em_host LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core SerialPort)

add_library(em_core STATIC
    src/TelemetryFrame.h
    src/EmDeviceCommunicator.h
    src/EmDeviceCommunicator.cpp
    src/BatteryEngine.h
    src/BatteryEngine.cpp
    src/EmulationController.h
    src/EmulationController.cpp
)
target_include_directories(em_core PUBLIC src)
target_link_libraries(em_core Qt6::Core Qt6::SerialPort)

add_executable(test_communicator src/test_communicator.cpp)
target_link_libraries(test_communicator em_core)

add_executable(test_engine src/test_engine.cpp)
target_link_libraries(test_engine em_core)

add_executable(test_full_loop src/test_full_loop.cpp)
target_link_libraries(test_full_loop em_core)
