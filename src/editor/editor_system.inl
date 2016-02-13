// ============================================================================

#include "editor/component/editor_component.h"
#include "violet/core/engine.h"
#include "violet/component/scene.h"

#include "violet/log/Log.h"
#include "violet/utility/formatted_string.h"

// ============================================================================

template <typename T>
void edt::editor_system::register_command()
{
	register_command(T::get_usage(), &T::parse);
}

// ============================================================================

template <typename ComponentType, typename MemberType, MemberType ComponentType::*Member>
void edt::editor_system::propagate_change(const vlt::handle entity_id, const MemberType & member) thread_const
{
	const auto & engine = vlt::engine::instance();
	auto copyableMember = to_copyable(member);
	engine.add_write_task(engine.get_current_scene(),
		[entity_id, copyableMember](vlt::scene & scene)
		{
			vlt::handle copyId;
			for (const auto & entity : scene.get_entity_view<edt::editor_component>())
				if (entity.get<edt::editor_component>().edit_id == entity_id)
					copyId = entity.id;
			vlt::log(vlt::formatted_string<128>().sprintf("propogate %s %d -> %d", ComponentType::get_tag_static().as_string().c_str(), entity_id.id, copyId.id));
			if (copyId.is_valid())
				scene.get_component<ComponentType>(copyId)->*Member = static_cast<MemberType>(copyableMember);
		}, ComponentType::get_thread_static());
}

// ============================================================================
