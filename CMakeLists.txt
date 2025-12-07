set(PROJECT_NAME "CubeZone")
project(${PROJECT_NAME})
cmake_minimum_required(VERSION 3.11)

# Require modern C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#### Setup Directories ####
SET(OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})

#### Add External Dependencies ####

#### SFML ####
add_subdirectory("lib/SFML")
set(SFML_INCS "lib/SFML/include")
link_directories("${CMAKE_BINARY_DIR}/lib/SFML/lib")

#### Box2D ####
add_subdirectory("lib/box2d")
set(B2D_INCS "lib/box2d/include")
link_directories("${CMAKE_BINARY_DIR}/lib/box2d")

#### Level Loading System ####
add_library(tile_level STATIC tile_level_loader/level_system.cpp)
target_include_directories(tile_level INTERFACE tile_level_loader)
target_link_libraries(tile_level sfml-graphics)

#### Engine ####
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    engine/*.cpp
)

add_library(engine STATIC ${ENGINE_SOURCES})
target_include_directories(engine INTERFACE engine ${SFML_INCS} ${B2D_INCS} tile_level_loader)
target_link_libraries(engine sfml-graphics box2d tile_level)

#### Executable ####
file(GLOB_RECURSE EXEC_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

add_executable(${PROJECT_NAME} ${EXEC_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${SFML_INCS} ${B2D_INCS} engine tile_level_loader)
target_link_libraries(${PROJECT_NAME} sfml-graphics box2d engine tile_level)
set_target_properties(${PROJECT_NAME} 
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
)

#### CPack stuff ####
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
include(CPack)

#### Resources Folder ####
add_custom_target(copy_resources ALL COMMAND ${CMAKE_COMMAND} 
  -E copy_directory
    "${PROJECT_SOURCE_DIR}/resources"
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources
)
add_dependencies(${PROJECT_NAME} copy_resources)

IF (WIN32)
    add_custom_target(copy_box2d_dll ALL COMMAND ${CMAKE_COMMAND}
        -E copy_directory
            "${CMAKE_BINARY_DIR}/lib/box2d/bin"
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )
ENDIF()