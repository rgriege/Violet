#ifndef INPUT_SYSTEM_H
#define INPUT_SYSTEM_H

#include "violet/core/system/System.h"
#include "violet/plugins/input/component/InputComponent.h"
#include "violet/core/window/MouseButton.h"

namespace Violet
{
	class VIOLET_INPUT_API InputSystem : public ComponentSystem<InputComponent>
	{
	public:

		enum InputResult
		{
			IR_pass,
			IR_block
		};

	public:

		static void install(SystemFactory & factory);
		static std::unique_ptr<System> init(Deserializer & deserializer);

	public:

		InputSystem(InputSystem && other);

		virtual ~InputSystem() override = default;
		virtual void update(float dt, Engine & engine);

	private:

		void onMouseMove(int x, int y, int xrel, int yrel, Engine & engine);
		void onMouseDown(int x, int y, MouseButton button, Engine & engine);
		void onMouseUp(int x, int y, MouseButton button, Engine & engine);
		void onKeyDown(unsigned char key, Engine & engine);
		void onKeyUp(unsigned char key, Engine & engine);

	private:

		InputSystem() = default;
	};
}

#endif