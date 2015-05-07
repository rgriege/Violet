#ifndef VIOLET_Scene_H
#define VIOLET_Scene_H

#include "engine/component/ComponentManager.h"
#include "engine/entity/EntityManager.h"
#include "engine/utility/Factory.h"

namespace Violet
{
	class Deserializer;

	class VIOLET_API Scene
	{
	public:

		template <typename ComponentType>
		static void installComponent();

		static Scene create(const char * filename);

	public:

		Scene();
		~Scene();

		Scene(Scene && other);
		Scene & operator=(Scene && other);

		Entity createEntity();
		Entity createEntity(Deserializer & deserializer);
		void destroyEntity(Entity entity);
		std::vector<Entity> getEntities() const;

		template <typename ComponentType, typename... Args>
		ComponentType & createComponent(Entity entity, Args&&... args);

		template <typename ComponentType>
		bool hasComponent(Entity entity) const;

		template <typename ComponentType>
		ComponentType * getComponent(Entity entity);
		template <typename... ComponentTypes>
		ComponentManager::View<ComponentTypes...> getView();

		template <typename ComponentType>
		bool removeComponent(Entity entity);

		void clear();

	private:

		static Factory<std::string, void(Scene &, Entity, Deserializer &)> ms_componentFactory;

		template <typename ComponentType>
		static void factoryCreateComponent(Scene & scene, Entity entity, Deserializer & deserializer);

	private:

		Scene(const Scene &) = delete;
		Scene & operator=(const Scene &) = delete;

	private:

		EntityManager m_entityManager;
		ComponentManager m_componentManager;
	};
}

#include "engine/scene/Scene.inl"

#endif