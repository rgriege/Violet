#ifndef VIOLET_CrsGraph_H
#define VIOLET_CrsGraph_H

#include "violet/Defines.h"

#include <vector>

namespace Violet
{
    template <class NodeType, class EdgeType>
	class CrsGraph
    {
    public:

		typedef std::vector<NodeType> Nodes;

		class NodeIterator
		{
		public:

			NodeIterator(const Nodes & nodes, uint32 index);

			NodeIterator & operator++();
			std::pair<uint32, const NodeType &> operator*();
			bool operator!=(const NodeIterator & other) const;

		private:

			typename Nodes::const_iterator m_nodesIterator;
			uint32 m_index;
		};

		class NodeSegment
		{
		public:

			NodeSegment(NodeIterator begin, NodeIterator end);

			NodeIterator begin();
			NodeIterator end();

		private:

			NodeIterator m_begin;
			NodeIterator m_end;
		};

        typedef std::vector<EdgeType> Edges;
		typedef typename Edges::iterator EdgeIterator;
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

    public:

        CrsGraph();

        uint32 addNode(const NodeType & node);
		bool hasNode(uint32 nodeIndex) const;
		NodeType & getNode(uint32 nodeIndex);
		const NodeType & getNode(uint32 nodeIndex) const;
        size_t numNodes() const;
		NodeSegment getNodes() const;

        void addEdge(const EdgeType & edge);
        /*
         * Used to add multiple edges at once.
         * Adding many edges using AddEdge(...) is highly inefficient,
         * since potentially lots of edge pointers will need updating.
         */
		void addEdges(const Edges & edges);
        uint32 numEdges() const;
		EdgeSegment getEdges();
		ConstEdgeSegment getEdges() const;
		EdgeSegment getEdges(uint32 src);
		ConstEdgeSegment getEdges(uint32 src) const;

		void clear();

	private:

		static bool compareEdges(const EdgeType & e1, const EdgeType & e2);

    private:

        Nodes m_nodes;
        std::vector<int> m_edgePointers;
        Edges m_edges;
    };
}

#include "violet/structures/CrsGraph.inl"

#endif
