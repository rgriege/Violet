#ifndef MAP_H
#define MAP_H

#include "violet/core/entity/Entity.h"
#include "violet/core/structures/CrsGraph.h"
#include "violet/core/math/Vec2.h"
#include "violet/game/Config.h"

namespace Violet
{
	class Deserializer;
	class Engine;
}

class VIOLET_GAME_API Map
{
public:

	class Intersection
	{
	public:

		Vec2f m_position;
	};

	class VIOLET_GAME_API Road
	{
	public:

		uint32 getSource() const;

	public:

		uint32 m_src;
		uint32 m_destination;
	};

public:

	Map();
	Map(Violet::Deserializer & deserializer);

	Violet::CrsGraph<Intersection, Road> & getGraph();
	const Violet::CrsGraph<Intersection, Road> & getGraph() const;

private:

	Violet::CrsGraph<Intersection, Road> m_graph;
	float m_roadWidth;
};

#endif