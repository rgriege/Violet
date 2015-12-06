#ifndef VIOLET_Scene_H
#define VIOLET_Scene_H

#include "violet/component/ComponentManager.h"
#include "violet/event/EventContextOwner.h"
#include "violet/handle/HandleManager.h"

namespace Violet
{
	class Deserializer;
	class Serializer;

	class VIOLET_API Scene : public EventContextOwner
	{
	public:

		static std::unique_ptr<Scene> create(const char * name);

	public:

		// Scene(Scene && rhs);
		~Scene();

		const ComponentManager & getComponentManager() const { return *m_componentManager; }

		void loadEntity(const char * entityName);
		void save(Serializer & serializer) const;

	private:

		Scene();

		Scene(const Scene &) = delete;
		Scene & operator=(const Scene &) = delete;

	private:

		HandleManager m_handleManager;
		std::unique_ptr<ComponentManager> m_componentManager;
	};
}

#endif