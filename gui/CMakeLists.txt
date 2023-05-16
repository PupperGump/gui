project(sfml_lib)
cmake_minimum_required(VERSION 3.21)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/PupperGump/SFML.git
    GIT_TAG puppergump_master)
FetchContent_MakeAvailable(SFML)

add_library(${PROJECT_NAME} ${SFML_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/SFML/include)

target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-window sfml-system)

if (WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}> COMMAND_EXPAND_LISTS)
endif()

install(TARGETS ${PROJECT_NAME})