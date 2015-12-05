#ifndef VIOLET_EditorSystem_H
#define VIOLET_EditorSystem_H

#include "violet/system/System.h"

#include "violet/event/Event.h"
#include "violet/handle/Handle.h"
#include "violet/utility/lent_ptr.h"
#include "editor/EditorConfig.h"

#include <memory>
#include <string>

namespace Violet
{
	class Deserializer;
	class Entity;
	class Scene;
	class SystemFactory;

	class VIOLET_EDITOR_API EditorSystem : public System
	{
	public:

		static const char * getStaticLabel();
		static void install(SystemFactory & factory);
		static void init(Deserializer & deserializer);

	public:

		virtual ~EditorSystem() override = default;

		void loadScene(const char * filename);
		void saveScene(const char * filename) const;

		bool hasScene() const;
		lent_ptr<const Entity> getSceneRoot() const;

	private:

		EditorSystem(std::string editScriptFileName);
		
	private:

		Handle m_rootSceneHandle;
		std::string m_editScriptFileName;
	};

	DEFINE_EVENT(EntitySelectedEvent, void(const Entity & entity));
}

#endif
