#ifndef VIOLET_Scene_H
#define VIOLET_Scene_H

#include "engine/utility/handle/HandleManager.h"

#include <memory>
#include <unordered_map>

namespace Violet
{
	class Deserializer;
	class Entity;

	class VIOLET_API Scene
	{
	public:

		static std::unique_ptr<Scene> create(const char * filename);

	public:

		Scene();
		~Scene();

		Scene(Scene && other);

		Entity & getRoot();
		const Entity & getRoot() const;
		Entity * getEntity(Handle handle);
		const Entity * getEntity(Handle handle) const;

		Handle createHandle(Handle desiredHandle = Handle::ms_invalid);
		void index(Entity & entity);
		bool deindex(Handle handle);

	private:

		Scene(const Scene &) = delete;
		Scene & operator=(const Scene &) = delete;

	private:

		std::unordered_map<Handle, std::reference_wrapper<Entity>> m_lookupMap;
		HandleManager m_handleManager;
		std::unique_ptr<Entity> m_root;
	};
}

#endif