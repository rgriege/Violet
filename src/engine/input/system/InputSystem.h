#ifndef VIOLET_InputSystem_H
#define VIOLET_InputSystem_H

#include "engine/system/System.h"
#include "engine/input/component/InputComponent.h"
#include "engine/window/WindowSystem.h"

namespace Violet
{
	class SystemFactory;

	class VIOLET_API InputSystem : public System
	{
	public:

		static const char * getStaticLabel();
		static void install(SystemFactory & factory);
		static std::unique_ptr<System> init(Deserializer & deserializer);

	public:

		InputSystem(InputSystem && other);

		virtual ~InputSystem() override = default;
		virtual void update(float dt, Engine & engine);

	private:

		void onMouseMove(const WindowSystem::MotionEvent & event, Engine & engine);
		void onMouseDown(const WindowSystem::MouseEvent & event, Engine & engine);
		void onMouseUp(const WindowSystem::MouseEvent & event, Engine & engine);
		void onKeyDown(const WindowSystem::KeyEvent & event, Engine & engine);
		void onKeyUp(const WindowSystem::KeyEvent & event, Engine & engine);

	private:

		InputSystem();
	};
}

#endif