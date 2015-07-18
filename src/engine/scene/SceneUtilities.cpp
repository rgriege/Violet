// ============================================================================

#include "engine/scene/SceneUtilities.h"

#include "engine/entity/Entity.h"

using namespace Violet;

// ============================================================================

bool SceneUtilities::Processor::Filter::qualifies(const Entity & entity) const
{
	return (entity.getComponentFlags() & m_componentFlags) == m_componentFlags;
}

// ----------------------------------------------------------------------------

SceneUtilities::Processor::Filter::Filter(uint32 componentFlags) :
	m_componentFlags(componentFlags)
{
}


// ============================================================================

SceneUtilities::Processor::SiblingIterator::SiblingIterator(const std::vector<std::unique_ptr<Entity>> & siblings) :
	m_pos(siblings.begin()),
	m_end(siblings.end())
{
}

// ----------------------------------------------------------------------------

const Entity & SceneUtilities::Processor::SiblingIterator::operator*() const
{
	return **m_pos;
}

// ----------------------------------------------------------------------------

SceneUtilities::Processor::SiblingIterator & SceneUtilities::Processor::SiblingIterator::operator++()
{
	++m_pos;
	return *this;
}

// ----------------------------------------------------------------------------

bool SceneUtilities::Processor::SiblingIterator::finished() const
{
	return m_pos == m_end;
}

// ============================================================================

void SceneUtilities::Processor::addDelegate(Filter filter, Delegate delegate)
{
	m_delegates.emplace_back(std::make_pair(filter, delegate));
}

// ----------------------------------------------------------------------------

void SceneUtilities::Processor::process(const Scene & scene, float dt)
{
	std::stack<SiblingIterator> iterators;
	auto const & rootChildren = scene.getRoot().getChildren();
	if (rootChildren.empty())
		return;

	iterators.emplace(rootChildren);
	while (!iterators.empty())
	{
		SiblingIterator & it = iterators.top();
		const auto & entity = *it;
		for (auto const & delegate : m_delegates)
			if (delegate.first.qualifies(entity))
				delegate.second(entity, dt);

		if ((++it).finished())
			iterators.pop();

		auto const & children = entity.getChildren();
		if (!children.empty())
			iterators.emplace(children);
	}
}

// ============================================================================