#ifndef VIOLET_ComponentPool_H
#define VIOLET_ComponentPool_H

#include <memory>
#include <vector>

#include "violet/component/component.h"
#include "violet/core/handle.h"

namespace vlt
{
	struct component_deserializer;
	struct serializer;

	struct VIOLET_API component_pool
	{
		const tag component_tag;
		const u32 component_size;
		const u32 component_version;
		struct func_table;
		std::unique_ptr<func_table> ftable;
		std::vector<ubyte> components;
		std::vector<handle> ids;

		template <typename ComponentType, bool IsConst>
		struct Iterator
		{
			typedef std::conditional_t<IsConst, const ComponentType *, ComponentType *> pointer;
			typedef std::conditional_t<IsConst, const ComponentType &, ComponentType &> reference;

		private:

			pointer component_ptr;
			const handle * id_ptr;

		public:

			Iterator(pointer ptr, const handle * idPtr);

			Iterator<ComponentType, IsConst> & operator++();
			Iterator<ComponentType, IsConst> & advance_to(handle entity_id);

			reference operator*();
			pointer operator->();
			handle get_entity_id() const;

			operator bool() const;
			bool operator!=(const Iterator<ComponentType, IsConst> & other) const;
		};

		template <typename T> using iterator = Iterator<T, false>;
		template <typename T> using const_iterator = Iterator<T, true>;

		struct VIOLET_API func_table
		{
			typedef void(*load_fn)(component_pool &, component_deserializer &);
			typedef void(*save_fn)(serializer &, const void *);
			typedef void(*destroy_fn)(void*);
			func_table(load_fn load, save_fn save, destroy_fn destroy);

			load_fn load;
			save_fn save;
			destroy_fn destroy;
		};

		template <typename ComponentType>
		static component_pool create();
		
		component_pool(component_pool && other);
		component_pool & operator=(component_pool && other);
		~component_pool();

		tag get_tag() const;

		void load(component_deserializer & deserializer);
		void save(serializer & serailizer) const;
		u32 save(serializer & serailizer, const std::vector<handle> & entity_ids) const;

		template <typename ComponentType, typename... Args>
		ComponentType & create(handle entity_id, Args && ... args);

		bool has(handle entity_id) const;

		template <typename ComponentType>
		ComponentType * get(handle entity_id);
		template <typename ComponentType>
		const ComponentType * get(handle entity_id) const;

		template <typename ComponentType>
		iterator<ComponentType> begin();
		template <typename ComponentType>
		const_iterator<ComponentType> begin() const;
		template <typename ComponentType>
		iterator<ComponentType> end();
		template <typename ComponentType>
		const_iterator<ComponentType> end() const;

		u32 size() const;

		bool remove(handle entity_id);
		void clear();

	private:

		component_pool(tag tag, u32 component_size, u32 version, std::unique_ptr<func_table> && ftable);

		component_pool(const component_pool &) = delete;
		component_pool & operator=(const component_pool &) = delete;

		u32 get_index(handle entity_id) const;

		component * get(u32 index);
		const component * get(u32 index) const;

		std::pair<void *, bool> insert(handle entity_id);
	};
}

#include "violet/component/component_pool.inl"

#endif
