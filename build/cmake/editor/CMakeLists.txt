# Create project
project (Editor)

set (Editor_ROOT ${Violet_SRC}/editor)

# define header and source files for the library
set (HEADER_FILES
    ${Editor_ROOT}/EditorConfig.h
    ${Editor_ROOT}/EditorSystem.h
)

set (SOURCE_FILES
    ${Editor_ROOT}/EditorSystem.cpp
)

include_directories (${Violet_SRC})

# Setup for library find
set (POSSIBLE_LIBRARIES OpenGL GLEW Freetype PNG Lua JsonCpp SDL2 Engine)

gather_libraries ()
add_library (Editor SHARED ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries (Editor ${LIBRARIES})

