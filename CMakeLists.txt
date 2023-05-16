cmake_minimum_required(VERSION 3.1)

# Set the project name
project(gui2 LANGUAGES CXX)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

add_subdirectory(gui)

add_executable(gui2 main.cpp include/gui.hpp src/gui.cpp)

target_include_directories(gui2 PRIVATE include SFML/include)

target_link_libraries(gui2 PRIVATE sfml_lib)

install(TARGETS ${PROJECT_NAME})