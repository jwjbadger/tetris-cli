cmake_minimum_required(VERSION 3.20)
project(tetris-cli)

# Set the output directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include FTXUI using FetchContent
include(FetchContent)

# Fetch and include FTXUI
FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
    GIT_TAG main
)

FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
    FetchContent_Populate(ftxui)
    add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR})
endif()

# Create the executable
add_executable(tetris ../src/main.cpp)

# Include directories for FTXUI
target_include_directories(tetris PRIVATE ${ftxui_SOURCE_DIR})

# Link with the FTXUI library
target_link_libraries(tetris 
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component)