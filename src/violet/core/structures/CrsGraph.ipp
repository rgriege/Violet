#ifndef CRS_GRAPH_IPP
#define CRS_GRAPH_IPP

#include <assert.h>

using namespace Violet;

template <class NodeType, class EdgeType>
CrsGraph<NodeType, EdgeType>::CrsGraph() :
    m_nodes(),
    m_edgePointers(),
    m_edges()
{
}

template <class NodeType, class EdgeType>
unsigned CrsGraph<NodeType, EdgeType>::addNode(const NodeType& node) {
    m_nodes.push_back(node);
    m_edgePointers.push_back(m_edges.size());
    return m_nodes.size();
}

template <class NodeType, class EdgeType>
bool CrsGraph<NodeType, EdgeType>::hasNode(unsigned nodeIndex) const
{
	return nodeIndex >= 0 && nodeIndex < m_nodes.size();
}

template <class NodeType, class EdgeType>
NodeType & CrsGraph<NodeType, EdgeType>::getNode(unsigned nodeIndex) {
    assert(hasNode(nodeIndex));
    return m_nodes[nodeIndex];
}

template <class NodeType, class EdgeType>
const NodeType & CrsGraph<NodeType, EdgeType>::getNode(unsigned nodeIndex) const {
    assert(hasNode(nodeIndex));
    return m_nodes[nodeIndex];
}

template <class NodeType, class EdgeType>
size_t CrsGraph<NodeType, EdgeType>::numNodes() const {
    return m_nodes.size();
}

template <class NodeType, class EdgeType>
typename CrsGraph<NodeType, EdgeType>::NodeIterator CrsGraph<NodeType, EdgeType>::beginNodes() {
    return m_nodes.begin();
}

template <class NodeType, class EdgeType>
typename CrsGraph<NodeType, EdgeType>::ConstNodeIterator CrsGraph<NodeType, EdgeType>::beginNodes() const {
    return m_nodes.begin();
}

template <class NodeType, class EdgeType>
typename CrsGraph<NodeType, EdgeType>::NodeIterator CrsGraph<NodeType, EdgeType>::endNodes() {
    return m_nodes.end();
}

template <class NodeType, class EdgeType>
typename CrsGraph<NodeType, EdgeType>::ConstNodeIterator CrsGraph<NodeType, EdgeType>::endNodes() const {
    return m_nodes.end();
}

template <class NodeType, class EdgeType>
void CrsGraph<NodeType, EdgeType>::addEdge(unsigned src, unsigned dst, const EdgeType & edge) {
    if (!m_edges.empty() && m_edges.back().m_src <= edge.m_src) {
        m_edges.push_back(edge);
    } else {
        typename EdgeIterator it = upper_bound(m_edges.begin(),
            m_edges.end(), edge, compareEdges);
        m_edges.insert(it, edge);
    }
    for (unsigned i = edge.m_src; i < m_edgePointers.size(); ++i)
        ++m_edgePointers[i];
}

/*
    * Used to add multiple edges at once.
    * Adding many edges using AddEdge(...) is highly inefficient,
    * since potentially lots of edge pointers will need updating.
    */
template <class NodeType, class EdgeType>
void CrsGraph<NodeType, EdgeType>::addEdges(const std::vector<unsigned> & sources, const std::vector<unsigned> & destinations, const Edges & edges) {
    if (edges.empty())
        return;

    /* Add the edges */
    for (typename EdgeIterator it = edges.begin(); it != edges.end(); ++it) {
        if (!m_edges.empty() && m_edges.back().m_src <= it->m_src) {
            m_edges.push_back(*it);
        } else {
            typename EdgeIterator pos = upper_bound(m_edges.begin(), m_edges.end(), *it, compareEdges);
            m_edges.insert(pos, *it);
        }
    }

    /* Adjust the edge pointers */
    int current_src = 0;
    for (unsigned i = 0; i < m_edges.size(); i++) {
        if (m_edges[i].m_src != current_src) {
            for (int j = current_src; j < m_edges[i].m_src; j++)
                m_edgePointers[j] = i;
            current_src = m_edges[i].m_src;
        }
    }
    for (unsigned i = current_src; i < m_edgePointers.size(); i++)
        m_edgePointers[i] = m_edges.size();
}

template <class NodeType, class EdgeType>
unsigned CrsGraph<NodeType, EdgeType>::numEdges() const {
    return m_edges.size();
}

template <class NodeType, class EdgeType>
typename CrsGraph<NodeType, EdgeType>::EdgeIterator CrsGraph<NodeType, EdgeType>::beginEdgesBySrc(unsigned src) {
    return m_edges.begin() + (src == 0 ? 0 : m_edgePointers[src-1]);
}

template <class NodeType, class EdgeType>
typename CrsGraph<NodeType, EdgeType>::ConstEdgeIterator CrsGraph<NodeType, EdgeType>::beginEdgesBySrc(unsigned src) const {
    return m_edges.begin() + (src == 0 ? 0 : m_edgePointers[src-1]);
}

template <class NodeType, class EdgeType>
typename CrsGraph<NodeType, EdgeType>::EdgeIterator CrsGraph<NodeType, EdgeType>::endEdgesBySrc(unsigned src) {
    return m_edges.begin() + m_edgePointers[src];
}

template <class NodeType, class EdgeType>
typename CrsGraph<NodeType, EdgeType>::ConstEdgeIterator CrsGraph<NodeType, EdgeType>::endEdgesBySrc(unsigned src) const {
    return m_edges.begin() + m_edgePointers[src];
}

template <class NodeType, class EdgeType>
void CrsGraph<NodeType, EdgeType>::clear() {
    m_nodes.clear();
    m_edgePointers.clear();
    m_edges.clear();
}

#endif