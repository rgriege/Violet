#ifndef VIOLET_ComponentPool_H
#define VIOLET_ComponentPool_H

#include "violet/component/Component.h"

#include <vector>

namespace Violet
{
	class ComponentDeserializer;
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

			Iterator(Pointer ptr, const EntityId * idPtr);

			Iterator<ComponentType, is_const> & operator++();
			Iterator<ComponentType, is_const> & advanceTo(EntityId entityId);

			Reference operator*();
			Pointer operator->();
			EntityId getEntityId() const;

			operator bool() const;
			bool operator!=(const Iterator<ComponentType, is_const> & other) const;

		private:

			Pointer m_ptr;
			const EntityId * m_idPtr;
		};

		template <typename T> using iterator = Iterator<T, false>;
		template <typename T> using const_iterator = Iterator<T, true>;

	private:

		struct VIOLET_API FuncTable
		{
			typedef void(*Load)(ComponentPool &, ComponentDeserializer &);
			typedef void(*Save)(Serializer &, const void *);
			typedef void(*Destroy)(void*);
			FuncTable(Load load, Save save, Destroy destroy);

			Load load;
			Save save;
			Destroy destroy;
		};

	public:

		template <typename ComponentType>
		static ComponentPool create();

	public:
		
		ComponentPool(ComponentPool && other);
		ComponentPool & operator=(ComponentPool && other);
		~ComponentPool();

		Tag getComponentTag() const;

		void load(ComponentDeserializer & deserializer);
		void save(Serializer & serailizer) const;
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

		ComponentPool(Tag componentTag, uint32 componentSize, uint32 componentVersion, std::unique_ptr<FuncTable> && ftable);

		ComponentPool(const ComponentPool &) = delete;
		ComponentPool & operator=(const ComponentPool &) = delete;

		uint32 getIndex(EntityId entityId) const;

		Component * get(uint32 index);
		const Component * get(uint32 index) const;

		std::pair<void *, bool> insert(EntityId entityId);

	private:

		const Tag m_componentTag;
		const uint32 m_componentSize;
		const uint32 m_componentVersion;
		std::unique_ptr<FuncTable> m_funcTable;
		std::vector<ubyte> m_componentData;
		std::vector<EntityId> m_ids;
	};
}

#include "violet/component/ComponentPool.inl"

#endif
