// ============================================================================

#include "game/pathfinding/mapcomponent.h"

#include "violet/engine.h"
#include "violet/math/polygon.h"
#include "violet/serialization/serializationutilities.h"

// ============================================================================

Violet::Tag MapComponent::getStaticTag()
{
	return Violet::Tag('m', 'a', 'p', 'c');
}

// ============================================================================

MapComponent::MapComponent(const Violet::EntityId entityId, Violet::Deserializer & Deserializer) :
	ComponentBase<MapComponent>(entityId),
	m_graph(),
	m_id(0)
{
	m_id = Deserializer.getUint("id");
	{
		auto nodesSegment = Deserializer.enterSegment("nodes");
		Violet::Vector<Vec2f> nodes = Violet::SerializationUtilities::deserializeElements<Vec2f>(*nodesSegment);
		for (auto const & node : nodes)
			m_graph.addNode({ node });
	}

	{
		auto edgesSegment = Deserializer.enterSegment("edges");
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

Violet::Serializer & operator<<(Violet::Serializer & Serializer, const MapComponent & component)
{
	Serializer.writeUint("id", component.m_id);
	{
		auto nodesSegment = Serializer.createSegment("nodes");
		Violet::Vector<Vec2f> nodes;
		for (auto const & node : component.m_graph.getNodes())
			nodes.emplace_back(node.second.m_position);
		Violet::SerializationUtilities::serializeElements(*nodesSegment, nodes);
	}
	{
		auto edgesSegment = Serializer.createSegment("edges");
		edgesSegment->writeUint("n", std::distance(component.m_graph.getEdges().begin(), component.m_graph.getEdges().end()));
		for (auto const & edge : component.m_graph.getEdges())
		{
			edgesSegment->writeUint("src", edge.m_src);
			edgesSegment->writeUint("dst", edge.m_destination);
		}
	}
	return Serializer;
}

// ============================================================================
