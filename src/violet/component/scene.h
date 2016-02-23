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
	struct Component_Deserializer;

	struct VIOLET_API Scene
	{
		Versioned_Handle_Manager handle_manager;
		std::vector<Component_Pool> pools;

		template <typename... ComponentTypes>
		struct View
		{
			typedef std::tuple<const ComponentTypes&...> component_tuple;
			typedef std::tuple<Component_Pool::const_iterator<ComponentTypes>...> iterator_tuple;

			struct Entity
			{
				const component_tuple components;
				Handle id;

				Entity(component_tuple components, Handle id);

				template <typename ComponentType>
				const ComponentType & get() const;
			};

			struct iterator : public std::iterator<std::input_iterator_tag, component_tuple>
			{
				explicit iterator(iterator_tuple iterators);

				iterator & operator++();
				Entity operator*();

				bool operator==(const iterator & other) const;
				bool operator!=(const iterator & other) const;

			private:

				void advance();
				template <u32 Index>
				void advance(u32 & count);

			private:

				iterator_tuple iterators;
				Handle entity_id;
			};

		private:

			static const size_t N = sizeof...(ComponentTypes);

		public:

			explicit View(const Scene & Scene);

			iterator begin();
			iterator end();

		private:

			const Scene & scene;
		};

	public:

		typedef Factory<Tag, Component_Pool()> Pool_Factory;

	public:

		template <typename ComponentType>
		static void install_component();
		static void install_component(Tag const tag, const Pool_Factory::Producer & producer);

		template <typename ComponentType>
		static void uninstall_component();
		static void uninstall_component(Tag const tag);

	public:

		Scene();
		Scene(Scene && other);
		Scene & operator=(Scene && other);
		~Scene();

		std::vector<Handle> load(const char * sceneName);
		void save(const char * sceneName) const;
		void save(const char * sceneName, std::vector<Handle> entityIds) const;

		Handle create_entity();
		template <typename ComponentType, typename... Args>
		ComponentType & create_component(Handle entity_id, Args &&... args);

		template <typename ComponentType>
		Component_Pool * get_pool();
		template <typename ComponentType>
		const Component_Pool * get_pool() const;

		bool exists(Handle entity_id) const;
		template <typename ComponentType>
		bool has_component(Handle entity_id) const;

		template <typename ComponentType>
		ComponentType * get_component(Handle entity_id);
		template <typename ComponentType>
		const ComponentType * get_component(Handle entity_id) const;
		template <typename... ComponentTypes>
		View<ComponentTypes...> get_entity_view() const;
		std::vector<Handle> get_entity_ids() const;
		u16 get_entity_version(u16 id) const;
		
		template <typename ComponentType>
		bool remove(Handle entity_id);
		void remove_all(Handle entity_id);
		void remove_all(Handle entity_id) thread_const;
		void clear();

	private:

		template <typename ComponentType>
		static Component_Pool create_pool();

	private:

		Component_Pool * get_pool(Tag tag);
		const Component_Pool * get_pool(Tag tag) const;

		Scene(const Scene &) = delete;
		Scene & operator=(const Scene &) = delete;
	};

	template <typename... ComponentTypes> 
	using Entity = typename Scene::View<ComponentTypes...>::Entity;
}

#include "violet/component/scene.inl"

#endif
