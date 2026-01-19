cmake_minimum_required(VERSION 3.14)
project(DeskAutomation)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_library(DeskController STATIC source/arduino/DeskController.cpp)
target_include_directories(DeskController PUBLIC source/arduino source/arduino/hal_mock)

add_executable(SoftwareTests
  tests/ComponentTests.cpp
  tests/IntegrationTests.cpp
)

# Ensure HAL and application sources are linked into the test binary so HAL_Init() and other symbols are available
target_sources(SoftwareTests PRIVATE
  ${CMAKE_SOURCE_DIR}/source/arduino/HAL.cpp
  ${CMAKE_SOURCE_DIR}/source/arduino/DeskController.cpp
  ${CMAKE_SOURCE_DIR}/source/arduino/hal_mock/HALMock.cpp
  ${CMAKE_SOURCE_DIR}/source/arduino/hal_mock/SerialMock.cpp
)

# Enable test interface for white-box testing
target_compile_definitions(SoftwareTests PRIVATE 
    DESK_CONTROLLER_ENABLE_TEST_INTERFACE
)

# ensure include path for test build
target_include_directories(SoftwareTests PRIVATE
  ${CMAKE_SOURCE_DIR}/source/arduino
  ${CMAKE_SOURCE_DIR}/source/arduino/hal_mock
  ${CMAKE_SOURCE_DIR}/tests
)

target_link_libraries(SoftwareTests PRIVATE
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(SoftwareTests)

# Add unit tests executable
add_executable(UnitTests
  tests/UnitTests.cpp
)

target_sources(UnitTests PRIVATE
  ${CMAKE_SOURCE_DIR}/source/arduino/DeskController.cpp
)

# Enable test interface for white-box testing
target_compile_definitions(UnitTests PRIVATE 
    DESK_CONTROLLER_ENABLE_TEST_INTERFACE
)

target_include_directories(UnitTests PRIVATE
  ${CMAKE_SOURCE_DIR}/source/arduino
  ${CMAKE_SOURCE_DIR}/tests
)

target_link_libraries(UnitTests PRIVATE
    GTest::gtest_main
)

gtest_discover_tests(UnitTests)
