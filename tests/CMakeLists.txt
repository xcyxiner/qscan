function(add_qscan_test target_name source_file)
    add_executable(${target_name}
        ${source_file}
    )

    target_include_directories(${target_name}
        PRIVATE
            ${CMAKE_SOURCE_DIR}/src
    )

    target_link_libraries(${target_name}
        PRIVATE
            qscan_core
            Qt${QT_VERSION_MAJOR}::Test
    )

    add_test(NAME ${target_name} COMMAND ${target_name})
endfunction()

add_qscan_test(qscan_application_controller_tests ApplicationControllerTests.cpp)
add_qscan_test(qscan_mock_scanner_tests MockScannerDeviceTests.cpp)
add_qscan_test(qscan_image_processor_tests ImageProcessorTests.cpp)
add_qscan_test(qscan_app_state_integration_tests AppStateIntegrationTests.cpp)
