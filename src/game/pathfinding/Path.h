#ifndef GAME_Path_H
#define GAME_Path_H

#include "violet/math/Vec2.h"
#include "game/config.h"

#include <Vector>

class Map;

class VIOLET_GAME_API Path
{
public:

	static Path create(const Vec2f & start, const Vec2f & goal, const Map & map);

public:

	Path();
	Path(std::Vector<Vec2f> && points);
	Path(Path && other);

	const std::Vector<Vec2f> & getPoints() const;

private:

	std::Vector<Vec2f> m_points;
};

#endif