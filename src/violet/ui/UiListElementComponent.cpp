// ============================================================================

#include "violet/ui/UiListElementComponent.h"

using namespace Violet;

// ============================================================================

Tag UiListElementComponent::getStaticTag()
{
	return Tag('u', 'i', 'l', 'e');
}

// ============================================================================

UiListElementComponent::UiListElementComponent(const Entity & owner, const uint32 index) :
	ComponentBase<UiListElementComponent>(owner),
	m_index(index)
{
}

// ----------------------------------------------------------------------------

UiListElementComponent::UiListElementComponent(UiListElementComponent && other) :
	ComponentBase<UiListElementComponent>(std::move(other)),
	m_index(other.m_index)
{
}

// ============================================================================
