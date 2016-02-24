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
#include "violet/utility/memory.h"

#include "dialog.h"

#include <cctype>

using namespace vlt;

struct focus_task_data
{
	Handle entity_id;
};

static void focus_task(void * mem)
{
	auto data = make_unique<focus_task_data>(mem);
	Engine::instance().get_system<Input_System>()->focus(data->entity_id);
}

struct Instance final : public Cpp_Script::Instance
{
public:

    Instance(Cpp_Script & script) :
        Cpp_Script::Instance(script),
        m_textInput()
    {
        BindToComponentMethod::assign(script, BindToComponentMethod::Handler::bind<Instance, &Instance::onBindToComponent>(this));
        FocusLostMethod::assign(script, FocusLostMethod::Handler::bind<Instance, &Instance::onFocusLost>(this));
        KeyUpMethod::assign(script, KeyUpMethod::Handler::bind<Instance, &Instance::on_key_up>(this));
    }

    virtual ~Instance() override
    {
        BindToComponentMethod::remove(script);
        FocusLostMethod::remove(script);
        KeyUpMethod::remove(script);
    }

private:

    void onBindToComponent(const Handle entity_id)
    {
		add_task(focus_task, new focus_task_data{ entity_id }, Key_Input_Component::metadata->thread, task_type::write);
    }

    void onFocusLost(const Handle entity_id)
    {
        const auto & engine = Engine::instance();
        DialogClosedEvent::emit(engine, std::string());
    }

    void on_key_up(const Handle entity_id, const Window_System::Key_Event & event)
    {
        if (m_textInput.on_key_up(entity_id, event) == Ui_Text_Input_Box::S_inactive)
        {
            const Engine & engine = Engine::instance();
            const auto * tc = engine.get_current_scene().get_component<Text_Component>(entity_id);
            if (tc != nullptr && !tc->text.empty())
                DialogClosedEvent::emit(engine, tc->text);
        }
    }

private:

	Ui_Text_Input_Box m_textInput;
};

VIOLET_SCRIPT_EXPORT void init(Cpp_Script & script, std::unique_ptr<Cpp_Script::Instance> & i)
{
    i = std::make_unique<Instance>(script);
}


