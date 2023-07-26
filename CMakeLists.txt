cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

macro(link_target_to_source targetName)
    add_custom_command(
        TARGET ${targetName}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink "$<TARGET_FILE:${targetName}>" "${CMAKE_CURRENT_SOURCE_DIR}/${targetName}"
        VERBATIM)
endmacro()

add_subdirectory(src)