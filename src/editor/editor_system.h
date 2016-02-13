#ifndef EDITOR_EDITOR_SYSTEM_H
#define EDITOR_EDITOR_SYSTEM_H

#include <deque>
#include <memory>
#include <set>
#include <string>

#include "editor/Defines.h"
#include "violet/component/scene.h"
#include "violet/event/event.h"
#include "violet/system/system.h"
#include "violet/utility/factory.h"

namespace vlt
{
	struct system_factory;
}

namespace edt
{
	struct command;

	struct EDITOR_API editor_system final : public vlt::system
	{
		typedef vlt::factory<std::string, std::unique_ptr<command>(std::string)> CommandFactory;

	public:

		static const char * get_label_static();
		static void install(vlt::system_factory & factory);
		static void init(vlt::deserializer & deserializer);

		template <typename T>
		static void register_command();
		static void register_command(const char * usage, const CommandFactory::producer & producer);

	public:

		vlt::scene & get_scene();
		const vlt::scene & get_scene() const;

		void execute(const std::string & command);
		void execute(std::unique_ptr<command> && command);
		void undo();

		bool select(vlt::handle entity_id);
		bool selected(vlt::handle entity_id) const;
		const std::set<vlt::handle> & get_selected_entities() const;
		bool deselect(vlt::handle entity_id);

		void propagate_add(vlt::handle entity_id) const;
		template <typename ComponentType, typename MemberType, MemberType ComponentType::*Member>
		void propagate_change(vlt::handle entity_id, const MemberType & member) thread_const;
		void propagated_remove(vlt::handle entity_id) const;

	private:

		editor_system(std::string editScriptFileName);

		void addEditBehavior(const vlt::scene & scene, const vlt::handle entity_id) const;

		editor_system(const editor_system &) = delete;
		editor_system & operator=(const editor_system &) = delete;

	private:

		std::unique_ptr<vlt::scene> m_scene;
		std::string m_editScriptFileName;
		std::deque<std::unique_ptr<command>> m_commandHistory;
		std::set<vlt::handle> m_selectedEntities;
	};

	DEFINE_EVENT(EntitySelectedEvent, void(vlt::handle));
	DEFINE_EVENT(EntityDeselectedEvent, void(vlt::handle));
}

#include "editor/editor_system.inl"

#endif
