#ifndef VIOLET_Entity_H
#define VIOLET_Entity_H

#include "violet/Defines.h"
#include "violet/component/Component.h"
#include "violet/utility/lent_ptr.h"

#include <vector>

namespace Violet
{
	class VIOLET_API Entity
	{
	public:

		Entity();
		~Entity();
		
		template <typename ComponentType>
		void addComponent(unique_val<ComponentType> && component);
		template <typename ComponentType, typename ... Args>
		void addComponent(Args && ... args);
		template <typename ComponentType>
		bool hasComponent() const;
		template <typename ... ComponentTypes>
		bool hasComponents() const;
		template <typename ComponentType>
		lent_ptr<ComponentType> getComponent();
		template <typename ComponentType>
		lent_ptr<const ComponentType> getComponent() const;
		uint32 getComponentFlags() const;
		template <typename ComponentType>
		bool removeComponent();

	private:

		Entity(const Entity &) = delete;
		Entity & operator=(const Entity &) = delete;

	private:

		std::vector<const Component *> m_components;
		uint32 m_componentFlags;
	};
}

#include "violet/entity/Entity.inl"

#endif
