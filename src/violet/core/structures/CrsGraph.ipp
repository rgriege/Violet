#ifndef CRS_GRAPH_IPP
#define CRS_GRAPH_IPP

#include <algorithm>
#include <assert.h>

using namespace Violet;

template <class NodeType, class EdgeType>
CrsGraph<NodeType, EdgeType>::NodeIterator::NodeIterator(const Nodes & nodes, const uint32 index) :
	m_nodesIterator(nodes.begin() + index),
	m_index(index)
{
}

template <class NodeType, class EdgeType>
std::pair<uint32, const NodeType &> CrsGraph<NodeType, EdgeType>::NodeIterator::operator*()
{
	return std::pair<uint32, const NodeType &>(m_index, *m_nodesIterator);
}

template <class NodeType, class EdgeType>
typename CrsGraph<NodeType, EdgeType>::NodeIterator & CrsGraph<NodeType, EdgeType>::NodeIterator::operator++()
{
	++m_nodesIterator;
	++m_index;
	return *this;
}

template <class NodeType, class EdgeType>
bool typename CrsGraph<NodeType, EdgeType>::NodeIterator::operator!=(const NodeIterator & other) const
{
	return m_index != other.m_index;
}

template <class NodeType, class EdgeType>
CrsGraph<NodeType, EdgeType>::NodeSegment::NodeSegment(NodeIterator begin, NodeIterator end) :
	m_begin(begin),
	m_end(end)
{
}

template <class NodeType, class EdgeType>
typename CrsGraph<NodeType, EdgeType>::NodeIterator CrsGraph<NodeType, EdgeType>::NodeSegment::begin()
{
	return m_begin;
}

template <class NodeType, class EdgeType>
typename CrsGraph<NodeType, EdgeType>::NodeIterator CrsGraph<NodeType, EdgeType>::NodeSegment::end()
{
	return m_end;
}

template <class NodeType, class EdgeType>
CrsGraph<NodeType, EdgeType>::EdgeSegment::EdgeSegment(EdgeIterator begin, EdgeIterator end) :
	m_begin(begin),
	m_end(end)
{
}

template <class NodeType, class EdgeType>
typename CrsGraph<NodeType, EdgeType>::EdgeIterator CrsGraph<NodeType, EdgeType>::EdgeSegment::begin()
{
	return m_begin;
}

template <class NodeType, class EdgeType>
typename CrsGraph<NodeType, EdgeType>::EdgeIterator CrsGraph<NodeType, EdgeType>::EdgeSegment::end()
{
	return m_end;
}

template <class NodeType, class EdgeType>
CrsGraph<NodeType, EdgeType>::ConstEdgeSegment::ConstEdgeSegment(ConstEdgeIterator begin, ConstEdgeIterator end) :
	m_begin(begin),
	m_end(end)
{
}

template <class NodeType, class EdgeType>
typename CrsGraph<NodeType, EdgeType>::ConstEdgeIterator CrsGraph<NodeType, EdgeType>::ConstEdgeSegment::begin() const
{
	return m_begin;
}

template <class NodeType, class EdgeType>
typename CrsGraph<NodeType, EdgeType>::ConstEdgeIterator CrsGraph<NodeType, EdgeType>::ConstEdgeSegment::end() const
{
	return m_end;
}

template <class NodeType, class EdgeType>
bool CrsGraph<NodeType, EdgeType>::compareEdges(const EdgeType & e1, const EdgeType & e2)
{
	return e1.getSource() < e2.getSource();
}

template <class NodeType, class EdgeType>
CrsGraph<NodeType, EdgeType>::CrsGraph() :
    m_nodes(),
    m_edgePointers(),
    m_edges()
{
}

template <class NodeType, class EdgeType>
uint32 CrsGraph<NodeType, EdgeType>::addNode(const NodeType& node)
{
    m_nodes.push_back(node);
    m_edgePointers.push_back(m_edges.size());
    return m_nodes.size();
}

template <class NodeType, class EdgeType>
bool CrsGraph<NodeType, EdgeType>::hasNode(const uint32 nodeIndex) const
{
	return nodeIndex >= 0 && nodeIndex < m_nodes.size();
}

template <class NodeType, class EdgeType>
NodeType & CrsGraph<NodeType, EdgeType>::getNode(const uint32 nodeIndex)
{
    assert(hasNode(nodeIndex));
    return m_nodes[nodeIndex];
}

template <class NodeType, class EdgeType>
const NodeType & CrsGraph<NodeType, EdgeType>::getNode(const uint32 nodeIndex) const
{
    assert(hasNode(nodeIndex));
    return m_nodes[nodeIndex];
}

template <class NodeType, class EdgeType>
size_t CrsGraph<NodeType, EdgeType>::numNodes() const
{
    return m_nodes.size();
}

template <class NodeType, class EdgeType>
typename CrsGraph<NodeType, EdgeType>::NodeSegment CrsGraph<NodeType, EdgeType>::getNodes() const
{
	return NodeSegment(NodeIterator(m_nodes, 0), NodeIterator(m_nodes, m_nodes.size()));
}

template <class NodeType, class EdgeType>
void CrsGraph<NodeType, EdgeType>::addEdge(const EdgeType & edge) {
    if (!m_edges.empty() && m_edges.back().getSource() <= edge.getSource()) {
        m_edges.push_back(edge);
    } else {
        typename EdgeIterator it = std::upper_bound(m_edges.begin(),
            m_edges.end(), edge, compareEdges);
        m_edges.insert(it, edge);
    }
    for (uint32 i = edge.getSource(); i < m_edgePointers.size(); ++i)
        ++m_edgePointers[i];
}

/*
    * Used to add multiple edges at once.
    * Adding many edges using AddEdge(...) is highly inefficient,
    * since potentially lots of edge pointers will need updating.
    */
template <class NodeType, class EdgeType>
void CrsGraph<NodeType, EdgeType>::addEdges(const Edges & edges) {
    if (edges.empty())
        return;

    /* Add the edges */
    for (typename EdgeIterator it = edges.begin(); it != edges.end(); ++it) {
        if (!m_edges.empty() && m_edges.back().getSource() <= it->getSource()) {
            m_edges.push_back(*it);
        } else {
            typename EdgeIterator pos = upper_bound(m_edges.begin(), m_edges.end(), *it, compareEdges);
            m_edges.insert(pos, *it);
        }
    }

    /* Adjust the edge pointers */
    int current_src = 0;
    for (uint32 i = 0; i < m_edges.size(); i++) {
        if (m_edges[i].getSource() != current_src) {
            for (int j = current_src; j < m_edges[i].getSource(); j++)
                m_edgePointers[j] = i;
            current_src = m_edges[i].getSource();
        }
    }
    for (uint32 i = current_src; i < m_edgePointers.size(); i++)
        m_edgePointers[i] = m_edges.size();
}

template <class NodeType, class EdgeType>
uint32 CrsGraph<NodeType, EdgeType>::numEdges() const {
    return m_edges.size();
}

template <class NodeType, class EdgeType>
typename CrsGraph<NodeType, EdgeType>::EdgeSegment CrsGraph<NodeType, EdgeType>::getEdges()
{
	return EdgeSegment(m_edges.begin(), m_edges.end());
}

template <class NodeType, class EdgeType>
typename CrsGraph<NodeType, EdgeType>::ConstEdgeSegment CrsGraph<NodeType, EdgeType>::getEdges() const
{
	return ConstEdgeSegment(m_edges.begin(), m_edges.end());
}

template <class NodeType, class EdgeType>
typename CrsGraph<NodeType, EdgeType>::EdgeSegment CrsGraph<NodeType, EdgeType>::getEdges(const uint32 src)
{
	return EdgeSegment(m_edges.begin() + (src == 0 ? 0 : m_edgePointers[src - 1]), m_edges.begin() + m_edgePointers[src]);
}

template <class NodeType, class EdgeType>
typename CrsGraph<NodeType, EdgeType>::ConstEdgeSegment CrsGraph<NodeType, EdgeType>::getEdges(const uint32 src) const
{
	return ConstEdgeSegment(m_edges.begin() + (src == 0 ? 0 : m_edgePointers[src - 1]), m_edges.begin() + m_edgePointers[src]);
}

template <class NodeType, class EdgeType>
void CrsGraph<NodeType, EdgeType>::clear() {
    m_nodes.clear();
    m_edgePointers.clear();
    m_edges.clear();
}

#endif