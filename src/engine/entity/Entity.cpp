// ============================================================================

#include "engine/component/Component.h"
#include "engine/entity/Entity.h"
#include "engine/scene/Scene.h"
#include "engine/serialization/Deserializer.h"

using namespace Violet;

// ============================================================================

Factory<std::string, void(Entity &, Deserializer &)> Entity::ms_componentFactory;

// ============================================================================

Entity::Entity(Scene & scene) :
	m_components(),
	m_children(),
	m_componentFlags(),
	m_scene(scene),
	m_handle(scene.createHandle()),
	m_parent()
{
	scene.index(*this);
}

// ----------------------------------------------------------------------------


Entity::Entity(Scene & scene, Deserializer & deserializer) :
	m_components(),
	m_children(),
	m_componentFlags(0),
	m_scene(scene),
	m_handle(),
	m_parent()
{
	auto entitySegment = deserializer.enterSegment("ntty");
	const bool referenced = entitySegment->getBoolean("ref");
	if (referenced)
		m_handle = scene.createHandle(Handle(entitySegment->getUint("id"), 0));
	else
		m_handle = scene.createHandle();
	scene.index(*this);
	auto componentsSegment = entitySegment->enterSegment("cpnt");
	while (*componentsSegment)
	{
		const char * const label = componentsSegment->nextLabel();
		auto componentSegment = componentsSegment->enterSegment(label);
		ms_componentFactory.create(label, *this, *componentSegment);
	}

	auto childrenSegment = entitySegment->enterSegment("chld");
	while (*childrenSegment)
		addChild(make_unique_val<Entity>(scene, *childrenSegment));
}

// ----------------------------------------------------------------------------

Entity::~Entity()
{
	m_components.clear();
	m_scene.deindex(m_handle);
}

// ----------------------------------------------------------------------------

Handle Entity::getHandle() const
{
	return m_handle;
}

// ----------------------------------------------------------------------------

Entity & Entity::addChild(unique_val<Entity> && child)
{
	m_children.emplace_back(std::move(child));
	Entity & newChild = *m_children.back();
	newChild.m_parent = this;
	return newChild;
}

// ----------------------------------------------------------------------------

Entity & Entity::addChild(Deserializer & deserializer)
{
	return addChild(make_unique_val<Entity>(m_scene, deserializer));
}

// ----------------------------------------------------------------------------

std::vector<unique_val<Entity>> & Entity::getChildren()
{
	return m_children;
}

// ----------------------------------------------------------------------------

const std::vector<unique_val<Entity>> & Entity::getChildren() const
{
	return m_children;
}

// ----------------------------------------------------------------------------

lent_ptr<Entity> Entity::getChild(const Handle handle)
{
	const auto it = std::find_if(m_children.begin(), m_children.end(), [=](const unique_val<Entity> & child) { return child->getHandle() == handle; });
	return it != m_children.end() ? it->ptr() : nullptr;
}

// ----------------------------------------------------------------------------

lent_ptr<const Entity> Entity::getChild(const Handle handle) const
{
	const auto it = std::find_if(m_children.begin(), m_children.end(), [=](const unique_val<Entity> & child) { return child->getHandle() == handle; });
	return it != m_children.end() ? it->ptr() : nullptr;
}

// ----------------------------------------------------------------------------

bool Entity::removeChild(const Handle handle)
{
	const auto it = std::find_if(m_children.begin(), m_children.end(), [=](const unique_val<Entity> & child) { return child->getHandle() == handle; });
	const bool found = it != m_children.end();
	if (found)
		m_children.erase(it);
	return found;
}

// ----------------------------------------------------------------------------

uint32 Entity::getComponentFlags() const
{
	return m_componentFlags;
}

// ----------------------------------------------------------------------------

lent_ptr<Entity> Entity::getParent()
{
	return m_parent;
}

// ============================================================================
