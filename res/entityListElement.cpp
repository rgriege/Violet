#include "editor/EditorSystem.h"
#include "violet/Engine.h"
#include "violet/graphics/component/TextComponent.h"
#include "violet/input/system/InputSystem.h"
#include "violet/script/cpp/CppScript.h"
#include "violet/ui/UiList.h"
#include "violet/utility/FormattedString.h"

using namespace Violet;

struct Mem : public CppScript::Memory
{
    Mem(const char * elementFile, uint32 elementHeight) :
        list(elementFile, elementHeight) {}
    UiList list;
};

void onAssignIndex(const Entity & entity, const Engine & engine, uint32 index);
InputResult onMouseDown(const Entity & entity, const Engine & engine, const InputSystem::MouseButtonEvent & event);

VIOLET_SCRIPT_EXPORT void init(CppScript & script, std::unique_ptr<CppScript::Memory> & mem)
{
    AssignIndexMethod::assign(script, onAssignIndex);
    MouseDownMethod::assign(script, onMouseDown);
}

VIOLET_SCRIPT_EXPORT void clean(CppScript & script, std::unique_ptr<CppScript::Memory> & mem)
{
    AssignIndexMethod::remove(script);
    MouseDownMethod::remove(script);
}

void onAssignIndex(const Entity & entity, const Engine & engine, const uint32 index)
{
    engine.addWriteTask(*entity.getComponent<TextComponent>(),
        [=](TextComponent & tc) { tc.m_text = "Entity"; });
}

InputResult onMouseDown(const Entity & entity, const Engine & engine, const InputSystem::MouseButtonEvent & event)
{
    return InputResult::Block;
}
