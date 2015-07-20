#ifndef VIOLET_Entity_H
#define VIOLET_Entity_H

#include "engine/Defines.h"
#include "engine/component/Component.h"
#include "engine/utility/Factory.h"
#include "engine/utility/handle/Handle.h"
#include "engine/utility/unique_val.h"
#include "engine/utility/lent_ptr.h"

#include <vector>
#include <memory>

namespace Violet
{
	class Deserializer;
	class Scene;

	class VIOLET_API Entity
	{
	public:

		template <typename ComponentType>
		static void installComponent();

	public:

		Entity(Scene & scene);
		Entity(Scene & scene, Deserializer & deserializer);
		~Entity();

		Handle getHandle() const;

		Entity & addChild(unique_val<Entity> && child);
		Entity & addChild(Deserializer & deserializer);
		std::vector<unique_val<Entity>> & getChildren();
		const std::vector<unique_val<Entity>> & getChildren() const;
		lent_ptr<Entity> getChild(Handle handle);
		lent_ptr<const Entity> getChild(Handle handle) const;
		bool removeChild(Handle handle);

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

		lent_ptr<Entity> getParent();

	private:

		static Factory<std::string, void(Entity &, Deserializer &)> ms_componentFactory;

		template <typename ComponentType>
		static void factoryCreateComponent(Entity & entity, Deserializer & deserializer);

	private:

		Entity(const Entity &) = delete;
		Entity & operator=(const Entity &) = delete;

	private:

		Handle m_handle;
		std::vector<unique_val<Component>> m_components;
		uint32 m_componentFlags;
		std::vector<unique_val<Entity>> m_children;
		Scene & m_scene;
		Entity * m_parent;
	};
}

#include "engine/entity/Entity.inl"

#endif