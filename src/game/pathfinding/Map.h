#ifndef GAME_Map_H
#define GAME_Map_H

#include "engine/math/Vec2.h"
#include "engine/structures/CrsGraph.h"
#include "game/Config.h"

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

class Map : public Violet::CrsGraph<Intersection, Road> {};

#endif