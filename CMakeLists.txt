cmake_minimum_required(VERSION 3.16)

project(twink VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(TWINK_BUILD_TESTS "Build twink tests" OFF)
option(TWINK_BUILD_EXAMPLES "Build twink examples" OFF)
option(TWINK_INSTALL "Install twink targets and package files" ON)

add_library(twink
    src/base_tween.cpp
    src/easing.cpp
    src/timeline.cpp
    src/tween.cpp
    src/tween_manager.cpp
)
add_library(twink::twink ALIAS twink)

target_compile_features(twink PUBLIC cxx_std_17)
target_include_directories(twink
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

if(MSVC)
    target_compile_options(twink PRIVATE /W4)
else()
    target_compile_options(twink PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(TWINK_BUILD_TESTS)
    enable_testing()
    add_executable(twink_tests tests/twink_tests.cpp)
    target_link_libraries(twink_tests PRIVATE twink::twink)
    add_test(NAME twink_tests COMMAND twink_tests)
endif()

if(TWINK_BUILD_EXAMPLES)
    add_executable(twink_basic_transform examples/basic_transform.cpp)
    target_link_libraries(twink_basic_transform PRIVATE twink::twink)
endif()

if(TWINK_INSTALL)
    install(TARGETS twink
        EXPORT TwinkTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
    install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    install(EXPORT TwinkTargets
        NAMESPACE twink::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/twink
    )

    configure_package_config_file(
        cmake/TwinkConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/TwinkConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/twink
    )
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/TwinkConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )
    configure_file(${CMAKE_CURRENT_BINARY_DIR}/TwinkConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/twinkConfig.cmake COPYONLY)
    configure_file(${CMAKE_CURRENT_BINARY_DIR}/TwinkConfigVersion.cmake ${CMAKE_CURRENT_BINARY_DIR}/twinkConfigVersion.cmake COPYONLY)
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/TwinkConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/TwinkConfigVersion.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/twinkConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/twinkConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/twink
    )
endif()
