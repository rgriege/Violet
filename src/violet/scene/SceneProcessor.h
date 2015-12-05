#ifndef VIOLET_SceneUtilities_H
#define VIOLET_SceneUtilities_H

#include "violet/entity/Entity.h"
#include "violet/scene/Scene.h"

#include <stack>

namespace Violet
{
	class VIOLET_API SceneProcessor
	{
	public:

		class VIOLET_API Filter
		{
		public:

			template <typename ... ComponentTypes>
			static Filter create()
			{
				return Filter(Component::gatherFlags<ComponentTypes...>());
			}

		public:

			static const Filter None;

		public:

			Filter(uint32 componentFlags);

			bool qualifies(const Entity & entity) const;
			bool operator==(const Filter & rhs) const;

		private:

			uint32 m_componentFlags;
		};

		typedef std::function<void(const Entity &, float)> Delegate;

	public:

		~SceneProcessor();

		void addDelegate(Filter filter, const Delegate & delegate);
		void removeDelegate(Filter filter, const Delegate & delegate);

		void process(const Scene & scene, float dt);

	private:

		std::vector<std::pair<Filter, Delegate>> m_delegates;
	};
}

#endif