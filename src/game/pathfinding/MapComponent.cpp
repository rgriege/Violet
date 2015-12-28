// ============================================================================

#include "game/pathfinding/MapComponent.h"

#include "violet/Engine.h"
#include "violet/math/Polygon.h"
#include "violet/serialization/SerializationUtilities.h"

// ============================================================================

Violet::Tag MapComponent::getStaticTag()
{
	return Violet::Tag('m', 'a', 'p', 'c');
}

// ============================================================================

MapComponent::MapComponent(const Violet::EntityId entityId, Violet::Deserializer & deserializer) :
	ComponentBase<MapComponent>(entityId),
	m_graph(),
	m_id(0)
{
	m_id = deserializer.getUint("id");
	{
		auto nodesSegment = deserializer.enterSegment("nodes");
		Violet::Vector<Vec2f> nodes = Violet::SerializationUtilities::deserializeElements<Vec2f>(*nodesSegment);
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

Violet::Serializer & operator<<(Violet::Serializer & serializer, const MapComponent & component)
{
	serializer.writeUint("id", component.m_id);
	{
		auto nodesSegment = serializer.createSegment("nodes");
		Violet::Vector<Vec2f> nodes;
		for (auto const & node : component.m_graph.getNodes())
			nodes.emplace_back(node.second.m_position);
		Violet::SerializationUtilities::serializeElements(*nodesSegment, nodes);
	}
	{
		auto edgesSegment = serializer.createSegment("edges");
		edgesSegment->writeUint("n", std::distance(component.m_graph.getEdges().begin(), component.m_graph.getEdges().end()));
		for (auto const & edge : component.m_graph.getEdges())
		{
			edgesSegment->writeUint("src", edge.m_src);
			edgesSegment->writeUint("dst", edge.m_destination);
		}
	}
	return serializer;
}

// ============================================================================
