cmake_minimum_required(VERSION 3.21)

# Set the project name

project(gui2 LANGUAGES CXX)


include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/PupperGump/SFML.git
    GIT_TAG puppergump_master)
FetchContent_MakeAvailable(SFML)

add_executable(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/main.cpp ${CMAKE_SOURCE_DIR}/include/gui.hpp ${CMAKE_SOURCE_DIR}/src/gui.cpp ${SFML_SOURCES})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/SFML/include)

target_link_libraries(${PROJECT_NAME} PUBLIC sfml-graphics sfml-window)

if (WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}> COMMAND_EXPAND_LISTS)
endif()

install(TARGETS ${PROJECT_NAME})