#ifndef EDITOR_Editor_H
#define EDITOR_Editor_H

#include "editor/Defines.h"
#include "violet/component/ComponentManager.h"

namespace edt
{
	class Command;

	class EDITOR_API Editor
	{
	public:

		static const Violet::ComponentManager::TagMap ms_tagMap;

	public:

		static void execute(std::unique_ptr<Command> && command);
		static void undo();

		static void addEditBehavior(const Violet::ComponentManager & scene, const Violet::EntityId entityId);
		static void removeEditBehavior(const Violet::ComponentManager & scene, const Violet::EntityId entityId);
	};
}

#endif