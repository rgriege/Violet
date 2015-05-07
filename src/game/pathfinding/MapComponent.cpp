// ============================================================================

#include "game/pathfinding/MapComponent.h"

#include "engine/Engine.h"
#include "engine/math/Polygon.h"
#include "engine/serialization/SerializationUtilities.h"

// ============================================================================

Violet::Tag MapComponent::getTypeId()
{
	return Violet::Tag('m', 'a', 'p', 'c');
}

// ============================================================================

MapComponent::MapComponent(const Violet::Entity entity, Violet::Deserializer & deserializer) :
	Component<MapComponent>(entity),
	m_graph()
{
	{
		auto nodesSegment = deserializer.enterSegment("nodes");
		std::vector<Vec2f> nodes = SerializationUtilities::deserializeElements<Vec2f>(*nodesSegment);
		for (auto const & node : nodes)
			m_graph.addNode({ node });
	}

	{
		auto edgesSegment = deserializer.enterSegment("edges");
		const uint32 n = edgesSegment->getUint("n");
		for (uint32 i = 0; i < n; ++i)
		{
			const uint32 src = edgesSegment->getUint("src");
			const uint32 dst = edgesSegment->getUint("dst");
			m_graph.addEdge({ src, dst });
		}
	}
}

// ============================================================================
