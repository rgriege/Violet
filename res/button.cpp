#include "editor/EditorSystem.h"
#include "violet/Engine.h"
#include "violet/input/system/InputSystem.h"
#include "violet/graphics/Color.h"
#include "violet/graphics/component/ColorComponent.h"
#include "violet/script/cpp/CppScript.h"
#include "violet/serialization/file/FileSerializerFactory.h"
#include "violet/update/system/UpdateSystem.h"

#include <algorithm>
#include <functional>

using namespace Violet;

const uint8 ms_maxOpacity = 0xCC;
const uint8 ms_minOpacity = 0x33;
const int8 ms_transitionDelta = 5;

class Instance : public CppScript::Instance
{
public:

    Instance(CppScript & script, const uint8 _m_opacity) :
        CppScript::Instance(script),
        m_opacity(_m_opacity),
        m_disappearing(true)
    {
        using namespace std::placeholders;
        MouseInMethod::assign(script, std::bind(&Instance::onMouseIn, this, _1));
        MouseDownMethod::assign(script, std::bind(&Instance::onMouseDown, this, _1, _2));
        MouseOutMethod::assign(script, std::bind(&Instance::onMouseOut, this, _1));
        UpdateMethod::assign(script, std::bind(&Instance::update, this, _1, _2));
    }

    virtual ~Instance() override
    {
        MouseInMethod::remove(m_script);
        MouseDownMethod::remove(m_script);
        MouseOutMethod::remove(m_script);
        UpdateMethod::remove(m_script);
    }

private:

    void onMouseIn(const Entity & entity)
    {
        m_opacity = ms_maxOpacity;
        adjustOpacity(entity, m_opacity);
    }

    InputResult onMouseDown(const Entity & entity, const InputSystem::MouseButtonEvent & event)
    {
        auto const & editor = Engine::getInstance().getSystem<EditorSystem>();
        if (!editor->hasScene())
        {
            Engine::getInstance().addWriteTask(*editor, [&](EditorSystem & editor)
                {
                    editor.loadScene("testScene.json");
                });
        }
        else
            editor->saveScene("testScene2.json");
        return InputResult::Block;
    }

    void onMouseOut(const Entity & entity)
    {
        m_opacity = ms_minOpacity;
        m_disappearing = true;
    }

    void update(const Entity & entity, const float dt)
    {
        if (m_disappearing)
            adjustOpacity(entity, -ms_transitionDelta);
    }

    void adjustOpacity(const Entity & entity, int diff)
    {
        auto colorComponent = entity.getComponent<ColorComponent>();
        if (colorComponent != nullptr)
        {
            if (colorComponent->m_color.a != m_opacity)
            {
                Color newColor(colorComponent->m_color);
                newColor.a = std::min<uint8>(std::max<uint8>(newColor.a.asUint() + diff, ms_minOpacity), ms_maxOpacity);
                Engine::getInstance().addWriteTask(*colorComponent, [newColor](ColorComponent & colorComponent) { colorComponent.m_color = newColor; });

                if (newColor.a.asUint() == m_opacity)
                    m_disappearing = false;
            }
        }
    }

private:

    uint8 m_opacity;
    bool m_disappearing;
};

VIOLET_SCRIPT_EXPORT void init(CppScript & script, std::unique_ptr<CppScript::Instance> & instance)
{
    instance = std::make_unique<Instance>(script, ms_minOpacity);
}

