# Create project
project (Editor)

set (Editor_ROOT ${Violet_SRC}/editor)

# define header and source files for the library
set (HEADER_FILES
    ${Editor_ROOT}/CppScriptExports.h
    ${Editor_ROOT}/Defines.h
    ${Editor_ROOT}/EditorSystem.h
    ${Editor_ROOT}/command/Command.h
    ${Editor_ROOT}/command/ClearAllCommand.h
    ${Editor_ROOT}/command/file/OpenCommand.h
    ${Editor_ROOT}/command/file/SaveAllCommand.h
    ${Editor_ROOT}/component/EditorComponent.h
    ${Editor_ROOT}/component/EditorComponentWrapper.h
)

set (SOURCE_FILES
    ${Editor_ROOT}/CppScriptExports.cpp
    ${Editor_ROOT}/command/ClearAllCommand.cpp
    ${Editor_ROOT}/command/file/OpenCommand.cpp
    ${Editor_ROOT}/command/file/SaveAllCommand.cpp
    ${Editor_ROOT}/component/EditorComponent.cpp
    ${Editor_ROOT}/EditorSystem.cpp
)

include_directories (${Violet_SRC})

# Setup for library find
set (POSSIBLE_LIBRARIES OpenGL GLEW Freetype PNG Lua JsonCpp SDL2 Engine)

gather_libraries ()
add_library (Editor SHARED ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries (Editor ${LIBRARIES})

