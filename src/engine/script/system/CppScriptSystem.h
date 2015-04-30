#ifndef CPP_SCRIPT_SYSTEM_H
#define CPP_SCRIPT_SYSTEM_H

#include "engine/system/System.h"
#include "engine/script/component/CppScriptComponent.h"

namespace Violet
{
	class VIOLET_API CppScriptSystem : public ComponentSystem<CppScriptComponent>
	{
	public:

		static void install(SystemFactory & factory);
		static std::unique_ptr<System> init(Deserializer & deserializer);

	public:

		CppScriptSystem(CppScriptSystem && other);

		virtual ~CppScriptSystem() override = default;
		virtual void update(float dt, Engine & engine);

	private:

		CppScriptSystem() = default;
	};
}

#endif