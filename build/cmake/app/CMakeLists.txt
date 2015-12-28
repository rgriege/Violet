# Create project
project (App)

set (App_ROOT ${Violet_SRC}/game)

set (SOURCE_FILES
    ${App_ROOT}/main.cpp
)

include_directories (${Violet_SRC})

# Setup for library find
set (POSSIBLE_LIBRARIES OpenGL GLEW Freetype PNG Lua JsonCpp SDL2 Engine Editor)

gather_libraries ()
add_executable (App ${SOURCE_FILES})
target_link_libraries (App ${LIBRARIES})

