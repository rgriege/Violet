// ============================================================================

#include "engine/scene/SceneProcessor.h"

#include "engine/entity/Entity.h"

using namespace Violet;

// ============================================================================

namespace SceneProcessorNamespace
{
	class SiblingIterator
	{
	public:

		SiblingIterator(const std::vector<unique_val<Entity>> & siblings);

		const Entity & operator*() const;
		SiblingIterator & operator++();
		bool finished() const;

	private:

		std::vector<unique_val<Entity>>::const_iterator m_pos;
		std::vector<unique_val<Entity>>::const_iterator m_end;
	};
}

using namespace SceneProcessorNamespace;

// ============================================================================

bool SceneProcessor::Filter::qualifies(const Entity & entity) const
{
	return (entity.getComponentFlags() & m_componentFlags) == m_componentFlags;
}

// ----------------------------------------------------------------------------

SceneProcessor::Filter::Filter(uint32 componentFlags) :
	m_componentFlags(componentFlags)
{
}

// ============================================================================

void SceneProcessor::addDelegate(Filter filter, Delegate delegate)
{
	m_delegates.emplace_back(std::make_pair(filter, delegate));
}

// ----------------------------------------------------------------------------

void SceneProcessor::process(const Scene & scene, float dt)
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

SceneProcessorNamespace::SiblingIterator::SiblingIterator(const std::vector<unique_val<Entity>> & siblings) :
	m_pos(siblings.begin()),
	m_end(siblings.end())
{
}

// ----------------------------------------------------------------------------

const Entity & SceneProcessorNamespace::SiblingIterator::operator*() const
{
	return **m_pos;
}

// ----------------------------------------------------------------------------

SceneProcessorNamespace::SiblingIterator & SceneProcessorNamespace::SiblingIterator::operator++()
{
	++m_pos;
	return *this;
}

// ----------------------------------------------------------------------------

bool SceneProcessorNamespace::SiblingIterator::finished() const
{
	return m_pos == m_end;
}

// ============================================================================
