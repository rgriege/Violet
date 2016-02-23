# Create project
project (Editor)

set (Editor_ROOT ${Violet_SRC}/editor)

# define header and source files for the library
set (HEADER_FILES
	${Editor_ROOT}/command/chain_command.h
	${Editor_ROOT}/command/clear_command.h
	${Editor_ROOT}/command/command.h
	${Editor_ROOT}/command/delete_command.h
	${Editor_ROOT}/command/file/open_command.h
	${Editor_ROOT}/command/file/save_all_command.h
	${Editor_ROOT}/command/move_to_command.h
	${Editor_ROOT}/command/resize_command.h
	${Editor_ROOT}/command/selection/deselect_command.h
	${Editor_ROOT}/command/selection/select_command.h
	${Editor_ROOT}/command/set_color_command.h
	${Editor_ROOT}/command/set_text_command.h
	${Editor_ROOT}/component/editor_component.h
	${Editor_ROOT}/cpp_script_exports.h
	${Editor_ROOT}/defines.h
	${Editor_ROOT}/editor_system.h
)

set (SOURCE_FILES
	${Editor_ROOT}/command/chain_command.cpp
	${Editor_ROOT}/command/clear_command.cpp
	${Editor_ROOT}/command/delete_command.cpp
	${Editor_ROOT}/command/file/open_command.cpp
	${Editor_ROOT}/command/file/save_all_command.cpp
	${Editor_ROOT}/command/move_to_command.cpp
	${Editor_ROOT}/command/resize_command.cpp
	${Editor_ROOT}/command/selection/deselect_command.cpp
	${Editor_ROOT}/command/selection/select_command.cpp
	${Editor_ROOT}/command/set_color_command.cpp
	${Editor_ROOT}/command/set_text_command.cpp
	${Editor_ROOT}/component/editor_component.cpp
	${Editor_ROOT}/cpp_script_exports.cpp
	${Editor_ROOT}/editor_system.cpp
)

include_directories (${Violet_SRC})

# Setup for library find
set (POSSIBLE_LIBRARIES OpenGL GLEW Freetype PNG Lua JsonCpp SDL2 Engine)

gather_libraries ()
add_library (Editor SHARED ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries (Editor ${LIBRARIES})

