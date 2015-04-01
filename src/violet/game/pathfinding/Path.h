#ifndef PATH_H
#define PATH_H

#include "violet/core/math/Vec2.h"
#include "violet/game/Config.h"

#include <vector>

class Map;

class VIOLET_GAME_API Path
{
public:

	static Path create(const Vec2f & start, const Vec2f & goal, const Map & map);

public:

	Path();
	Path(std::vector<Vec2f> && points);
	Path(Path && other);

	const std::vector<Vec2f> & getPoints() const;

private:

	std::vector<Vec2f> m_points;
};

#endif