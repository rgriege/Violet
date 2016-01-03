#ifndef EDITOR_Editor_H
#define EDITOR_Editor_H

#include "editor/Defines.h"
#include "violet/component/ComponentManager.h"
#include "violet/system/System.h"
#include "violet/utility/Factory.h"

#include <deque>
#include <memory>
#include <string>

namespace Violet
{
	class SystemFactory;
}

namespace edt
{
	class Command;

	class EDITOR_API EditorSystem : public Violet::System
	{
	public:

		typedef Violet::Factory<std::string, std::unique_ptr<Command>(std::string)> CommandFactory;

	public:

		static const char * getStaticLabel();
		static void install(Violet::SystemFactory & factory);
		static void init(Violet::Deserializer & deserializer);

		template <typename T>
		static void registerCommand();
		static void registerCommand(const char * usage, const CommandFactory::Producer & producer);

	public:

        Violet::ComponentManager & getScene();
        const Violet::ComponentManager & getScene() const;

		void execute(const std::string & command);
		void execute(std::unique_ptr<Command> && command);
		void undo();

        void propogateAdd(Violet::EntityId entityId) const;
        template <typename ComponentType, typename MemberType, MemberType ComponentType::*Member>
        void propogateChange(Violet::EntityId entityId, const MemberType & member) thread_const;
        void propogateRemove(Violet::EntityId entityId) const;

	private:

		EditorSystem(std::string editScriptFileName);

		void addEditBehavior(const Violet::ComponentManager & scene, const Violet::EntityId entityId) const;

		EditorSystem(const EditorSystem &) = delete;
		EditorSystem & operator=(const EditorSystem &) = delete;

	private:

        std::unique_ptr<Violet::ComponentManager> m_scene;
        std::string m_editScriptFileName;
		std::deque<std::unique_ptr<Command>> m_commandHistory;
	};
}

#include "editor/EditorSystem.inl"

#endif
