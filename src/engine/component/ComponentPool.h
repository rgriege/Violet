#ifndef VIOLET_ComponentPool_H
#define VIOLET_ComponentPool_H

#include "engine/component/Component.h"
#include "engine/entity/Entity.h"

#include <unordered_map>
#include <vector>

namespace Violet
{
	class VIOLET_API ComponentPool
	{
	public:

		template <typename ComponentType>
		class Iterator
		{
		public:

			Iterator(ComponentType * ptr);

			Iterator<ComponentType> & operator++();
			Iterator<ComponentType> & advanceTo(Entity entity, const Iterator<ComponentType> & end);

			ComponentType & operator*();
			ComponentType * operator->();

			bool operator!=(const Iterator<ComponentType> & other) const;

		private:

			ComponentType * m_ptr;
		};

	public:

		template <typename ComponentType>
		static ComponentPool create();

	public:

		~ComponentPool();

		ComponentPool(ComponentPool && other);
		ComponentPool & operator=(ComponentPool && other);

		Tag getTypeId() const;

		template <typename ComponentType, typename... Args>
		ComponentType & create(Entity entity, Args&&... args);

		bool has(Entity entity) const;

		template <typename ComponentType>
		ComponentType * get(Entity entity);

		template <typename ComponentType>
		Iterator<ComponentType> begin();
		template <typename ComponentType>
		Iterator<ComponentType> end();

		bool remove(Entity entity);
		void clear();

	private:

		ComponentPool(Tag typeId, uint32 componentSize);

		ComponentPool(const ComponentPool &) = delete;
		ComponentPool & operator=(const ComponentPool &) = delete;

		template <typename ComponentType>
		ComponentType * get(uint32 index);

	private:

		Tag m_typeId;
		uint32 m_componentSize;
		std::vector<ubyte> m_data;
		std::unordered_map<Entity, uint32> m_lookupMap;
	};

	template <typename T> using ComponentPoolIterator = typename ComponentPool::Iterator<T>;
}

#include "engine/component/ComponentPool.inl"

#endif