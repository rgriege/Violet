#ifndef EDITOR_EDITOR_SYSTEM_H
#define EDITOR_EDITOR_SYSTEM_H

#include <deque>
#include <memory>
#include <set>
#include <string>

#include "editor/defines.h"
#include "violet/component/scene.h"
#include "violet/event/event.h"
#include "violet/system/system.h"
#include "violet/utility/factory.h"

namespace vlt
{
	struct System_Factory;
}

namespace edt
{
	struct Command;

	struct EDITOR_API Editor_System final : public vlt::System
	{
		typedef vlt::Factory<std::string, std::unique_ptr<Command>(std::string)> CommandFactory;

	public:

		static const char * get_label_static();
		static void install(vlt::System_Factory & Factory);
		static void init(vlt::Deserializer & deserializer);

		template <typename T>
		static void register_command();
		static void register_command(const char * usage, const CommandFactory::Producer & producer);

	public:

		Editor_System(std::string editScriptFileName);
		Editor_System(const Editor_System &) = delete;
		Editor_System & operator=(const Editor_System &) = delete;

		vlt::Scene & get_scene();
		const vlt::Scene & get_scene() const;
		vlt::Handle get_proxy_id(vlt::Handle proxied_id) const;

		b8 execute(const std::string & Command);
		void execute(std::unique_ptr<Command> && command);
		void undo();

		bool select(vlt::Handle proxy_id);
		bool selected(vlt::Handle proxy_id) const;
		const std::set<vlt::Handle> & get_selected_proxies() const;
		bool deselect(vlt::Handle proxy_id);

		void propagate_add(vlt::Handle entity_id) const;

	private:

		std::unique_ptr<vlt::Scene> m_scene;
		std::string m_editScriptFileName;
		std::deque<std::unique_ptr<Command>> m_commandHistory;
		std::set<vlt::Handle> selected_proxies;
	};

	DEFINE_EVENT(EntitySelectedEvent, void(vlt::Handle));
	DEFINE_EVENT(EntityDeselectedEvent, void(vlt::Handle));
}

#include "editor/editor_system.inl"

#endif
