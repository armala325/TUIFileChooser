cmake_minimum_required(VERSION 3.10)
project(TUIFileChooser LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Curses REQUIRED)

set(TFC_SRC_FILES
    src/main.cpp
    src/App.cpp
    src/FileView.cpp
)

add_executable(tfc ${TFC_SRC_FILES})

target_include_directories(tfc PRIVATE
    include
    ${Curses_INCLUDE_DIRS}
)
target_link_libraries(tfc PRIVATE ${CURSES_LIBRARIES})

if(CMAKE_EXPORT_COMPILE_COMMANDS)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            "${CMAKE_BINARY_DIR}/compile_commands.json"
            "${CMAKE_SOURCE_DIR}/compile_commands.json"
    )
endif()
