#ifndef VIOLET_COMPONENT_POOL_H
#define VIOLET_COMPONENT_POOL_H

#include <memory>
#include <vector>

#include "violet/component/component_metadata.h"
#include "violet/core/handle.h"

namespace vlt
{
	struct Component_Deserializer;
	struct Serializer;

	struct VIOLET_API Component_Pool
	{
		const Component_Metadata * metadata;
		struct Func_Table;
		std::unique_ptr<Func_Table> ftable;
		std::vector<ubyte> components;
		std::vector<Handle> ids;

		template <typename ComponentType, bool IsConst>
		struct Iterator
		{
			typedef std::conditional_t<IsConst, const ComponentType *, ComponentType *> pointer;
			typedef std::conditional_t<IsConst, const ComponentType &, ComponentType &> reference;

		private:

			pointer component_ptr;
			const Handle * id_ptr;

		public:

			Iterator(pointer ptr, const Handle * idPtr);

			Iterator<ComponentType, IsConst> & operator++();
			Iterator<ComponentType, IsConst> & advance_to(Handle entity_id);

			reference operator*();
			pointer operator->();
			Handle get_entity_id() const;

			operator bool() const;
			bool operator!=(const Iterator<ComponentType, IsConst> & other) const;
		};

		template <typename T> using iterator = Iterator<T, false>;
		template <typename T> using const_iterator = Iterator<T, true>;

		struct VIOLET_API Func_Table
		{
			typedef void(*load_fn)(Component_Pool &, Component_Deserializer &);
			typedef void(*save_fn)(Serializer &, const void *);
			typedef void(*destroy_fn)(void*);
			Func_Table(load_fn load, save_fn save, destroy_fn destroy);

			load_fn load;
			save_fn save;
			destroy_fn destroy;
		};

		template <typename ComponentType>
		static Component_Pool create();
		
		Component_Pool(Component_Pool && other);
		Component_Pool & operator=(Component_Pool && other);
		~Component_Pool();

		void load(Component_Deserializer & Deserializer);
		void save(Serializer & serailizer) const;
		u32 save(Serializer & serailizer, const std::vector<Handle> & entity_ids) const;

		template <typename ComponentType, typename... Args>
		ComponentType & create(Handle entity_id, Args && ... args);

		bool has(Handle entity_id) const;

		template <typename ComponentType>
		ComponentType * get(Handle entity_id);
		template <typename ComponentType>
		const ComponentType * get(Handle entity_id) const;

		template <typename ComponentType>
		iterator<ComponentType> begin();
		template <typename ComponentType>
		const_iterator<ComponentType> begin() const;
		template <typename ComponentType>
		iterator<ComponentType> end();
		template <typename ComponentType>
		const_iterator<ComponentType> end() const;

		u32 size() const;

		bool remove(Handle entity_id);
		void clear();

	private:

		Component_Pool(const Component_Metadata * metadata, std::unique_ptr<Func_Table> && ftable);

		Component_Pool(const Component_Pool &) = delete;
		Component_Pool & operator=(const Component_Pool &) = delete;

		u32 get_index(Handle entity_id) const;

		void * get(u32 index);
		const void * get(u32 index) const;

		std::pair<void *, bool> insert(Handle entity_id);
	};
}

#include "violet/component/component_pool.inl"

#endif
