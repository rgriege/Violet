# Create project
project (Engine)

set (Engine_ROOT ${Violet_SRC}/violet)

# define header and source files for the library
set (HEADER_FILES
	${Engine_ROOT}/component/component_deserializer.h
	${Engine_ROOT}/component/component_metadata.h
	${Engine_ROOT}/component/component_pool.h
	${Engine_ROOT}/component/mapped_data_component.h
	${Engine_ROOT}/component/scene.h
	${Engine_ROOT}/core/defines.h
	${Engine_ROOT}/core/engine.h
	${Engine_ROOT}/core/handle.h
	${Engine_ROOT}/core/tag.h
	${Engine_ROOT}/core/types.h
	${Engine_ROOT}/event/event_context.h
	${Engine_ROOT}/event/event_context_owner.h
	${Engine_ROOT}/event/event.h
	${Engine_ROOT}/graphics/color.h
	${Engine_ROOT}/graphics/component/color_component.h
	${Engine_ROOT}/graphics/component/render_component_data.h
	${Engine_ROOT}/graphics/component/text_component.h
	${Engine_ROOT}/graphics/component/texture_component.h
	${Engine_ROOT}/graphics/font/font.h
	${Engine_ROOT}/graphics/mesh.h
	${Engine_ROOT}/graphics/shader/shader.h
	${Engine_ROOT}/graphics/system/render_system.h
	${Engine_ROOT}/graphics/texture/texture.h
	${Engine_ROOT}/handle/handle_manager.h
	${Engine_ROOT}/handle/versioned_handle_manager.h
	${Engine_ROOT}/input/component/key_input_component.h
	${Engine_ROOT}/input/component/mouse_input_component.h
	${Engine_ROOT}/input/input_result.h
	${Engine_ROOT}/input/key.h
	${Engine_ROOT}/input/system/input_system.h
	${Engine_ROOT}/log/console_log_target.h
	${Engine_ROOT}/log/file_log_target.h
	${Engine_ROOT}/log/log.h
	${Engine_ROOT}/math/aabb.h
	${Engine_ROOT}/math/circle.h
	${Engine_ROOT}/math/constants.h
	${Engine_ROOT}/math/interval.h
	${Engine_ROOT}/math/log_const.h
	${Engine_ROOT}/math/m2.h
	${Engine_ROOT}/math/m3.h
	${Engine_ROOT}/math/m4.h
	${Engine_ROOT}/math/poly.h
	${Engine_ROOT}/math/v2.h
	${Engine_ROOT}/math/v2i.h
	${Engine_ROOT}/math/v3.h
	${Engine_ROOT}/physics/collision/intersection.h
	${Engine_ROOT}/physics/collision/rigid_body.h
	${Engine_ROOT}/physics/component/physics_component.h
	${Engine_ROOT}/physics/system/physics_system.h
	${Engine_ROOT}/script/cpp/cpp_script.h
	${Engine_ROOT}/script/cpp/shared_library.h
	${Engine_ROOT}/script/lua/lua_library.h
	${Engine_ROOT}/script/lua/lua_script.h
	${Engine_ROOT}/script/script_component.h
	${Engine_ROOT}/script/script_factory.h
	${Engine_ROOT}/script/script.h
	${Engine_ROOT}/serialization/binary/binary_deserializer.h
	${Engine_ROOT}/serialization/binary/binary_serializer.h
	${Engine_ROOT}/serialization/deserializer.h
	${Engine_ROOT}/serialization/file/file_deserializer_factory.h
	${Engine_ROOT}/serialization/file/file_deserializer.h
	${Engine_ROOT}/serialization/file/file_serializer_factory.h
	${Engine_ROOT}/serialization/file/file_serializer.h
	${Engine_ROOT}/serialization/json/json_deserializer.h
	${Engine_ROOT}/serialization/json/json_serializer.h
	${Engine_ROOT}/serialization/serialization_utilities.h
	${Engine_ROOT}/serialization/serializer.h
	${Engine_ROOT}/structures/crs_graph.h
	${Engine_ROOT}/structures/quad_tree.h
	${Engine_ROOT}/structures/vector.h
	${Engine_ROOT}/system/system_factory.h
	${Engine_ROOT}/system/system.h
	${Engine_ROOT}/task/thread.h
	${Engine_ROOT}/template/template_utilities.h
	${Engine_ROOT}/template/tuple_utilities.h
	${Engine_ROOT}/time/timestamp.h
	${Engine_ROOT}/transform/component/local_transform_component.h
	${Engine_ROOT}/transform/component/world_transform_component.h
	${Engine_ROOT}/transform/system/transform_system.h
	${Engine_ROOT}/transform/transform.h
	${Engine_ROOT}/ui/ui_text_input_box.h
	${Engine_ROOT}/update/component/update_component.h
	${Engine_ROOT}/update/system/update_system.h
	${Engine_ROOT}/utility/buffer.h
	${Engine_ROOT}/utility/delegate.h
	${Engine_ROOT}/utility/factory.h
	${Engine_ROOT}/utility/file_utilities.h
	${Engine_ROOT}/utility/formatted_string.h
	${Engine_ROOT}/utility/free_list.h
	${Engine_ROOT}/utility/guard.h
	${Engine_ROOT}/utility/lent_ptr.h
	${Engine_ROOT}/utility/memory.h
	${Engine_ROOT}/utility/profiler.h
	${Engine_ROOT}/utility/random.h
	${Engine_ROOT}/utility/resource_cache.h
	${Engine_ROOT}/utility/shared_val.h
	${Engine_ROOT}/utility/string_utilities.h
	${Engine_ROOT}/utility/text_resource.h
	${Engine_ROOT}/utility/unique_val.h
	${Engine_ROOT}/window/glut/glut_window_system.h
	${Engine_ROOT}/window/mouse_button.h
	${Engine_ROOT}/window/sdl/sdl_window_system.h
	${Engine_ROOT}/window/window_system.h
)

set (SOURCE_FILES
	${Engine_ROOT}/component/component_deserializer.cpp
	${Engine_ROOT}/component/component_metadata.cpp
	${Engine_ROOT}/component/component_pool.cpp
	${Engine_ROOT}/component/mapped_data_component.cpp
	${Engine_ROOT}/component/scene.cpp
	${Engine_ROOT}/core/engine.cpp
	${Engine_ROOT}/core/handle.cpp
	${Engine_ROOT}/core/tag.cpp
	${Engine_ROOT}/event/event_context.cpp
	${Engine_ROOT}/event/event_context_owner.cpp
	${Engine_ROOT}/event/event.cpp
	${Engine_ROOT}/graphics/color.cpp
	${Engine_ROOT}/graphics/component/color_component.cpp
	${Engine_ROOT}/graphics/component/render_component_data.cpp
	${Engine_ROOT}/graphics/component/text_component.cpp
	${Engine_ROOT}/graphics/component/texture_component.cpp
	${Engine_ROOT}/graphics/font/font.cpp
	${Engine_ROOT}/graphics/mesh.cpp
	${Engine_ROOT}/graphics/shader/shader.cpp
	${Engine_ROOT}/graphics/system/render_system.cpp
	${Engine_ROOT}/graphics/texture/texture.cpp
	${Engine_ROOT}/handle/handle_manager.cpp
	${Engine_ROOT}/handle/versioned_handle_manager.cpp
	${Engine_ROOT}/input/component/key_input_component.cpp
	${Engine_ROOT}/input/component/mouse_input_component.cpp
	${Engine_ROOT}/input/key.cpp
	${Engine_ROOT}/input/system/input_system.cpp
	${Engine_ROOT}/log/console_log_target.cpp
	${Engine_ROOT}/log/file_log_target.cpp
	${Engine_ROOT}/log/log.cpp
	${Engine_ROOT}/math/aabb.cpp
	${Engine_ROOT}/math/circle.cpp
	${Engine_ROOT}/math/interval.cpp
	${Engine_ROOT}/math/m2.cpp
	${Engine_ROOT}/math/m3.cpp
	${Engine_ROOT}/math/m4.cpp
	${Engine_ROOT}/math/poly.cpp
	${Engine_ROOT}/math/v2.cpp
	${Engine_ROOT}/math/v2i.cpp
	${Engine_ROOT}/math/v3.cpp
	${Engine_ROOT}/physics/collision/intersection.cpp
	${Engine_ROOT}/physics/collision/rigid_body.cpp
	${Engine_ROOT}/physics/component/physics_component.cpp
	${Engine_ROOT}/physics/system/physics_system.cpp
	${Engine_ROOT}/script/cpp/cpp_script.cpp
	${Engine_ROOT}/script/cpp/unix_shared_library.cpp
	${Engine_ROOT}/script/lua/lua_library.cpp
	${Engine_ROOT}/script/lua/lua_script.cpp
	${Engine_ROOT}/script/script_component.cpp
	${Engine_ROOT}/script/script.cpp
	${Engine_ROOT}/script/script_factory.cpp
	${Engine_ROOT}/serialization/binary/binary_deserializer.cpp
	${Engine_ROOT}/serialization/binary/binary_serializer.cpp
	${Engine_ROOT}/serialization/file/file_deserializer_factory.cpp
	${Engine_ROOT}/serialization/file/file_serializer_factory.cpp
	${Engine_ROOT}/serialization/json/json_deserializer.cpp
	${Engine_ROOT}/serialization/json/json_serializer.cpp
	${Engine_ROOT}/system/system.cpp
	${Engine_ROOT}/task/thread.cpp
	${Engine_ROOT}/time/timestamp.cpp
	${Engine_ROOT}/transform/component/local_transform_component.cpp
	${Engine_ROOT}/transform/component/world_transform_component.cpp
	${Engine_ROOT}/transform/system/transform_system.cpp
	${Engine_ROOT}/transform/transform.cpp
	${Engine_ROOT}/ui/ui_text_input_box.cpp
	${Engine_ROOT}/update/component/update_component.cpp
	${Engine_ROOT}/update/system/update_system.cpp
	${Engine_ROOT}/utility/file_utilities.cpp
	${Engine_ROOT}/utility/free_list.cpp
	${Engine_ROOT}/utility/profiler.cpp
	${Engine_ROOT}/utility/random.cpp
	${Engine_ROOT}/utility/string_utilities.cpp
	${Engine_ROOT}/window/glut/glut_window_system.cpp
	${Engine_ROOT}/window/sdl/sdl_window_system.cpp
)

include_directories (${Violet_SRC})

set (PNG_FIND_VERSION "1.6.18")
set (POSSIBLE_LIBRARIES OpenGL GLEW Freetype PNG Lua JsonCpp SDL2 GLUT)
# Setup for library find
# set (POSSIBLE_LIBRARIES FFmpeg)
# set (FFmpeg_COMMAND FFmpeg REQUIRED COMPONENTS avcodec avutil swscale)
# set (FFmpeg_ADDITIONAL_DEFINITIONS -D__STDC_CONSTANT_MACROS)

gather_libraries ()
add_library (Engine SHARED ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries (Engine ${LIBRARIES})

