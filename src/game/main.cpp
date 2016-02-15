#include <iostream>

#include "editor/editor_system.h"
#include "editor/command/clear_all_command.h"
#include "editor/command/move_to_command.h"
#include "editor/command/file/open_command.h"
#include "editor/command/file/save_all_command.h"
#include "editor/command/selection/deselect_command.h"
#include "editor/command/selection/select_command.h"
#include "editor/component/editor_component.h"
#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/component/mapped_data_component.h"
#include "violet/graphics/component/color_component.h"
#include "violet/graphics/component/text_component.h"
#include "violet/graphics/component/texture_component.h"
#include "violet/graphics/system/render_system.h"
#include "violet/input/component/key_input_component.h"
#include "violet/input/component/mouse_input_component.h"
#include "violet/input/system/input_system.h"
#include "violet/log/Log.h"
#include "violet/log/console_log_target.h"
#include "violet/log/file_log_target.h"
#include "violet/physics/system/physics_system.h"
#include "violet/script/script_component.h"
#include "violet/script/cpp/cpp_script.h"
#include "violet/script/lua/lua_script.h"
#include "violet/serialization/file/file_deserializer_factory.h"
#include "violet/serialization/json/json_deserializer.h"
#include "violet/serialization/json/json_serializer.h"
#include "violet/system/system_factory.h"
#include "violet/transform/component/local_transform_component.h"
#include "violet/transform/component/world_transform_component.h"
#include "violet/transform/system/transform_system.h"
#include "violet/update/component/update_component.h"
#include "violet/update/system/update_system.h"
#include "violet/window/glut/glut_window_system.h"
#include "violet/window/sdl/sdl_window_system.h"

using namespace edt;

vlt::system_factory setup()
{
	vlt::install_color_component();
	vlt::install_key_input_component();
	vlt::install_local_transform_component();
	vlt::install_mapped_data_component();
	vlt::install_mouse_input_component();
	vlt::install_physics_component();
	vlt::install_script_component();
	vlt::install_text_component();
	vlt::install_texture_component();
	vlt::install_update_component();
	vlt::install_world_transform_component();
	install_editor_component();

	vlt::json_deserializer::install();
	vlt::json_serializer::install();
	// vlt::binary_deserializer::install();

	vlt::lua_script::install();
	vlt::cpp_script::install();

	vlt::system_factory factory;
	// vlt::glut_window_system::install(factory);
	vlt::sdl_window_system::install(factory);
	vlt::physics_system::install(factory);
	vlt::render_system::install(factory);
	vlt::input_system::install(factory);
	vlt::transform_system::install(factory);
	vlt::update_system::install(factory);
	edt::editor_system::install(factory);

	editor_system::register_command<clear_all_command>();
	editor_system::register_command<deselect_command>();
	editor_system::register_command<move_to_command>();
	editor_system::register_command<open_command>();
	editor_system::register_command<save_all_command>();
	editor_system::register_command<select_command>();

	return factory;
}

int main(int /*argc*/, char ** /*argv*/)
{
	const vlt::log_target::guard consoleLogGuard(vlt::install_log_target(std::make_unique<vlt::console_log_target>()));
	const vlt::log_target::guard fileLogGuard(vlt::install_log_target(vlt::file_log_target::create("log.txt")));

	auto factory = setup();
	if (!vlt::engine::bootstrap(factory, "demoConfig.json"))
	{
		vlt::log("failed to init engine");
		char c;
		std::cin >> c;
		exit(1);
	}
}
