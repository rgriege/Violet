#ifndef VIOLET_Entity_H
#define VIOLET_Entity_H

#include "violet/Defines.h"
#include "violet/component/Component.h"
#include "violet/utility/Factory.h"
#include "violet/utility/unique_val.h"
#include "violet/utility/lent_ptr.h"

#include <algorithm>
#include <memory>
#include <vector>

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

		Scene & getScene();
		const Scene & getScene() const;

		Entity & addChild();
		Entity & addChild(unique_val<Entity> && child);
		Entity & addChild(Deserializer & deserializer);
		std::vector<unique_val<Entity>> & getChildren();
		const std::vector<unique_val<Entity>> & getChildren() const;
		lent_ptr<Entity> getChild(uint32 index);
		lent_ptr<const Entity> getChild(uint32 index) const;
		bool removeChild(uint32 index);

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

		lent_ptr<Entity> getParent();
		lent_ptr<const Entity> getParent() const;

	private:

		static Factory<std::string, void(Entity &, Deserializer &)> ms_componentFactory;

		template <typename ComponentType>
		static void factoryCreateComponent(Entity & entity, Deserializer & deserializer);

	private:

		Entity(const Entity &) = delete;
		Entity & operator=(const Entity &) = delete;

	private:

		std::vector<unique_val<Component>> m_components;
		uint32 m_componentFlags;
		std::vector<unique_val<Entity>> m_children;
		Scene & m_scene;
		Entity * m_parent;
	};
}

#include "violet/entity/Entity.inl"

#endif
