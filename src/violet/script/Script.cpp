// ============================================================================

#include <assert.h>
#include <mutex>
#include <set>

#include "violet/log/Log.h"
#include "violet/script/script.h"
#include "violet/script/script_component.h"
#include "violet/utility/memory.h"

using namespace vlt;

// ============================================================================

void script::add_hook(script & script, const u32 id, const delegate_store & hook)
{
	script.m_boundMethods.emplace(id, hook);
}

// ----------------------------------------------------------------------------

struct add_hook_task_data
{
	script * script;
	u32 id;
	delegate_store func;
};

static void add_hook_task(void * mem)
{
	auto data = make_unique<add_hook_task_data>(mem);
	script::add_hook(*data->script, data->id, data->func);
}

void script::add_hook(const script & script, u32 id, const delegate_store & func)
{
	add_task(add_hook_task, new add_hook_task_data{ const_cast<vlt::script*>(&script), id, func }, script_component::metadata->thread, task_type::write);
}

// ----------------------------------------------------------------------------

void script::remove_hook(script & script, u32 id, const char * name)
{
	const auto it = script.m_boundMethods.find(id);
	if (it != script.m_boundMethods.end())
		script.m_boundMethods.erase(it);
	else
		script.warn(name, "failed to remove - method not bound");
}

// ----------------------------------------------------------------------------

struct remove_hook_task_data
{
	script * script;
	u32 id;
	const char * name;
};

static void remove_hook_task(void * mem)
{
	auto data = make_unique<remove_hook_task_data>(mem);
	script::remove_hook(*data->script, data->id, data->name);
}

void script::remove_hook(const script & script, u32 id, const char * name)
{
	add_task(remove_hook_task, new remove_hook_task_data{ const_cast<vlt::script*>(&script), id, name }, script_component::metadata->thread, task_type::write);
}

// ============================================================================

script::~script()
{
	assert(m_boundMethods.empty());
}

// ============================================================================

void script::warn(const char * const procedureName, const char * const context) const
{
	static std::set<std::string> s_warnings;
	static std::mutex s_mutex;

	const std::lock_guard<std::mutex> guard(s_mutex);
	std::string warning = get_filename() + "(" + procedureName + "): " + context;
	auto it = s_warnings.find(warning);
	if (it == s_warnings.end())
	{
		log(warning.c_str());
		s_warnings.insert(warning);
	}
}

// ============================================================================
