#ifndef VIOLET_ScriptMethod_H
#define VIOLET_ScriptMethod_H

#include "engine/entity/Entity.h"

#include <functional>
#include <iostream>
#include <unordered_map>

namespace Violet
{
	template <typename Derived, typename Signature>
	class ScriptMethod;

	template <typename Derived, typename ResultType, typename ... Args>
	class ScriptMethod<Derived, ResultType(Args...)>
	{
	public:

		typedef std::function<ResultType(Args...)> Delegate;

	public:

		static void assign(const Entity & entity, Delegate func)
		{
			const auto it = ms_delegates.find(entity.getHandle());
			if (it != ms_delegates.end())
				std::cout << "warning: entity " << entity.getHandle().getId() << " already has delegate" << std::endl;

			ms_delegates.emplace_hint(it, entity.getHandle(), func);
			std::cout << "method bound for " << entity.getHandle().getId() << std::endl;
		}

		static bool has(const Entity & entity)
		{
			return ms_delegates.find(entity.getHandle()) != ms_delegates.end();
		}

		static ResultType run(const Entity & entity, Args && ... args)
		{
			const auto it = ms_delegates.find(entity.getHandle());
			if (it != ms_delegates.end())
				return it->second(std::forward<Args>(args)...);
			else
			{
				// std::cout << "error: entity has no delegate" << std::endl;
				return ResultType();
			}
		}

		static void remove(const Entity & entity)
		{
			if (ms_delegates.erase(entity.getHandle()) == 0)
				std::cout << "no method bound for " << entity.getHandle().getId() << std::endl;
		}

	private:

		static std::unordered_map<Handle, Delegate> ms_delegates;
	};

	template <typename Derived, typename ResultType, typename ... Args>
	std::unordered_map<Handle, typename ScriptMethod<Derived, ResultType(Args...)>::Delegate> ScriptMethod<Derived, ResultType(Args...)>::ms_delegates;

#define DEFINE_METHOD(Method, Signature) class VIOLET_API Method : public ScriptMethod<Method, Signature> \
	{ \
	public: \
		static const char * getName() { return #Method; } \
	}
}

#endif