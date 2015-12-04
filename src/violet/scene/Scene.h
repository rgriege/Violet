#ifndef VIOLET_Scene_H
#define VIOLET_Scene_H

#include "violet/event/EventContextOwner.h"
#include "violet/handle/HandleComponent.h"
#include "violet/handle/HandleManager.h"
#include "violet/utility/lent_ptr.h"

#include <memory>
#include <unordered_map>

namespace Violet
{
	class Deserializer;
	class Serializer;
	class Entity;

	class VIOLET_API Scene : public EventContextOwner
	{
	public:

		static std::unique_ptr<Scene, void(*)(Scene *)> create(const char * filename);
		static void destroy(Scene * scene);

	public:

		Scene();

		const Entity & getRoot() const;
		lent_ptr<const Entity> getEntity(Handle handle) const;

		void index(const HandleComponent & handleComponent) thread_const;
		void deindex(const HandleComponent & handleComponent) thread_const;

		void save(Serializer & serializer) const;

	private:

		~Scene();

		Scene(const Scene &) = delete;
		Scene & operator=(const Scene &) = delete;

	private:

		std::unordered_map<Handle, std::reference_wrapper<const Entity>> m_lookupMap;
		HandleManager m_handleManager;
		std::unique_ptr<Entity> m_root;
	};
}

#endif