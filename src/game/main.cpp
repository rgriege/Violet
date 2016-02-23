#include <iostream>

#include "editor/editor_system.h"
#include "editor/command/clear_command.h"
#include "editor/command/delete_command.h"
#include "editor/command/move_to_command.h"
#include "editor/command/resize_command.h"
#include "editor/command/set_color_command.h"
#include "editor/command/set_text_command.h"
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
#include "violet/log/log.h"
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

vlt::System_Factory setup()
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

	vlt::Json_Deserializer::install();
	vlt::Json_Serializer::install();
	// vlt::Binary_Deserializer::install();

	vlt::Lua_Script::install();
	vlt::Cpp_Script::install();

	vlt::System_Factory Factory;
	// vlt::Glut_Window_System::install(Factory);
	vlt::Sdl_Window_System::install(Factory);
	vlt::Physics_System::install(Factory);
	vlt::Render_System::install(Factory);
	vlt::Input_System::install(Factory);
	vlt::Transform_System::install(Factory);
	vlt::Update_System::install(Factory);
	edt::Editor_System::install(Factory);

	Editor_System::register_command<Clear_Command>();
	Editor_System::register_command<Delete_Command>();
	Editor_System::register_command<Deselect_Command>();
	Editor_System::register_command<Move_To_Command>();
	Editor_System::register_command<Open_Command>();
	Editor_System::register_command<Resize_Command>();
	Editor_System::register_command<Save_All_Command>();
	Editor_System::register_command<Select_Command>();
	Editor_System::register_command<Set_Color_Command>();
	Editor_System::register_command<Set_Text_Command>();

	return Factory;
}

int main(int /*argc*/, char ** /*argv*/)
{
	const vlt::Log_Target::Guard consoleLogGuard(vlt::install_log_target(std::make_unique<vlt::Console_Log_Target>()));
	const vlt::Log_Target::Guard fileLogGuard(vlt::install_log_target(vlt::File_Log_Target::create("Log.txt")));

	auto factory = setup();
	if (!vlt::Engine::bootstrap(factory, "editorConfig.json"))
	{
		vlt::log("failed to init Engine");
		char c;
		std::cin >> c;
		exit(1);
	}
}
