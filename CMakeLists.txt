cmake_minimum_required(VERSION 3.1)

# Set the project name
project(gui2 LANGUAGES CXX)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)


add_subdirectory(gui)