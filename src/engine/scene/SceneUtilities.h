#ifndef VIOLET_SceneUtilities_H
#define VIOLET_SceneUtilities_H

#include "engine/entity/Entity.h"
#include "engine/scene/Scene.h"

#include <stack>

namespace Violet
{
	class SceneUtilities
	{
	public:

		class Processor
		{
		public:

			class Filter
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

			private:

				const uint32 m_componentFlags;
			};

			typedef std::function<void(const Entity &, float)> Delegate;

			class SiblingIterator
			{
			public:

				SiblingIterator(const std::vector<unique_val<Entity>> & siblings);

				const Entity & operator*() const;
				SiblingIterator & operator++();
				bool finished() const;

			private:

				std::vector<unique_val<Entity>>::const_iterator m_pos;
				std::vector<unique_val<Entity>>::const_iterator m_end;
			};

		public:

			void addDelegate(Filter filter, Delegate delegate);

			void process(const Scene & scene, float dt);

		private:

			std::vector<std::pair<Filter, Delegate>> m_delegates;
		};
	};
}

#endif