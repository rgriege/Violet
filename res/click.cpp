#include "violet/core/entity/Entity.h"

#include <iostream>

using namespace Violet;

__declspec(dllexport) void main(const Entity & e)
{
    std::cout << "entity " << e.m_id << " clicked!" << std::endl;
}
