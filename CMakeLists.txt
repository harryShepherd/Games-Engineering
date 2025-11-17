set(PROJECT_NAME "CubeZone")

project(${PROJECT_NAME} CXX)
cmake_minimum_required(VERSION 3.11)
# Require modern C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Get sources
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

#### Setup Directories ####
#Main output directory
SET(OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
# Ouput all DLLs from all libs into main build folder
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})

#### Add External Dependencies ####
add_subdirectory("lib/SFML")
set(SFML_INCS "lib/SFML/include")
link_directories("${CMAKE_BINARY_DIR}/lib/SFML/lib")

add_subdirectory("lib/box2d")
set(B2D_INCS "lib/box2d/include")
link_directories("${CMAKE_BINARY_DIR}/lib/box2d")

# Engine
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    engine/*.cpp
)
add_library(engine STATIC ${ENGINE_SOURCES})
target_include_directories(engine INTERFACE engine ${SFML_INCS} ${B2D_INCS})
target_link_libraries(engine sfml-graphics box2d tile_level)

#add_subdirectory("lib/level_loader")

#### Practical 2 ####
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE )
target_include_directories(${PROJECT_NAME} PRIVATE ${SFML_INCS} ${B2D_INCS} engine)
#target_include_directories(${PROJECT_NAME} PRIVATE "lib/level_loader")
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/private)

#### level system ####
target_link_libraries(${PROJECT_NAME} PRIVATE box2d sfml-graphics engine) #level_loader

# Resources File
file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})