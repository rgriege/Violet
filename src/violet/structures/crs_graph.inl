// ============================================================================

#include <algorithm>
#include <assert.h>

// ============================================================================

template <typename NodeType, struct EdgeType>
vlt::Crs_Graph<NodeType, EdgeType>::NodeIterator::NodeIterator(const Nodes & nodes, const u32 index) :
	m_nodesIterator(nodes.begin() + index),
	m_index(index)
{
}

// ----------------------------------------------------------------------------

template <typename NodeType, struct EdgeType>
std::pair<u32, const NodeType &> vlt::Crs_Graph<NodeType, EdgeType>::NodeIterator::operator*()
{
	return std::pair<u32, const NodeType &>(m_index, *m_nodesIterator);
}

// ----------------------------------------------------------------------------

template <typename NodeType, struct EdgeType>
typename vlt::Crs_Graph<NodeType, EdgeType>::NodeIterator & vlt::Crs_Graph<NodeType, EdgeType>::NodeIterator::operator++()
{
	++m_nodesIterator;
	++m_index;
	return *this;
}

// ----------------------------------------------------------------------------

template <typename NodeType, struct EdgeType>
bool vlt::Crs_Graph<NodeType, EdgeType>::NodeIterator::operator!=(const NodeIterator & other) const
{
	return m_index != other.m_index;
}

// ============================================================================

template <typename NodeType, struct EdgeType>
vlt::Crs_Graph<NodeType, EdgeType>::NodeSegment::NodeSegment(NodeIterator begin, NodeIterator end) :
	m_begin(begin),
	m_end(end)
{
}

// ----------------------------------------------------------------------------

template <typename NodeType, struct EdgeType>
typename vlt::Crs_Graph<NodeType, EdgeType>::NodeIterator vlt::Crs_Graph<NodeType, EdgeType>::NodeSegment::begin()
{
	return m_begin;
}

// ----------------------------------------------------------------------------

template <typename NodeType, struct EdgeType>
typename vlt::Crs_Graph<NodeType, EdgeType>::NodeIterator vlt::Crs_Graph<NodeType, EdgeType>::NodeSegment::end()
{
	return m_end;
}

// ============================================================================

template <typename NodeType, struct EdgeType>
vlt::Crs_Graph<NodeType, EdgeType>::EdgeSegment::EdgeSegment(EdgeIterator begin, EdgeIterator end) :
	m_begin(begin),
	m_end(end)
{
}

// ----------------------------------------------------------------------------

template <typename NodeType, struct EdgeType>
typename vlt::Crs_Graph<NodeType, EdgeType>::EdgeIterator vlt::Crs_Graph<NodeType, EdgeType>::EdgeSegment::begin()
{
	return m_begin;
}

// ----------------------------------------------------------------------------

template <typename NodeType, struct EdgeType>
typename vlt::Crs_Graph<NodeType, EdgeType>::EdgeIterator vlt::Crs_Graph<NodeType, EdgeType>::EdgeSegment::end()
{
	return m_end;
}

// ============================================================================

template <typename NodeType, struct EdgeType>
vlt::Crs_Graph<NodeType, EdgeType>::ConstEdgeSegment::ConstEdgeSegment(ConstEdgeIterator begin, ConstEdgeIterator end) :
	m_begin(begin),
	m_end(end)
{
}

// ----------------------------------------------------------------------------

template <typename NodeType, struct EdgeType>
typename vlt::Crs_Graph<NodeType, EdgeType>::ConstEdgeIterator vlt::Crs_Graph<NodeType, EdgeType>::ConstEdgeSegment::begin() const
{
	return m_begin;
}

// ----------------------------------------------------------------------------

template <typename NodeType, struct EdgeType>
typename vlt::Crs_Graph<NodeType, EdgeType>::ConstEdgeIterator vlt::Crs_Graph<NodeType, EdgeType>::ConstEdgeSegment::end() const
{
	return m_end;
}

// ============================================================================

template <typename NodeType, struct EdgeType>
vlt::Crs_Graph<NodeType, EdgeType>::Crs_Graph() :
	m_nodes(),
	m_edgePointers(),
	m_edges()
{
}

// ----------------------------------------------------------------------------

template <typename NodeType, struct EdgeType>
u32 vlt::Crs_Graph<NodeType, EdgeType>::addNode(const NodeType& node)
{
	m_nodes.push_back(node);
	m_edgePointers.push_back(m_edges.size());
	return m_nodes.size();
}

// ----------------------------------------------------------------------------

template <typename NodeType, struct EdgeType>
bool vlt::Crs_Graph<NodeType, EdgeType>::hasNode(const u32 nodeIndex) const
{
	return nodeIndex >= 0 && nodeIndex < m_nodes.size();
}

// ----------------------------------------------------------------------------

template <typename NodeType, struct EdgeType>
NodeType & vlt::Crs_Graph<NodeType, EdgeType>::getNode(const u32 nodeIndex)
{
	assert(hasNode(nodeIndex));
	return m_nodes[nodeIndex];
}

// ----------------------------------------------------------------------------

template <typename NodeType, struct EdgeType>
const NodeType & vlt::Crs_Graph<NodeType, EdgeType>::getNode(const u32 nodeIndex) const
{
	assert(hasNode(nodeIndex));
	return m_nodes[nodeIndex];
}

// ----------------------------------------------------------------------------

template <typename NodeType, struct EdgeType>
size_t vlt::Crs_Graph<NodeType, EdgeType>::numNodes() const
{
	return m_nodes.size();
}

// ----------------------------------------------------------------------------

template <typename NodeType, struct EdgeType>
typename vlt::Crs_Graph<NodeType, EdgeType>::NodeSegment vlt::Crs_Graph<NodeType, EdgeType>::getNodes() const
{
	return NodeSegment(NodeIterator(m_nodes, 0), NodeIterator(m_nodes, m_nodes.size()));
}

// ----------------------------------------------------------------------------

template <typename NodeType, struct EdgeType>
void vlt::Crs_Graph<NodeType, EdgeType>::addEdge(const EdgeType & edge) {
	if (!m_edges.empty() && m_edges.back().getSource() <= edge.getSource()) {
		m_edges.push_back(edge);
	} else {
		const auto it = std::upper_bound(m_edges.begin(),
			m_edges.end(), edge, compareEdges);
		m_edges.insert(it, edge);
	}
	for (u32 i = edge.getSource(); i < m_edgePointers.size(); ++i)
		++m_edgePointers[i];
}

// ----------------------------------------------------------------------------

/*
	* Used to add multiple edges at once.
	* Adding many edges using AddEdge(...) is highly inefficient,
	* since potentially lots of edge pointers will need updating.
	*/
template <typename NodeType, struct EdgeType>
void vlt::Crs_Graph<NodeType, EdgeType>::addEdges(const Edges & edges) {
	if (edges.empty())
		return;

	/* Add the edges */
	for (auto it = edges.begin(); it != edges.end(); ++it) {
		if (!m_edges.empty() && m_edges.back().getSource() <= it->getSource()) {
			m_edges.push_back(*it);
		} else {
			const auto pos = upper_bound(m_edges.begin(), m_edges.end(), *it, compareEdges);
			m_edges.insert(pos, *it);
		}
	}

	/* Adjust the edge pointers */
	int current_src = 0;
	for (u32 i = 0; i < m_edges.size(); i++) {
		if (m_edges[i].getSource() != current_src) {
			for (int j = current_src; j < m_edges[i].getSource(); j++)
				m_edgePointers[j] = i;
			current_src = m_edges[i].getSource();
		}
	}
	for (u32 i = current_src; i < m_edgePointers.size(); i++)
		m_edgePointers[i] = m_edges.size();
}

// ----------------------------------------------------------------------------

template <typename NodeType, struct EdgeType>
u32 vlt::Crs_Graph<NodeType, EdgeType>::numEdges() const {
	return m_edges.size();
}

// ----------------------------------------------------------------------------

template <typename NodeType, struct EdgeType>
typename vlt::Crs_Graph<NodeType, EdgeType>::EdgeSegment vlt::Crs_Graph<NodeType, EdgeType>::getEdges()
{
	return EdgeSegment(m_edges.begin(), m_edges.end());
}

// ----------------------------------------------------------------------------

template <typename NodeType, struct EdgeType>
typename vlt::Crs_Graph<NodeType, EdgeType>::ConstEdgeSegment vlt::Crs_Graph<NodeType, EdgeType>::getEdges() const
{
	return ConstEdgeSegment(m_edges.begin(), m_edges.end());
}

// ----------------------------------------------------------------------------

template <typename NodeType, struct EdgeType>
typename vlt::Crs_Graph<NodeType, EdgeType>::EdgeSegment vlt::Crs_Graph<NodeType, EdgeType>::getEdges(const u32 src)
{
	return EdgeSegment(m_edges.begin() + (src == 0 ? 0 : m_edgePointers[src - 1]), m_edges.begin() + m_edgePointers[src]);
}

// ----------------------------------------------------------------------------

template <typename NodeType, struct EdgeType>
typename vlt::Crs_Graph<NodeType, EdgeType>::ConstEdgeSegment vlt::Crs_Graph<NodeType, EdgeType>::getEdges(const u32 src) const
{
	return ConstEdgeSegment(m_edges.begin() + (src == 0 ? 0 : m_edgePointers[src - 1]), m_edges.begin() + m_edgePointers[src]);
}

// ----------------------------------------------------------------------------

template <typename NodeType, struct EdgeType>
void vlt::Crs_Graph<NodeType, EdgeType>::clear() {
	m_nodes.clear();
	m_edgePointers.clear();
	m_edges.clear();
}

// ============================================================================

template <typename NodeType, struct EdgeType>
bool vlt::Crs_Graph<NodeType, EdgeType>::compareEdges(const EdgeType & e1, const EdgeType & e2)
{
	return e1.getSource() < e2.getSource();
}

// ============================================================================
