#ifndef VIOLET_ScriptUtilities_H
#define VIOLET_ScriptUtilities_H

#include "violet/entity/Entity.h"
#include "violet/script/ScriptComponent.h"

namespace Violet
{
	class ScriptUtilities
	{
	public:

		template <typename Method, typename ... Args>
		static typename Method::Delegate::result_type runOnAncestor(const Entity & entity, Args && ... args)
		{
			auto parent = entity.getParent();
			while (parent != nullptr && !parent->hasComponent<ScriptComponent>())
				parent = entity.getParent();

			return parent != nullptr ? Method::run(*parent->getComponent<ScriptComponent>()->m_script, std::forward<Args>(args)...) : typename Method::Delegate::result_type();
		}
	};
}

#endif
