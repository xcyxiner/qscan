cmake_minimum_required(VERSION 3.20)

project(qscan_demo_prototype VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Test Concurrent)

function(qscan_pick_import_library out_var target_name)
    foreach(prop
        IMPORTED_IMPLIB_RELEASE
        IMPORTED_IMPLIB_RELWITHDEBINFO
        IMPORTED_IMPLIB_MINSIZEREL
        IMPORTED_IMPLIB_DEBUG
        IMPORTED_IMPLIB)
        get_target_property(candidate ${target_name} ${prop})
        if(candidate)
            set(${out_var} "${candidate}" PARENT_SCOPE)
            return()
        endif()
    endforeach()

    set(${out_var} "" PARENT_SCOPE)
endfunction()

if(MINGW)
    qscan_pick_import_library(QSCAN_QT_CORE_IMPLIB Qt6::Core)
    if(QSCAN_QT_CORE_IMPLIB MATCHES "\\.lib$")
        message(FATAL_ERROR
            "qscan requires a MinGW-built Qt6 when using a MinGW kit.\n"
            "Resolved Qt import library: ${QSCAN_QT_CORE_IMPLIB}\n"
            "This usually means CMake picked an MSVC/Conda Qt package.\n"
            "In Qt Creator, select a Qt 6 MinGW kit or set CMAKE_PREFIX_PATH to a MinGW Qt6 installation.")
    endif()
endif()

add_library(qscan_core
    src/app/AppState.cpp
    src/app/ApplicationController.cpp
    src/pipeline/MockScannerDevice.cpp
    src/pipeline/ImageProcessor.cpp
    src/pipeline/ScanPipeline.cpp
    src/ui/MainWindow.cpp
    src/ui/ScanViewWidget.cpp
    src/ui/ControlPanel.cpp
    src/ui/StatusPanel.cpp
    src/ui/LogPanel.cpp
)

target_include_directories(qscan_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(qscan_core
    PUBLIC
    Qt6::Widgets
    Qt6::Concurrent
)

add_executable(qscan_demo
    src/main.cpp
)

target_link_libraries(qscan_demo
    PRIVATE
        qscan_core
)

enable_testing()

add_subdirectory(tests)
