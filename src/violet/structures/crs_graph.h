#ifndef VIOLET_CRS_GRAPH_H
#define VIOLET_CRS_GRAPH_H

#include "violet/core/defines.h"

#include <vector>

namespace vlt
{
	template <typename NodeType, struct EdgeType>
	struct crs_graph
	{
	public:

		typedef std::vector<NodeType> Nodes;

		struct NodeIterator
		{
		public:

			NodeIterator(const Nodes & nodes, u32 index);

			NodeIterator & operator++();
			std::pair<u32, const NodeType &> operator*();
			bool operator!=(const NodeIterator & other) const;

		private:

			typename Nodes::const_iterator m_nodesIterator;
			u32 m_index;
		};

		struct NodeSegment
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

		struct EdgeSegment
		{
		public:

			EdgeSegment(EdgeIterator begin, EdgeIterator end);

			EdgeIterator begin();
			EdgeIterator end();

		private:

			const EdgeIterator m_begin;
			const EdgeIterator m_end;
		};

		struct ConstEdgeSegment
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

		crs_graph();

		u32 addNode(const NodeType & node);
		bool hasNode(u32 nodeIndex) const;
		NodeType & getNode(u32 nodeIndex);
		const NodeType & getNode(u32 nodeIndex) const;
		size_t numNodes() const;
		NodeSegment getNodes() const;

		void addEdge(const EdgeType & edge);
		/*
		 * Used to add multiple edges at once.
		 * Adding many edges using AddEdge(...) is highly inefficient,
		 * since potentially lots of edge pointers will need updating.
		 */
		void addEdges(const Edges & edges);
		u32 numEdges() const;
		EdgeSegment getEdges();
		ConstEdgeSegment getEdges() const;
		EdgeSegment getEdges(u32 src);
		ConstEdgeSegment getEdges(u32 src) const;

		void clear();

	private:

		static bool compareEdges(const EdgeType & e1, const EdgeType & e2);

	private:

		Nodes m_nodes;
		std::vector<int> m_edgePointers;
		Edges m_edges;
	};
}

#include "violet/structures/crs_graph.inl"

#endif
