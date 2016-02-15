#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/log/log.h"
#include "violet/script/cpp/cpp_script.h"
#include "violet/transform/component/world_transform_component.h"
#include "violet/update/system/update_system.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/memory.h"
#include "violet/utility/random.h"
#include "violet/window/window_system.h"

using namespace vlt;

struct update_task_data
{
	handle entity_id;
	v2 delta;
};

static void update_task(void * mem)
{
	auto data = make_unique<update_task_data>(mem);
	auto tc = engine::instance().get_current_scene().get_component<world_transform_component>(data->entity_id);
	if (tc != nullptr)
	{
		tc->transform[0][3] += data->delta.x;
		tc->transform[1][3] += data->delta.y;
	}
}

class instance : public cpp_script::instance
{
public:

    instance(cpp_script & script) :
        cpp_script::instance(script),
        m_speed(100.f, 0)
    {
        UpdateMethod::assign(script, UpdateMethod::Handler::bind<instance, &instance::onUpdate>(this));

        m_speed.rotate(random::ms_generator.generate_0_to_1() * 3.141 * 2);
    }

    virtual ~instance() override
    {
        UpdateMethod::remove(script);
    }

private:

    void onUpdate(const handle entityId, const float dt)
    {
        auto transformC = engine::instance().get_current_scene().get_component<world_transform_component>(entityId);
        if (transformC != nullptr)
        {
            const auto & window = engine::instance().get_system<window_system>();
			const v2 delta = m_speed * dt;
			add_task(update_task, new update_task_data{ entityId, delta }, world_transform_component::metadata->thread, task_type::write);
            if (std::abs(transformC->transform[0][3] + delta.x) > window->get_width() / 2)
                m_speed.x = -m_speed.x;
            if (std::abs(transformC->transform[1][3] + delta.y) > window->get_height() / 2)
                m_speed.y = -m_speed.y;
        }
    }

private:

    v2 m_speed;
};

VIOLET_SCRIPT_EXPORT void init(cpp_script & script, std::unique_ptr<cpp_script::instance> & i)
{
    i = std::make_unique<instance>(script);
}

