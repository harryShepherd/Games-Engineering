add_library(level_loader level_system.cpp)

target_include_directories(level_loader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
    level_loader
    PUBLIC
    sfml-graphics
)