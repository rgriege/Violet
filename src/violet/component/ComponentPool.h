#ifndef VIOLET_ComponentPool_H
#define VIOLET_ComponentPool_H

#include "violet/component/Component.h"

#include <map>
#include <vector>
#include <unordered_map>

namespace Violet
{
	class Deserializer;
	class Serializer;

	class VIOLET_API ComponentPool
	{
	public:

		template <typename ComponentType, bool is_const>
		class Iterator
		{
		public:

			typedef std::conditional_t<is_const, const ComponentType *, ComponentType *> Pointer;
			typedef std::conditional_t<is_const, const ComponentType &, ComponentType &> Reference;

		public:

			explicit Iterator(Pointer ptr);

			Iterator<ComponentType, is_const> & operator++();
			Iterator<ComponentType, is_const> & advanceTo(EntityId entityId);

			Reference operator*();
			Pointer operator->();

			operator bool() const;
			bool operator!=(const Iterator<ComponentType, is_const> & other) const;

		private:

			Pointer m_ptr;
		};

		template <typename T> using iterator = typename Iterator<T, false>;
		template <typename T> using const_iterator = typename Iterator<T, true>;

	public:

		template <typename ComponentType>
		static ComponentPool create();

	public:

		~ComponentPool();

		ComponentPool(ComponentPool && other);
		ComponentPool & operator=(ComponentPool && other);

		Tag getComponentTag() const;

		template <typename ComponentType>
		void load(Deserializer & deserializer, const std::unordered_map<uint32, EntityId> & idMap);
		template <typename ComponentType>
		void save(Serializer & serailizer) const;
		template <typename ComponentType>
		uint32 save(Serializer & serailizer, const std::vector<EntityId> & entityIds) const;

		template <typename ComponentType, typename... Args>
		ComponentType & create(EntityId entityId, Args && ... args);

		bool has(EntityId entityId) const;

		template <typename ComponentType>
		ComponentType * get(EntityId entityId);
		template <typename ComponentType>
		const ComponentType * get(EntityId entityId) const;

		template <typename ComponentType>
		iterator<ComponentType> begin();
		template <typename ComponentType>
		const_iterator<ComponentType> begin() const;
		template <typename ComponentType>
		iterator<ComponentType> end();
		template <typename ComponentType>
		const_iterator<ComponentType> end() const;

		uint32 size() const;

		bool remove(EntityId entityId);
		void clear();

	private:

		ComponentPool(Tag componentTag, uint32 componentSize);

		ComponentPool(const ComponentPool &) = delete;
		ComponentPool & operator=(const ComponentPool &) = delete;

		template <typename ComponentType>
		ComponentType * get(uint32 index);
		template <typename ComponentType>
		const ComponentType * get(uint32 index) const;

		uint32 getLastDataIndex() const;

		std::pair<void *, bool> getLocation(EntityId entityId);
		void verify();

	private:

		const Tag m_componentTag;
		const uint32 m_componentSize;
		std::vector<ubyte> m_data;
		std::map<EntityId, uint32> m_lookupMap;
	};
}

#include "violet/component/ComponentPool.inl"

#endif