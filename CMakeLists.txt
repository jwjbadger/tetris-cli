# Set the output directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include FTXUI in your project
add_subdirectory(FTXUI)


# Create the executable
add_executable(a src/main.cpp)

target_include_directories(a PRIVATE FTXUI)

# Link with the FTXUI library
target_link_libraries(a 
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component)
