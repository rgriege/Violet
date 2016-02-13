// ============================================================================

#include <algorithm>

#include "violet/event/event.h"

using namespace vlt;

// ============================================================================

void event_base::subscribe(const event_context & context, const u32 identifier, const delegate_store & func)
{
	vlt::engine::instance().add_write_task(context,
		[=](event_context & context)
		{
			context.subscribe(identifier, func);
		});
}

// ----------------------------------------------------------------------------

void event_base::unsubscribe(const event_context & context, const u32 identifier, const delegate_store & func)
{
	vlt::engine::instance().add_write_task(context,
		[=](event_context & context)
	{
		context.unsubscribe(identifier, func);
	});
}

// ============================================================================
