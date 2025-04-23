cmake_minimum_required(VERSION 3.30)
project(SearchEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(MY_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(RESOURCE_DIR ${CMAKE_SOURCE_DIR}/resourses)
set(JSON_BuildTests OFF CACHE INTERNAL "")

add_executable(SearchEngine src/main.cpp tests/SearchEngineTest.cpp)

add_subdirectory(src)
add_subdirectory(tests)

enable_testing()
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)
add_test(search_engine_gtests GTest::GTest)

target_link_libraries(SearchEngine
        PRIVATE
        search_engine
        GTest::GTest
        nlohmann_json::nlohmann_json)