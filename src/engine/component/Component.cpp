// ============================================================================

#include "engine/component/Component.h"

using namespace Violet;

#ifdef _DEBUG
#include <iostream>
#endif

// ============================================================================

uint32 Component::getNextFlag()
{
	static uint32 s_nextFlag = 1;
	const uint32 result = s_nextFlag;
	s_nextFlag <<= 1;
#ifdef _DEBUG
	if (s_nextFlag == 0)
		std::cout << "Exceeded component flag size" << std::endl;
#endif
	return result;
}

// ============================================================================

Component::~Component()
{
}

// ============================================================================

Component::Component(const Entity & owner) :
	m_owner(owner)
{
}

// ----------------------------------------------------------------------------

Component::Component(Component && other) :
	m_owner(other.m_owner)
{
}

// ----------------------------------------------------------------------------

//Component & Component::operator=(Component && other)
//{
//	m_owner = other.m_owner;
//	return *this;
//}

// ----------------------------------------------------------------------------

const Entity & Component::getOwner() const
{
	return m_owner;
}

// ============================================================================
