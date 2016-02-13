#include "test/game/PathfindingTest.h"

#include "game/pathfinding/Map.h"
#include "game/pathfinding/Path.h"
#include "test/core/TestEvaluator.h"
#include "test/core/TestFactory.h"

#include <iostream>

std::ostream & operator<<(std::ostream & os, const Path & path);
bool operator==(const Path & lhs, const Path & rhs);

void PathfindingTest::run(vlt::TestEvaluator & evaluator)
{
	Map map;
	map.addNode({ { 0.f, 0.f } });
	map.addNode({ { 10.f, 0.f } });
	map.addNode({ { 10.f, 2.f } });
	map.addNode({ { 0.f, 2.f } });
	map.addNode({ { 5.f, 2.f } });
	map.addNode({ { 5.f, 8.f } });
	map.addNode({ { -10.f, 8.f } });
	map.addNode({ { 10.f, 8.f } });

	map.addEdge({ 0, 1 });
	map.addEdge({ 1, 2 });
	map.addEdge({ 0, 3 });
	map.addEdge({ 3, 4 });
	map.addEdge({ 2, 4 });
	map.addEdge({ 4, 5 });
	map.addEdge({ 5, 6 });
	map.addEdge({ 5, 7 });

	auto const path = Path::create({ 0.f, 0.f }, { -10.f, 8.f }, map);

	if (path == Path({ { 0.f, 0.f }, { 0.f, 2.f }, { 5.f, 2.f }, { 5.f, 8.f }, { -10.f, 8.f } }))
		std::cout << path << std::endl;

	//TestFactory::makeStateless("pathfinding", path, []() { return Path({ { 0.f, 0.f }, { 0.f, 2.f }, { 5.f, 2.f }, { 5.f, 8.f }, { -10.f, 8.f } }); }).evaluate(evaluator);
	evaluator.evaluate("pathfinding", Path({ { 0.f, 0.f }, { 0.f, 2.f }, { 5.f, 2.f }, { 5.f, 8.f }, { -10.f, 8.f } }), path);
}

std::ostream & operator<<(std::ostream & os, const Path & path)
{
	os << "[";
	for (u32 i = 0, end = path.getPoints().size(); i < end; ++i)
		os << path.getPoints()[i] << (i < end - 1 ? "," : "");
	os << "]";
	return os;
}

bool operator==(const Path & lhs, const Path & rhs)
{
	if (lhs.getPoints().size() != rhs.getPoints().size())
		return false;

	for (u32 i = 0, end = lhs.getPoints().size(); i != end; ++i)
		if (lhs.getPoints()[i] != rhs.getPoints()[i])
			return false;

	return true;
}