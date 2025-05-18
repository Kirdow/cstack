cmake_minimum_required(VERSION 3.10)
project(cstack C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

file(GLOB_RECURSE SOURCE_FILES "src/*.c" "src/*.s")

add_executable(cstack ${SOURCE_FILES})
target_include_directories(cstack PRIVATE src)

add_custom_command(
    TARGET cstack POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_SOURCE_DIR}/compile_commands.json
    COMMENT "Creating symlink to compile_commands.json"
)
