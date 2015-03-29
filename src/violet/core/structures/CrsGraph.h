#ifndef CRS_GRAPH_H
#define CRS_GRAPH_H

#include <vector>

namespace Violet
{
    template <class NodeType, class EdgeType>
	class CrsGraph
    {
    public:

        typedef std::vector<NodeType> Nodes;
        typedef std::vector<EdgeType> Edges;
        typedef typename Nodes::iterator NodeIterator;
		typedef typename Edges::iterator EdgeIterator;
		typedef typename Nodes::const_iterator ConstNodeIterator;
		typedef typename Edges::const_iterator ConstEdgeIterator;

	private:

		static bool compareEdges(const EdgeType & e1, const EdgeType & e2) {
			return e1.m_src < e2.m_src;
		}

    public:

        CrsGraph();

        unsigned addNode(const NodeType & node);
		bool hasNode(unsigned nodeIndex) const;
		NodeType & getNode(unsigned nodeIndex);
		const NodeType & getNode(unsigned nodeIndex) const;
        size_t numNodes() const;
		NodeIterator beginNodes();
		ConstNodeIterator beginNodes() const;
		NodeIterator endNodes();
		ConstNodeIterator endNodes() const;

        void addEdge(unsigned src, unsigned dst, const EdgeType & edge);
        /*
         * Used to add multiple edges at once.
         * Adding many edges using AddEdge(...) is highly inefficient,
         * since potentially lots of edge pointers will need updating.
         */
		void addEdges(const std::vector<unsigned> & sources, const std::vector<unsigned> & destinations, const Edges & edges);
        unsigned numEdges() const;
        EdgeIterator beginEdgesBySrc(unsigned src);
		ConstEdgeIterator beginEdgesBySrc(unsigned src) const;
        EdgeIterator endEdgesBySrc(unsigned src);
		ConstEdgeIterator endEdgesBySrc(unsigned src) const;

        void clear();

    private:

        Nodes m_nodes;
        std::vector<int> m_edgePointers;
        Edges m_edges;
    };
}

#include "violet/core/structures/CrsGraph.ipp"

#endif
