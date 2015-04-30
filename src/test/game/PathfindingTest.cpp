#include "test/game/PathfindingTest.h"

#include "game/pathfinding/Map.h"
#include "game/pathfinding/Path.h"
#include "test/core/TestEvaluator.h"
#include "test/core/TestFactory.h"

#include <iostream>

std::ostream & operator<<(std::ostream & os, const Path & path);
bool operator==(const Path & lhs, const Path & rhs);

void PathfindingTest::run(Violet::TestEvaluator & evaluator)
{
	Map map;
	map.getGraph().addNode({ { 0.f, 0.f } });
	map.getGraph().addNode({ { 10.f, 0.f } });
	map.getGraph().addNode({ { 10.f, 2.f } });
	map.getGraph().addNode({ { 0.f, 2.f } });
	map.getGraph().addNode({ { 5.f, 2.f } });
	map.getGraph().addNode({ { 5.f, 8.f } });
	map.getGraph().addNode({ { -10.f, 8.f } });
	map.getGraph().addNode({ { 10.f, 8.f } });

	map.getGraph().addEdge({ 0, 1 });
	map.getGraph().addEdge({ 1, 2 });
	map.getGraph().addEdge({ 0, 3 });
	map.getGraph().addEdge({ 3, 4 });
	map.getGraph().addEdge({ 2, 4 });
	map.getGraph().addEdge({ 4, 5 });
	map.getGraph().addEdge({ 5, 6 });
	map.getGraph().addEdge({ 5, 7 });

	auto const path = Path::create({ 0.f, 0.f }, { -10.f, 8.f }, map);

	if (path == Path({ { 0.f, 0.f }, { 0.f, 2.f }, { 5.f, 2.f }, { 5.f, 8.f }, { -10.f, 8.f } }))
		std::cout << path << std::endl;

	//TestFactory::makeStateless("pathfinding", path, []() { return Path({ { 0.f, 0.f }, { 0.f, 2.f }, { 5.f, 2.f }, { 5.f, 8.f }, { -10.f, 8.f } }); }).evaluate(evaluator);
	evaluator.evaluate("pathfinding", Path({ { 0.f, 0.f }, { 0.f, 2.f }, { 5.f, 2.f }, { 5.f, 8.f }, { -10.f, 8.f } }), path);
}

std::ostream & operator<<(std::ostream & os, const Path & path)
{
	os << "[";
	for (uint32 i = 0, end = path.getPoints().size(); i < end; ++i)
		os << path.getPoints()[i] << (i < end - 1 ? "," : "");
	os << "]";
	return os;
}

bool operator==(const Path & lhs, const Path & rhs)
{
	if (lhs.getPoints().size() != rhs.getPoints().size())
		return false;

	for (uint32 i = 0, end = lhs.getPoints().size(); i != end; ++i)
		if (lhs.getPoints()[i] != rhs.getPoints()[i])
			return false;

	return true;
}