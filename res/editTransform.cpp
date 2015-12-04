#include "violet/Engine.h"
#include "violet/component/MappedDataComponent.h"
#include "violet/input/Key.h"
#include "violet/input/component/KeyInputComponent.h"
#include "violet/input/system/InputSystem.h"
#include "violet/graphics/component/TextComponent.h"
#include "violet/script/ScriptComponent.h"
#include "violet/script/cpp/CppScript.h"
#include "violet/transform/component/TransformComponent.h"
#include "violet/utility/FormattedString.h"

#include "editor/EditorSystem.h"

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>

using namespace Violet;
using namespace std::placeholders;

class Instance : public CppScript::Instance
{
public:

    Instance(CppScript & script) :
        CppScript::Instance(script),
        m_entity(nullptr),
        m_selectedEntity(nullptr),
        m_entitySelectedDelegateId(std::numeric_limits<uint32>::max())
    {
        BindToComponentMethod::assign(script, std::bind(&Instance::onBindToComponent, this, _1));
        KeyUpMethod::assign(script, std::bind(&Instance::onKeyUp, this, _1, _2, _3));
        UnbindFromComponentMethod::assign(script, std::bind(&Instance::onUnbindFromComponent, this, _1));
    }

    virtual ~Instance() override
    {
        MouseDownMethod::remove(m_script);
        BindToComponentMethod::remove(m_script);
        KeyUpMethod::remove(m_script);
        UnbindFromComponentMethod::remove(m_script);
    }

private:

    InputResult onMouseDown(const Entity & entity, const Engine & engine, const InputSystem::MouseButtonEvent & event)
    {
        std::cout << "md" << std::endl;
        if (!entity.hasComponent<KeyInputComponent>())
        {
            std::cout << "adding key input" << std::endl;
            engine.addWriteTask(entity, [](Entity & entity)
                {
                    entity.addComponent<KeyInputComponent>();
                });
        }

        return InputResult::Block;
    }

    void onKeyUp(const Entity & entity, const Engine & engine, const unsigned char key)
    {
        if (Key::isNumericInput(key))
        {
            const auto & tc = entity.getComponent<TextComponent>();
            if (tc != nullptr)
            {
                engine.addWriteTask(*tc, [key](TextComponent & tc)
                    {
                        tc.m_text += key;
                    });
            }
        }
        else if (key == Key::Backspace)
        {
            const auto & tc = entity.getComponent<TextComponent>();
            if (tc != nullptr && !tc->m_text.empty())
            {
                engine.addWriteTask(*tc, [](TextComponent & tc)
                    {
                        tc.m_text.pop_back();
                    });
            }
        }
        else if (key == Key::Return)
        {
            engine.addWriteTask(entity, [](Entity & entity)
                {
                    entity.removeComponent<KeyInputComponent>();
                });

            if (m_selectedEntity != nullptr)
            {
                const auto & transformC = m_selectedEntity->getComponent<TransformComponent>();
                const auto & textC = entity.getComponent<TextComponent>();
                if (transformC != nullptr && textC != nullptr)
                {
                    const std::pair<uint32, uint32> & index = getIndex();
                    const float value = std::atof(textC->m_text.c_str());
                    engine.addWriteTask(*transformC, [=](TransformComponent & transformC)
                        {
                            transformC.m_transform[index.first][index.second] = value;
                        });
                }
            }

        }
        else
            printf("%d\n", key);
    }

    void onBindToComponent(Entity & entity)
    {
        Engine::getInstance().addWriteTask(entity.getScene().getEventContext(),
            [=](EventContext & context)
            {
                m_entitySelectedDelegateId = EntitySelectedEvent::subscribe(context, std::bind(&Instance::onEntitySelected, this, _1, _2));
            });
        m_entity = &entity;
    }

    void onUnbindFromComponent(Entity & entity)
    {
        EntitySelectedEvent::unsubscribe(entity.getScene(), m_entitySelectedDelegateId);
        m_entity = nullptr;
    }

    void onEntitySelected(const Entity & entity, const Engine & engine)
    {
        m_selectedEntity = &entity;
        if (m_entity != nullptr)
        {
            MouseDownMethod::assign(m_script, std::bind(&Instance::onMouseDown, this, _1, _2, _3));
            const auto & transformC = entity.getComponent<TransformComponent>();
            const auto & textC = m_entity->getComponent<TextComponent>();
            if (transformC != nullptr && textC != nullptr)
            {
                const std::pair<uint32, uint32> & index = getIndex();
                const float value = transformC->m_transform[index.first][index.second];
                engine.addWriteTask(*textC, [value](TextComponent & textC)
                    {
                        textC.m_text = FormattedString<16>().sprintf("%.3f", value);
                    });
            }
        }
    }

    std::pair<uint32, uint32> getIndex()
    {
        if (m_entity != nullptr)
        {
            const auto & mdc = m_entity->getComponent<MappedDataComponent>();
            if (mdc != nullptr)
                return std::make_pair(std::atoi(mdc->m_data.find("row")->second.c_str()), std::atoi(mdc->m_data.find("column")->second.c_str()));
        }
        return std::make_pair(0, 0);
    }

private:

    const Entity * m_entity;
    const Entity * m_selectedEntity;
    uint32 m_entitySelectedDelegateId;
};

VIOLET_SCRIPT_EXPORT void init(CppScript & script, std::unique_ptr<CppScript::Instance> & instance)
{
    instance = std::make_unique<Instance>(script);
}


