#ifndef CRS_GRAPH_H
#define CRS_GRAPH_H

#include "violet/core/Defines.h"

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

		class EdgeSegment
		{
		public:

			EdgeSegment(EdgeIterator begin, EdgeIterator end);

			EdgeIterator begin();
			EdgeIterator end();

		private:

			const EdgeIterator m_begin;
			const EdgeIterator m_end;
		};

		class ConstEdgeSegment
		{
		public:

			ConstEdgeSegment(ConstEdgeIterator begin, ConstEdgeIterator end);

			ConstEdgeIterator begin() const;
			ConstEdgeIterator end() const;

		private:

			const ConstEdgeIterator m_begin;
			const ConstEdgeIterator m_end;
		};

	private:

		static bool compareEdges(const EdgeType & e1, const EdgeType & e2);

    public:

        CrsGraph();

        uint32 addNode(const NodeType & node);
		bool hasNode(uint32 nodeIndex) const;
		NodeType & getNode(uint32 nodeIndex);
		const NodeType & getNode(uint32 nodeIndex) const;
        size_t numNodes() const;
		const Nodes & getNodes() const;

        void addEdge(const EdgeType & edge);
        /*
         * Used to add multiple edges at once.
         * Adding many edges using AddEdge(...) is highly inefficient,
         * since potentially lots of edge pointers will need updating.
         */
		void addEdges(const Edges & edges);
        uint32 numEdges() const;
		EdgeSegment getEdges(uint32 src);
		ConstEdgeSegment getEdges(uint32 src) const;

        void clear();

    private:

        Nodes m_nodes;
        std::vector<int> m_edgePointers;
        Edges m_edges;
    };
}

#include "violet/core/structures/CrsGraph.ipp"

#endif
