// ============================================================================

#include "violet/scene/SceneProcessor.h"

#include "violet/entity/Entity.h"

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

bool SceneProcessor::Filter::operator==(const Filter & rhs) const
{
	return m_componentFlags == rhs.m_componentFlags;
}

// ----------------------------------------------------------------------------

SceneProcessor::Filter::Filter(const uint32 componentFlags) :
	m_componentFlags(componentFlags)
{
}

// ============================================================================

SceneProcessor::~SceneProcessor()
{
	assert(m_delegates.empty());
}

// ----------------------------------------------------------------------------

void SceneProcessor::addDelegate(const Filter filter, const Delegate & delegate)
{
	m_delegates.emplace_back(std::make_pair(filter, delegate));
}

// ----------------------------------------------------------------------------

void SceneProcessor::removeDelegate(const Filter filter, const Delegate & delegate)
{
	m_delegates.erase(std::remove_if(m_delegates.begin(), m_delegates.end(),
		[filter, &delegate](const std::pair<Filter, Delegate> & filteredDelegate)
		{
			return filteredDelegate.first == filter && *filteredDelegate.second.target<void(*)(const Entity &, float)>() == *delegate.target<void(*)(const Entity &, float)>();
		}), m_delegates.end());
}

// ----------------------------------------------------------------------------

void SceneProcessor::process(const Scene & scene, const float dt)
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
