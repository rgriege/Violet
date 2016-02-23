// ============================================================================

#include <algorithm>

#include "violet/event/event.h"
#include "violet/utility/memory.h"

using namespace vlt;

// ============================================================================

struct subscribe_task_data
{
	Event_Context * context;
	u32 identifier;
	Delegate_Store func;
};

static void subscribe_task(void * mem)
{
	auto data = make_unique<subscribe_task_data>(mem);
	data->context->subscribe(data->identifier, data->func);
}

void Event_Base::subscribe(const Event_Context & context, const u32 identifier, const Delegate_Store & func)
{
	add_task(subscribe_task, new subscribe_task_data{ const_cast<Event_Context*>(&context), identifier, func }, 0, task_type::write);
}

// ----------------------------------------------------------------------------

struct unsubscribe_task_data
{
	Event_Context * context;
	u32 identifier;
	Delegate_Store func;
};

static void unsubscribe_task(void * mem)
{
	auto data = make_unique<unsubscribe_task_data>(mem);
	data->context->unsubscribe(data->identifier, data->func);
}

void Event_Base::unsubscribe(const Event_Context & context, const u32 identifier, const Delegate_Store & func)
{
	add_task(unsubscribe_task, new unsubscribe_task_data{ const_cast<Event_Context*>(&context), identifier, func }, 0, task_type::write);
}

// ============================================================================
