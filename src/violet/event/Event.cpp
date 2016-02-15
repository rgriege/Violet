// ============================================================================

#include <algorithm>

#include "violet/event/event.h"
#include "violet/utility/memory.h"

using namespace vlt;

// ============================================================================

struct subscribe_task_data
{
	event_context * context;
	u32 identifier;
	delegate_store func;
};

static void subscribe_task(void * mem)
{
	auto data = make_unique<subscribe_task_data>(mem);
	data->context->subscribe(data->identifier, data->func);
}

void event_base::subscribe(const event_context & context, const u32 identifier, const delegate_store & func)
{
	add_task(subscribe_task, new subscribe_task_data{ const_cast<event_context*>(&context), identifier, func }, 0, task_type::write);
}

// ----------------------------------------------------------------------------

struct unsubscribe_task_data
{
	event_context * context;
	u32 identifier;
	delegate_store func;
};

static void unsubscribe_task(void * mem)
{
	auto data = make_unique<unsubscribe_task_data>(mem);
	data->context->unsubscribe(data->identifier, data->func);
}

void event_base::unsubscribe(const event_context & context, const u32 identifier, const delegate_store & func)
{
	add_task(unsubscribe_task, new unsubscribe_task_data{ const_cast<event_context*>(&context), identifier, func }, 0, task_type::write);
}

// ============================================================================
