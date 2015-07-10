#ifndef VIOLET_Entity_H
#define VIOLET_Entity_H

#include "engine/Defines.h"
#include "engine/component/Component.h"
#include "engine/utility/handle/Handle.h"

#include <vector>
#include <memory>

namespace Violet
{
	class Scene;

	class VIOLET_API Entity
	{
	public:

		static const Entity ms_invalid;

	public:

		Entity(Handle handle);
		Entity(Entity && other);
		Entity & operator=(Entity && other);
		~Entity();

		Handle getHandle() const;
		bool isValid() const;

		Entity & addChild(Entity && child);
		std::vector<Entity> & getChildren();
		const std::vector<Entity> & getChildren() const;
		Entity & getChild(Handle handle);
		const Entity & getChild(Handle handle) const;
		bool removeChild(Handle handle);

		template <typename ComponentType>
		void addComponent(std::unique_ptr<ComponentType> && component);
		template <typename ComponentType, typename ... Args>
		void addComponent(Args && ... args);
		template <typename ComponentType>
		bool hasComponent() const;
		template <typename ... ComponentTypes>
		bool hasComponents() const;
		template <typename ComponentType>
		const std::unique_ptr<ComponentType> & getComponent();
		template <typename ComponentType>
		const std::unique_ptr<const ComponentType> & getComponent() const;
		uint32 getComponentFlags() const;

		void addToScene(Scene & scene);
		bool inScene() const;
		void removeFromScene();

		Entity * getParent();

	private:

		Entity(const Entity &) = delete;
		Entity & operator=(const Entity &) = delete;

	private:

		Handle m_handle;
		std::vector<std::unique_ptr<Component>> m_components;
		uint32 m_componentFlags;
		std::vector<Entity> m_children;
		Scene * m_scene;
		Entity * m_parent;
	};
}

#include "engine/entity/Entity.inl"

#endif