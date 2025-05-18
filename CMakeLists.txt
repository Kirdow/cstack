cmake_minimum_required(VERSION 3.10)
project(cstack C)

set(CMAKE_C_STANDARD 23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(UNITY_VERSION 2.5.2)
find_package(unity ${UNITY_VERSION} QUIET)
if (NOT unity_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        unity
        URL https://github.com/ThrowTheSwitch/Unity/archive/refs/tags/v${UNITY_VERSION}.tar.gz
    )

    FetchContent_GetProperties(unity)
    if (NOT unity_POPULATED)
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(unity)
        set(UNITY_SOURCE_DIR ${unity_SOURCE_DIR})
        set(UNITY_INCLUDE_DIR ${unity_SOURCE_DIR}/src)
        set(UNITY_SOURCES ${unity_SOURCE_DIR}/src/unity.c)
    endif()
endif()

add_library(unity STATIC ${UNITY_SOURCES})
target_include_directories(unity PUBLIC ${UNITY_INCLUDE_DIR})

# Source Files
file(GLOB_RECURSE SOURCE_FILES "src/*.c" "src/*.s")

# Test Source Files
file(GLOB_RECURSE SOURCE_TEST_FILES "src/*.c" "src/*.s")
list(REMOVE_ITEM SOURCE_TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c")

# Test Files
file(GLOB_RECURSE TEST_SOURCES "test/*.c")

add_executable(cstack ${SOURCE_FILES})
target_include_directories(cstack PRIVATE src)

add_executable(test_run ${SOURCE_TEST_FILES} ${UNITY_SOURCES} ${TEST_SOURCES})
target_include_directories(test_run PRIVATE src ${UNITY_INCLUDE_DIR} test)

add_custom_target(run
    COMMAND cstack
    DEPENDS cstack
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_custom_target(test
    COMMAND test_run
    DEPENDS test_run
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_custom_command(
    TARGET cstack test_run POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_SOURCE_DIR}/compile_commands.json
    COMMENT "Creating symlink to compile_commands.json"
)
