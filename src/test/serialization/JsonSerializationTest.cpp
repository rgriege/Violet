#include "test/serialization/JsonSerializationTest.h"

#include "violet/serialization/json/json_deserializer.h"
#include "test/core/TestEvaluator.h"
#include "test/core/TestFactory.h"

#include <string>
#include <tuple>

using namespace vlt;

namespace JsonSerializationTestNamespace
{
	const int ms_firstNum = 42;
	const int ms_secondNum = 14;

	bool testEmpty();
	json_deserializer createForInt(int num);
	json_deserializer createForTwoInts(int firstNum, int secondNum);
	json_deserializer createForNested(int num);

	bool is_valid(json_deserializer & deserializer);
	int deserializeInt(json_deserializer & deserializer);
	int deserializeSecondInt(json_deserializer & deserializer);
	int deserializeNestedInt(json_deserializer & deserializer);
}

using namespace JsonSerializationTestNamespace;

void JsonSerializationTests::run(TestEvaluator & evaluator)
{
	TestFactory::makeStatelessSuite("json serialization", std::forward_as_tuple(
		TestFactory::makeStateless("empty object", false, &JsonSerializationTestNamespace::testEmpty),
		TestFactory::makeStatefulSuite("single int", std::move(createForInt(ms_firstNum)), std::forward_as_tuple(
			TestFactory::makeStateful<json_deserializer>("valid pre-parse", true, &JsonSerializationTestNamespace::is_valid),
			TestFactory::makeStateful<json_deserializer>("correct", ms_firstNum, &JsonSerializationTestNamespace::deserializeInt),
			TestFactory::makeStateful<json_deserializer>("finished", false, &JsonSerializationTestNamespace::is_valid)
		)),
		TestFactory::makeStatefulSuite("two ints", std::move(createForTwoInts(ms_firstNum, ms_secondNum)), std::forward_as_tuple(
			TestFactory::makeStateful<json_deserializer>("valid pre-parse", true, &JsonSerializationTestNamespace::is_valid),
			TestFactory::makeStateful<json_deserializer>("first value correct", ms_firstNum, &JsonSerializationTestNamespace::deserializeInt),
			TestFactory::makeStateful<json_deserializer>("second value correct", ms_secondNum, &JsonSerializationTestNamespace::deserializeSecondInt),
			TestFactory::makeStateful<json_deserializer>("finished", false, &JsonSerializationTestNamespace::is_valid)
		)),
		TestFactory::makeStatefulSuite("nested int", std::move(createForNested(ms_firstNum)), std::forward_as_tuple(
			TestFactory::makeStateful<json_deserializer>("valid pre-parse", true, &JsonSerializationTestNamespace::is_valid),
			TestFactory::makeStateful<json_deserializer>("correct", ms_firstNum, &JsonSerializationTestNamespace::deserializeNestedInt),
			TestFactory::makeStateful<json_deserializer>("finished", false, &JsonSerializationTestNamespace::is_valid)
		))
	)).evaluate(evaluator);
}

bool JsonSerializationTestNamespace::testEmpty()
{
	std::stringstream ss;
	ss << "{}";
	return json_deserializer(ss);
}

json_deserializer JsonSerializationTestNamespace::createForInt(const int num)
{
	Json::Value root(Json::objectValue);
	root.append("int", num);
	std::stringstream ss;
	ss << root;
	return json_deserializer(ss);
}

json_deserializer JsonSerializationTestNamespace::createForTwoInts(const int firstNum, const int secondNum)
{
	Json::Value root(Json::objectValue);
	root.append("int", firstNum);
	root.append("int2", secondNum);
	std::stringstream ss;
	ss << root;
	return json_deserializer(ss);
}

json_deserializer JsonSerializationTestNamespace::createForNested(const int num)
{
	Json::Value root(Json::objectValue);
	Json::Value nested(Json::objectValue);
	nested.append("int", num);
	root.append("object", nested);
	std::stringstream ss;
	ss << root;
	return json_deserializer(ss);
}

bool JsonSerializationTestNamespace::is_valid(json_deserializer & deserializer)
{
	return deserializer;
}

int JsonSerializationTestNamespace::deserializeInt(json_deserializer & deserializer)
{
	return deserializer.get_s32("int");
}

int JsonSerializationTestNamespace::deserializeSecondInt(json_deserializer & deserializer)
{
	return deserializer.get_s32("int2");
}

int JsonSerializationTestNamespace::deserializeNestedInt(json_deserializer & deserializer)
{
	auto segment = deserializer.enter_segment("object");
	int const result = segment->get_s32("int");
	return result;
}