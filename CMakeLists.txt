cmake_minimum_required(VERSION 3.8)
project(Parser CXX)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" ${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${Parser_SOURCE_DIR}/bin)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fsanitize=undefined")

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(CodeCoverage)
    append_coverage_compiler_flags()

    setup_target_for_coverage_gcovr_html(
        NAME parser-coverage
        EXECUTABLE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_main"
        EXCLUDE "${PROJECT_SOURCE_DIR}/tests" "${PROJECT_SOURCE_DIR}/build" "${PROJECT_SOURCE_DIR}/src/main.cpp"
        DEPENDENCIES test_main
    )
endif()

include_directories(include)
add_subdirectory(src)
add_subdirectory(tests)

enable_testing()
