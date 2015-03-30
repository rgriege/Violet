#include "violet/game/pathfinding/Path.h"

#include "violet/game/pathfinding/Map.h"

#include <queue>

namespace PathNamespace
{
	class AStarNode
	{
	public:

		uint32 m_node;
		Vec2f m_position;
		float m_cost;
		uint32 m_previousNode;
	};

	class AStarOpenSet : public std::priority_queue<AStarNode>
	{
	public:

		typedef std::vector<AStarNode>::iterator Iterator;

	public:

		Iterator begin();
		Iterator end();
		void resort();
	};

	bool operator<(const AStarNode & lhs, const AStarNode & rhs)
	{
		return lhs.m_cost > rhs.m_cost;
	}

	template <typename T>
	void reverse(std::vector<T> & vec);
}

using namespace PathNamespace;

Path Path::create(const Vec2f & start, const Vec2f & goal, const Map & map)
{
	AStarOpenSet openSet;
	std::vector<AStarNode> closedSet;

	uint32 firstNode = 0;
	float minDistance = std::numeric_limits<float>::max();
	for (auto const & node : map.getGraph().getNodes())
	{
		const float distance = (node.m_position - start).magnitude();
		if (distance < minDistance)
		{
			firstNode;
			minDistance = distance;
		}
	}

	openSet.push({ firstNode, map.getGraph().getNode(firstNode).m_position, minDistance, -1 });
	while (!openSet.empty() && (closedSet.empty() || closedSet.back().m_position != goal)) {
		closedSet.push_back(openSet.top());
		const AStarNode & currentNode = closedSet.back();
		const Map::Intersection & currentIntersection = map.getGraph().getNode(currentNode.m_node);
		openSet.pop();
		if (closedSet.back().m_position != goal)
		{
			for (auto const & edge : map.getGraph().getEdges(currentNode.m_node))
			{
				const Map::Intersection & nextIntersection = map.getGraph().getNode(edge.m_destination);
				if (std::find_if(closedSet.begin(), closedSet.end(), [&](AStarNode const & node) { return node.m_position == nextIntersection.m_position; }) == closedSet.end())
				{
					auto it = std::find_if(openSet.begin(), openSet.end(), [&](const AStarNode & node) { return node.m_node == edge.m_destination; });
					if (it != openSet.end())
					{
						float newCost = currentNode.m_cost + (nextIntersection.m_position - currentIntersection.m_position).magnitude();
						if (it->m_cost > newCost)
						{
							it->m_cost = newCost;
							it->m_previousNode = closedSet.size() - 1;
							openSet.resort();
						}
					}
					else {
						AStarNode node = { edge.m_destination, nextIntersection.m_position, currentNode.m_cost + (nextIntersection.m_position - currentIntersection.m_position).magnitude(), closedSet.size() - 1 };
						openSet.push(node);
					}
				}
			}
		}
	}

	std::vector<Vec2f> points;
	uint32 node = closedSet.size() - 1;
	while (node != -1)
	{
		points.emplace_back(closedSet[node].m_position);
		node = closedSet[node].m_previousNode;
	}
	reverse(points);
	return Path(std::move(points));
}

Path::Path(std::vector<Vec2f> && points) :
	m_points(std::move(points))
{
}

const std::vector<Vec2f> & Path::getPoints() const
{
	return m_points;
}

AStarOpenSet::Iterator PathNamespace::AStarOpenSet::begin()
{
	return c.begin();
}

AStarOpenSet::Iterator PathNamespace::AStarOpenSet::end()
{
	return c.end();
}

void PathNamespace::AStarOpenSet::resort()
{
	std::sort(begin(), end());
}

template <typename T>
void PathNamespace::reverse(std::vector<T> & vec)
{
	if (vec.size() < 2)
		return;

	auto left = vec.begin();
	auto right = vec.end() - 1;
	while (right - left > 0)
	{
		std::swap(*left, *right);
		++left;
		--right;
	}
}