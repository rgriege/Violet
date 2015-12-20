#ifndef EDITOR_Editor_H
#define EDITOR_Editor_H

#include "editor/Defines.h"
#include "violet/component/ComponentManager.h"
#include "violet/utility/Factory.h"

#include <string>

namespace edt
{
	class Command;

	class EDITOR_API Editor
	{
	public:

		typedef Violet::Factory<std::string, std::unique_ptr<Command>(std::string)> CommandFactory;

		static const Violet::ComponentManager::TagMap ms_tagMap;

	public:

		template <typename T>
		static void registerCommand();
		static void registerCommand(const char * usage, const CommandFactory::Producer & producer);

		static void execute(const std::string & command);
		static void execute(std::unique_ptr<Command> && command);
		static void undo();

		static void addEditBehavior(const Violet::ComponentManager & scene, const Violet::EntityId entityId);
		static void removeEditBehavior(const Violet::ComponentManager & scene, const Violet::EntityId entityId);
	};
}

#include "editor/Editor.inl"

#endif