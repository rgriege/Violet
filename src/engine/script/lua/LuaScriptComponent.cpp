// ============================================================================

#include "engine/script/lua/LuaScriptComponent.h"

#include "engine/entity/Entity.h"
#include "engine/serialization/Deserializer.h"

using namespace Violet;

// ============================================================================

Tag LuaScriptComponent::getTag()
{
	return Tag('l', 'u', 'a', 's');
}

// ============================================================================

LuaScriptComponent::LuaScriptComponent(const Entity & owner, Deserializer & deserializer) :
	ComponentBase<LuaScriptComponent>(owner),
	m_script(make_shared_val<FileResource>(deserializer.getString("file")))
{
}

// ----------------------------------------------------------------------------

LuaScriptComponent::LuaScriptComponent(LuaScriptComponent && other) :
	ComponentBase<LuaScriptComponent>(std::move(other)),
	m_script(std::move(other.m_script))
{
}

// ============================================================================
