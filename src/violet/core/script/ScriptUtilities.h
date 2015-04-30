#ifndef SCRIPT_UTILITIES_H
#define SCRIPT_UTILITIES_H

#include "violet/core/script/component/CppScriptComponent.h"

namespace Violet
{
	class ScriptUtilities
	{
	public:

		template <typename ResultType, typename... Args>
		static ResultType run(Engine & engine, const Entity & entity, const char * method, Args&&... args)
		{
			if (engine.has<CppScriptComponent>(entity))
			{
				auto & component = engine.fetch<CppScriptComponent>(entity);
				return component.run<ResultType>(method, std::forward<Args>(args)...);
			}

			return ResultType();
		}
	};
}

#endif