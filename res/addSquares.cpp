#include "violet/component/scene.h"
#include "violet/core/engine.h"
#include "violet/graphics/component/color_component.h"
#include "violet/input/system/input_system.h"
#include "violet/log/log.h"
#include "violet/script/script_component.h"
#include "violet/script/cpp/cpp_script.h"
#include "violet/serialization/file/file_deserializer_factory.h"
#include "violet/serialization/file/file_serializer_factory.h"
#include "violet/transform/component/world_transform_component.h"
#include "violet/update/component/update_component.h"
#include "violet/update/system/update_system.h"
#include "violet/utility/formatted_string.h"

using namespace vlt;

void add_square_task(void*)
{
	engine::instance().get_current_scene().load("demoSquare.json");
}

struct instance final : public cpp_script::instance
{
    instance(cpp_script & script) :
        cpp_script::instance(script),
        m_count(0)
    {
		UpdateMethod::assign(script, UpdateMethod::Handler::bind<instance, &instance::onUpdate>(this));
		KeyUpMethod::assign(script, KeyUpMethod::Handler::bind<instance, &instance::onKeyUp>(this));
    }

    virtual ~instance() override
    {
        UpdateMethod::remove(script);
        KeyUpMethod::remove(script);
    }

    void onUpdate(const handle entityId, const float dt)
    {
        if (!m_paused)
        {
            const u32 delta = 10;
            m_count += delta;
            log(formatted_string<32>().sprintf("square count: %d", m_count));
            for (u32 i = 0; i < delta; ++i)
				add_task(add_square_task, nullptr, 0, task_type::write);
        }
    }

    void onKeyUp(const handle entityId, const window_system::key_event & event)
    {
        if (event.code == 'p')
            m_paused = !m_paused;
        else if (event.code == 's' && m_paused)
			engine::instance().get_current_scene().save("save.json");
    }

    u32 m_count;
    bool m_paused = false;
};

VIOLET_SCRIPT_EXPORT void init(cpp_script & script, std::unique_ptr<cpp_script::instance> & _instance)
{
    _instance = std::make_unique<instance>(script);
}

