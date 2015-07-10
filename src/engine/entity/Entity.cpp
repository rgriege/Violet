// ============================================================================

#include "engine/entity/Entity.h"
#include "engine/component/Component.h"
#include "engine/scene/Scene.h"

using namespace Violet;

// ============================================================================

const Entity Entity::ms_invalid(Handle::ms_invalid);

// ============================================================================

Entity::Entity(Handle handle) :
	m_handle(handle),
	m_components(),
	m_children(),
	m_componentFlags(0),
	m_scene(nullptr),
	m_parent(nullptr)
{
}

// ----------------------------------------------------------------------------

Entity::Entity(Entity && other) :
	m_handle(other.m_handle),
	m_components(std::move(other.m_components)),
	m_children(std::move(other.m_children)),
	m_componentFlags(other.m_componentFlags),
	m_scene(other.m_scene)
{
	other.m_handle = Handle::ms_invalid;
	other.m_componentFlags = 0;

	if (inScene())
	{
		m_scene->reindex(*this);
		other.m_scene = nullptr;
	}
}

// ----------------------------------------------------------------------------

Entity & Entity::operator=(Entity && other)
{
	m_handle = other.m_handle;
	m_components = std::move(other.m_components);
	m_children = std::move(other.m_children);
	m_componentFlags = other.m_componentFlags;
	m_scene = other.m_scene;

	other.m_handle = Handle::ms_invalid;
	other.m_componentFlags = 0;

	if (inScene())
	{
		m_scene->reindex(*this);
		other.m_scene = nullptr;
	}

	return *this;
}

// ----------------------------------------------------------------------------

Entity::~Entity()
{
	if (inScene())
		m_scene->deindex(m_handle);
}

// ----------------------------------------------------------------------------

Handle Entity::getHandle() const
{
	return m_handle;
}

// ----------------------------------------------------------------------------

bool Entity::isValid() const
{
	return m_handle.isValid();
}

// ----------------------------------------------------------------------------

Entity & Entity::addChild(Entity && child)
{
	const bool resized = m_children.capacity() == m_children.size();
	m_children.emplace_back(std::move(child));
	Entity & newChild = m_children.back();
	newChild.m_parent = this;

	if (inScene())
	{
		if (resized)
		{
			for (uint32 i = 0, end = m_children.size() - 1; i < end; ++i)
				m_scene->reindex(m_children[i]);
		}
		newChild.addToScene(*m_scene);
	}

	return newChild;
}

// ----------------------------------------------------------------------------

std::vector<Entity> & Entity::getChildren()
{
	return m_children;
}

// ----------------------------------------------------------------------------

const std::vector<Entity> & Entity::getChildren() const
{
	return m_children;
}

// ----------------------------------------------------------------------------

Entity & Entity::getChild(const Handle handle)
{
	static Entity s_empty(Handle::ms_invalid);
	const auto it = std::find_if(m_children.begin(), m_children.end(), [=](const Entity & child) { return child.getHandle() == handle; });
	return it != m_children.end() ? *it : s_empty;
}

// ----------------------------------------------------------------------------

const Entity & Entity::getChild(const Handle handle) const
{
	const auto it = std::find_if(m_children.begin(), m_children.end(), [=](const Entity & child) { return child.getHandle() == handle; });
	return it != m_children.end() ? *it : Entity::ms_invalid;
}

// ----------------------------------------------------------------------------

bool Entity::removeChild(const Handle handle)
{
	const auto it = std::find_if(m_children.begin(), m_children.end(), [=](const Entity & child) { return child.getHandle() == handle; });
	const bool found = it != m_children.end();
	if (found)
	{
		it->m_parent = nullptr;
		m_children.erase(it);
	}
	return found;
}

// ----------------------------------------------------------------------------

uint32 Entity::getComponentFlags() const
{
	return m_componentFlags;
}

// ----------------------------------------------------------------------------

void Entity::addToScene(Scene & scene)
{
	scene.index(*this);
	m_scene = &scene;

	for (auto & child : m_children)
		child.addToScene(scene);
}

// ----------------------------------------------------------------------------

bool Entity::inScene() const
{
	return m_scene != nullptr;
}

// ----------------------------------------------------------------------------

void Entity::removeFromScene()
{
	m_scene->deindex(m_handle);
	m_scene = nullptr;

	for (auto & child : m_children)
		child.removeFromScene();
}

// ----------------------------------------------------------------------------

Entity * Entity::getParent()
{
	return m_parent;
}

// ============================================================================
