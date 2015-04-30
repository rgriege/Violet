#include "violet/core/Engine.h"
#include "violet/core/script/component/CppScriptComponent.h"
#include "violet/core/serialization/FileDeserializerFactory.h"
#include "violet/core/window/MouseButton.h"
#include "violet/plugins/input/InputResult.h"

using namespace Violet;

struct Mem
{
    bool hasMenu;
};

extern "C" __declspec(dllexport) void init(CppScriptComponent::Allocator & allocator)
{
    Mem * mem = allocator.allocate<Mem>();
    mem->hasMenu = false;
}

extern "C" __declspec(dllexport) InputResult onMouseDown(const Entity & e, Engine & engine, const MouseButton button, Mem * mem)
{
    if (button == MB_Right)
    {
        if (mem->hasMenu)
        {
            // move it
        }
        else
        {
            auto deserializer = FileDeserializerFactory::getInstance().create("block.json");
            if (deserializer != nullptr)
            {
                engine.getEntityFactory().create("ntty", *deserializer, engine);
                mem->hasMenu = true;
            }
        }

        return InputResult::Block;
    }

    return InputResult::Pass;
}

