#ifndef VIOLET_SCENE_H
#define VIOLET_SCENE_H

#include "violet/component/component_pool.h"
#include "violet/handle/versioned_handle_manager.h"
#include "violet/task/thread.h"
#include "violet/utility/factory.h"

#include <iterator>
#include <memory>
#include <tuple>
#include <unordered_map>

namespace vlt
{
	struct component_deserializer;

	struct VIOLET_API scene
	{
		template <typename... ComponentTypes>
		struct view
		{
			typedef std::tuple<const ComponentTypes&...> component_tuple;
			typedef std::tuple<component_pool::const_iterator<ComponentTypes>...> iterator_tuple;

			struct entity
			{
				const component_tuple components;
				handle id;

				entity(component_tuple components, handle id);

				template <typename ComponentType>
				const ComponentType & get() const;
			};

			struct iterator : public std::iterator<std::input_iterator_tag, component_tuple>
			{
				explicit iterator(iterator_tuple iterators);

				iterator & operator++();
				entity operator*();

				bool operator==(const iterator & other) const;
				bool operator!=(const iterator & other) const;

			private:

				void advance();
				template <u32 Index>
				void advance(u32 & count);

			private:

				iterator_tuple m_iterators;
				handle m_entityId;
			};

		private:

			static const size_t N = sizeof...(ComponentTypes);

		public:

			explicit view(const scene & scene);

			iterator begin();
			iterator end();

		private:

			const scene & m_manager;
		};

	public:

		typedef factory<tag, component_pool()> pool_factory;

	public:

		template <typename ComponentType>
		static void install_component();
		static void install_component(tag const tag, const pool_factory::producer & producer, thread thread);

		template <typename ComponentType>
		static void uninstall_component();
		static void uninstall_component(tag const tag);

	public:

		scene();
		scene(scene && other);
		scene & operator=(scene && other);
		~scene();

		std::vector<handle> load(const char * sceneName);
		void save(const char * sceneName) const;
		void save(const char * sceneName, std::vector<handle> entityIds) const;

		handle create_entity();
		template <typename ComponentType, typename... Args>
		ComponentType & create_component(handle entity_id, Args &&... args);

		template <typename ComponentType>
		component_pool * get_pool();
		template <typename ComponentType>
		const component_pool * get_pool() const;

		bool exists(handle entity_id) const;
		template <typename ComponentType>
		bool has_component(handle entity_id) const;

		template <typename ComponentType>
		ComponentType * get_component(handle entity_id);
		template <typename ComponentType>
		const ComponentType * get_component(handle entity_id) const;
		template <typename... ComponentTypes>
		view<ComponentTypes...> get_entity_view() const;
		std::vector<handle> get_entity_ids() const;
		u16 get_entity_version(u16 id) const;
		
		template <typename ComponentType>
		bool remove(handle entity_id);
		void remove_all(handle entity_id);
		void remove_all(handle entity_id) thread_const;
		void clear();

	private:

		template <typename ComponentType>
		static component_pool create_pool();

	private:

		component_pool * get_pool(tag componentTag);
		const component_pool * get_pool(tag componentTag) const;

		scene(const scene &) = delete;
		scene & operator=(const scene &) = delete;

	private:

		static pool_factory ms_poolFactory;
		static std::map<tag, thread> ms_poolThreads;

	private:

		versioned_handle_manager m_handleManager;
		std::vector<component_pool> m_pools;
	};

	template <typename... ComponentTypes> 
	using entity = typename scene::view<ComponentTypes...>::entity;
}

#include "violet/component/scene.inl"

#endif
