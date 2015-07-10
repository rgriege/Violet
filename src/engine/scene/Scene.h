#ifndef VIOLET_Scene_H
#define VIOLET_Scene_H

#include "engine/entity/Entity.h"
#include "engine/utility/Factory.h"
#include "engine/utility/handle/HandleManager.h"

#include <unordered_map>

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

		Entity & createEntity();
		Entity & createEntity(Deserializer & deserializer);

		Entity & getRoot();
		const Entity & getRoot() const;
		Entity & getEntity(Handle handle);
		const Entity & getEntity(Handle handle) const;

		bool destroyEntity(Handle handle);

		void index(Entity & entity);
		void reindex(Entity & entity);
		bool deindex(Handle handle);

	private:

		static Factory<std::string, void(Scene &, Entity &, Deserializer &)> ms_componentFactory;

		template <typename ComponentType>
		static void factoryCreateComponent(Scene & scene, Entity & entity, Deserializer & deserializer);

	private:

		Scene(const Scene &) = delete;
		Scene & operator=(const Scene &) = delete;

	private:

		Entity m_root;
		std::unordered_map<Handle, std::reference_wrapper<Entity>> m_lookupMap;
		HandleManager m_handleManager;
	};
}

#include "engine/scene/Scene.inl"

#endif