cmake_minimum_required(VERSION 3.10)
project(TaskDispatcher LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------------------------------------------------
# Switches: TESTING MODE & STEALING MODE
# -------------------------------------------------------------------
option(BUILD_TESTING_MODE "Enable testing mode (GoogleTest + TESTING defines)" OFF)
option(ENABLE_STEALING "Enable stealing in Worker" ON)
option(BUILD_BENCHMARK "Turn on to build benchmarks" ON)

if(ENABLE_STEALING)
    add_compile_definitions(STEALING)
endif()

if(BUILD_TESTING_MODE)
    add_compile_definitions(TESTING)
endif()

# -------------------------------------------------------------------
# Project sources
# -------------------------------------------------------------------
file(GLOB_RECURSE SOURCES
    src/*.cpp
    src/*.hpp
)

# -------------------------------------------------------------------
# Library target
# -------------------------------------------------------------------
add_library(TaskDispatcherLib ${SOURCES})
target_include_directories(TaskDispatcherLib PUBLIC src)

# -------------------------------------------------------------------
# Tests
# -------------------------------------------------------------------
if(BUILD_TESTING_MODE)
    include(FetchContent)

    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/heads/main.zip
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    file(GLOB_RECURSE TEST_SOURCES
        tests/*.cpp
        tests/*.hpp
    )

    add_executable(UnitTests ${TEST_SOURCES})
    target_link_libraries(UnitTests
        PRIVATE
            TaskDispatcherLib
            GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(UnitTests)
endif()

if(BUILD_BENCHMARK OR BUILD_TESTING_MODE)
    file(GLOB_RECURSE WORKLOAD_SOURCES
        workloads/*.cpp
        workloads/*.hpp
    )

    list(FILTER WORKLOAD_SOURCES INCLUDE REGEX "\\.cpp$")
    
    if(WORKLOAD_SOURCES)
        add_library(Workload ${WORKLOAD_SOURCES})
    endif()
endif()

if(BUILD_BENCHMARK)

    FetchContent_Declare(
    benchmark
    URL https://github.com/google/benchmark/archive/refs/heads/main.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    
    file(GLOB BENCHMARK_FILES benchmarks/*.cpp)
    foreach(BENCH_SRC ${BENCHMARK_FILES})
        get_filename_component(BM_NAME ${BENCH_SRC} NAME_WE)
        add_executable(${BM_NAME} ${BENCH_SRC})
        target_link_libraries(${BM_NAME} PRIVATE TaskDispatcherLib benchmark::benchmark)
        set_target_properties(${BM_NAME} PROPERTIES
           RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/benchmarks
        )
    endforeach()




    FetchContent_MakeAvailable(benchmark)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
