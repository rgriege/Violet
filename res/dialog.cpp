#include "violet/core/engine.h"
#include "violet/component/scene.h"
#include "violet/input/key.h"
#include "violet/input/component/key_input_component.h"
#include "violet/input/system/input_system.h"
#include "violet/graphics/component/text_component.h"
#include "violet/script/script_component.h"
#include "violet/script/cpp/cpp_script.h"
#include "violet/ui/ui_text_input_box.h"
#include "violet/utility/formatted_string.h"

#include "dialog.h"

#include <cctype>
#include <functional>

using namespace vlt;
using namespace std::placeholders;

struct instance : public cpp_script::instance
{
public:

    instance(cpp_script & script) :
        cpp_script::instance(script),
        m_textInput()
    {
        BindToComponentMethod::assign(script, BindToComponentMethod::Handler::bind<instance, &instance::onBindToComponent>(this));
        FocusLostMethod::assign(script, FocusLostMethod::Handler::bind<instance, &instance::onFocusLost>(this));
        KeyUpMethod::assign(script, KeyUpMethod::Handler::bind<instance, &instance::on_key_up>(this));
    }

    virtual ~instance() override
    {
        BindToComponentMethod::remove(script);
        FocusLostMethod::remove(script);
        KeyUpMethod::remove(script);
    }

private:

    void onBindToComponent(const handle entity_id)
    {
        const engine & engine = engine::instance();
        engine.add_write_task(*engine.get_system<input_system>(),
            [=](input_system & input)
            {
                input.focus(entity_id);
            });
    }

    void onFocusLost(const handle entity_id)
    {
        const auto & engine = engine::instance();
        DialogClosedEvent::emit(engine, std::string());
    }

    void on_key_up(const handle entity_id, const window_system::key_event & event)
    {
        if (m_textInput.on_key_up(entity_id, event) == ui_text_input_box::S_inactive)
        {
            const engine & engine = engine::instance();
            const auto * tc = engine.get_current_scene().get_component<text_component>(entity_id);
            if (tc != nullptr && !tc->m_text.empty())
                DialogClosedEvent::emit(engine, tc->m_text);
        }
    }

private:

	ui_text_input_box m_textInput;
};

VIOLET_SCRIPT_EXPORT void init(cpp_script & script, std::unique_ptr<cpp_script::instance> & i)
{
    i = std::make_unique<instance>(script);
}


