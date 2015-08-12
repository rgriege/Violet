# Create project
project (Game)

set (Game_ROOT ${Violet_SRC}/game)

# define header and source files for the library
set (HEADER_FILES
    ${Game_ROOT}/Config.h
    ${Game_ROOT}/pathfinding/Map.h
    ${Game_ROOT}/pathfinding/MapComponent.h
    ${Game_ROOT}/pathfinding/Path.h
    ${Game_ROOT}/pathfinding/PathComponent.h
    ${Game_ROOT}/pathfinding/PathfindingComponent.h
    ${Game_ROOT}/pathfinding/PathfindingSystem.h
    ${Game_ROOT}/world/WorldSystem.h
)

set (SOURCE_FILES
    ${Game_ROOT}/pathfinding/Map.cpp
    ${Game_ROOT}/pathfinding/Path.cpp
    ${Game_ROOT}/pathfinding/MapComponent.cpp
    ${Game_ROOT}/pathfinding/PathComponent.cpp
    ${Game_ROOT}/pathfinding/PathfindingComponent.cpp
    ${Game_ROOT}/pathfinding/PathfindingSystem.cpp
    ${Game_ROOT}/world/WorldSystem.cpp
)

include_directories (${Violet_SRC})

# Setup for library find
set (POSSIBLE_LIBRARIES OpenGL GLEW Freetype PNG Lua JsonCpp SDL2 Engine)

gather_libraries ()
add_library (Game SHARED ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries (Game ${LIBRARIES})

