#ifndef SCRIPT_SYSTEM_H
#define SCRIPT_SYSTEM_H

#include "violet/core/system/System.h"
#include "violet/core/script/component/ScriptComponent.h"

namespace Violet
{
	class VIOLET_API ScriptSystem : public ComponentSystem<ScriptComponent>
	{
	public:

		static void install(SystemFactory & factory);
		static std::unique_ptr<System> init(Deserializer & deserializer);

	public:

		ScriptSystem(ScriptSystem && other);

		virtual ~ScriptSystem() override = default;
		virtual void update(float dt, Engine & engine);

	private:

		ScriptSystem() = default;
	};
}

#endif