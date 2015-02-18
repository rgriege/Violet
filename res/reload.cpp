#include "violet/core/Engine.h"
#include "violet/core/entity/Entity.h"
#include "violet/core/script/CppScript.h"
#include "violet/core/script/system/ScriptSystem.h"

#include <iostream>

using namespace Violet;

extern "C" __declspec(dllexport) void onKeyDown(const Entity & e, Engine & engine, const unsigned char & key)
{
    if (key == 114)
    {
        auto & scriptComponent = engine.fetch<ScriptSystem>(7);
        scriptComponent.m_script->reload();
    }
}

