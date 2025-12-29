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

add_executable(UnitTests
    tests/UnitTests.cpp
)

target_include_directories(UnitTests PRIVATE
    source/arduino
    source/arduino/hal_mock
)

target_link_libraries(UnitTests PRIVATE
    DeskController
    GTest::gtest
)

include(GoogleTest)
gtest_discover_tests(UnitTests)
