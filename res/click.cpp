#include "violet/core/entity/Entity.h"

#include <iostream>

using namespace Violet;

extern "C" __declspec(dllexport) void onMouse(const Entity & e)
{
    std::cout << "entity " << e.m_id << " clicked!" << std::endl;
}
