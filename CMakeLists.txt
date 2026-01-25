cmake_minimum_required(VERSION 3.14)
project(DeskAutomation)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Core desk automation library (production code, no .ino)
add_library(DeskAutomation STATIC
  src/hal.cpp
  src/desk_app.cpp
  src/motor_controller.cpp
  tests/hal_mock/HALMock.cpp
  tests/hal_mock/SerialMock.cpp
)

target_include_directories(DeskAutomation PUBLIC 
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/tests
)

# Define TESTENVIRONMENT for HAL mock usage in test builds
target_compile_definitions(DeskAutomation PUBLIC
    TESTENVIRONMENT
)

# Component tests executable - Application layer logic
add_executable(ComponentTests
  tests/ComponentTests.cpp
)

target_compile_definitions(ComponentTests PRIVATE 
    DESK_CONTROLLER_ENABLE_TEST_INTERFACE
)

target_include_directories(ComponentTests PRIVATE
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/tests
  ${CMAKE_SOURCE_DIR}/tests/hal_mock
)

target_link_libraries(ComponentTests PRIVATE
    DeskAutomation
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(ComponentTests PROPERTIES LABELS "Component")

# Unit tests executable - Motor controller signal processing
add_executable(UnitTests
  tests/UnitTests.cpp
)

target_compile_definitions(UnitTests PRIVATE 
    DESK_CONTROLLER_ENABLE_TEST_INTERFACE
)

target_include_directories(UnitTests PRIVATE
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/tests
  ${CMAKE_SOURCE_DIR}/tests/hal_mock
)

target_link_libraries(UnitTests PRIVATE
    DeskAutomation
    GTest::gtest_main
)

gtest_discover_tests(UnitTests PROPERTIES LABELS "Unit")

# Integration tests executable - HAL/Signal layer
add_executable(IntegrationTests
  tests/IntegrationTests.cpp
)

target_compile_definitions(IntegrationTests PRIVATE 
    DESK_CONTROLLER_ENABLE_TEST_INTERFACE
)

target_include_directories(IntegrationTests PRIVATE
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/tests
  ${CMAKE_SOURCE_DIR}/tests/hal_mock
)

target_link_libraries(IntegrationTests PRIVATE
    DeskAutomation
    GTest::gtest_main
)

gtest_discover_tests(IntegrationTests PROPERTIES LABELS "Integration")
