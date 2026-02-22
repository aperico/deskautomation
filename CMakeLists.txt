cmake_minimum_required(VERSION 3.14)
project(DeskAutomation)

# Performance optimizations for faster configuration
set(CMAKE_MESSAGE_LOG_LEVEL WARNING)  # Suppress verbose INFO messages
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG_MODE ON)  # Prefer config mode for faster finds
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch Google Test
include(FetchContent)

# Performance optimization: Set DOWNLOAD_EXTRACT_TIMESTAMP to use file timestamps
# This suppresses CMake warning and speeds up cached downloads
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.24)
    cmake_policy(SET CMP0135 NEW)
    set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE ALWAYS)  # Try to find existing Google Test first
endif()

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_NO_PROGRESS ON  # Suppress download progress messages for cleaner output
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Performance: Make Google Test available (uses cache if already downloaded)
FetchContent_MakeAvailable(googletest)

enable_testing()

# ============================================================================
# MOTOR DRIVER TYPE CONFIGURATION
# ============================================================================
# Select which motor driver implementation to use:
# - MT_BASIC (0):   L298N Dual H-Bridge (default, cost-optimized)
# - MT_ROBUST (1):  IBT_2 Intelligent Motor Driver (high-capacity)
#
# This affects which pins and control signals are used.
# The HAL provides a unified interface regardless of driver type.
#
# Usage: cmake -S . -B build -DMOTOR_TYPE=MT_ROBUST
# ============================================================================

if(NOT DEFINED MOTOR_TYPE)
    set(MOTOR_TYPE "MT_BASIC" CACHE STRING "Motor driver type: MT_BASIC or MT_ROBUST")
    set_property(CACHE MOTOR_TYPE PROPERTY STRINGS "MT_BASIC" "MT_ROBUST")
    message(STATUS "MOTOR_TYPE not specified; using default: MT_BASIC (L298N)")
else()
    message(STATUS "Motor Driver Type: ${MOTOR_TYPE}")
    
    # Validate MOTOR_TYPE value
    if(NOT (MOTOR_TYPE STREQUAL "MT_BASIC" OR MOTOR_TYPE STREQUAL "MT_ROBUST"))
        message(FATAL_ERROR "MOTOR_TYPE must be 'MT_BASIC' or 'MT_ROBUST', got: ${MOTOR_TYPE}")
    endif()
endif()

# Coverage configuration for Debug builds
if(CMAKE_BUILD_TYPE MATCHES Debug)
  if(MSVC)
    # MSVC doesn't support --coverage, use alternatives
    message(STATUS "Building for MSVC - coverage flags not added")
  else()
    # GCC/Clang coverage flags
    add_compile_options(--coverage)
    add_link_options(--coverage)
    message(STATUS "Coverage flags enabled for Debug build")
  endif()
endif()

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
# Also define MOTOR_TYPE compile definition so C++ source can use #if MOTOR_TYPE == MT_BASIC
target_compile_definitions(DeskAutomation PUBLIC
    TESTENVIRONMENT
    MOTOR_TYPE=${MOTOR_TYPE}
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
