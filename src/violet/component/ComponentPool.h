#ifndef VIOLET_ComponentPool_H
#define VIOLET_ComponentPool_H

#include "violet/component/Component.h"
#include "violet/Handle/Handle.h"

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
			Iterator<ComponentType, is_const> & advanceTo(Handle entityId);

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
		void load(Deserializer & deserializer, const std::unordered_map<uint32, Handle> & idMap);
		template <typename ComponentType>
		void save(Serializer & serailizer) const;
		template <typename ComponentType>
		uint32 save(Serializer & serailizer, const std::vector<Handle> & entityIds) const;

		template <typename ComponentType, typename... Args>
		ComponentType & create(Handle entityId, Args && ... args);

		bool has(Handle entityId) const;

		template <typename ComponentType>
		ComponentType * get(Handle entityId);
		template <typename ComponentType>
		const ComponentType * get(Handle entityId) const;

		template <typename ComponentType>
		iterator<ComponentType> begin();
		template <typename ComponentType>
		const_iterator<ComponentType> begin() const;
		template <typename ComponentType>
		iterator<ComponentType> end();
		template <typename ComponentType>
		const_iterator<ComponentType> end() const;

		uint32 size() const;

		bool remove(Handle entityId);
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

		std::pair<void *, bool> getLocation(Handle entityId);
		void verify();

	private:

		const Tag m_componentTag;
		const uint32 m_componentSize;
		std::vector<ubyte> m_data;
		std::map<Handle, uint32> m_lookupMap;
	};
}

#include "violet/component/ComponentPool.inl"

#endif