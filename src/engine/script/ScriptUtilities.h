#ifndef VIOLET_ScriptUtilities_H
#define VIOLET_ScriptUtilities_H

#include "engine/script/component/CppScriptComponent.h"

namespace Violet
{
	class ScriptUtilities
	{
	public:

		template <typename ResultType, typename... Args>
		static ResultType run(CppScriptComponent & component, const char * method, Args&&... args);

		template <typename ResultType, typename... Args>
		static ResultType run(Engine & engine, const Entity & entity, const char * method, Args&&... args);
	};
}

#include "engine/script/ScriptUtilities.inl"

#endif