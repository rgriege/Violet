#include "test/serialization/JsonSerializationTest.h"

#include "engine/serialization/json/JsonDeserializer.h"
#include "test/core/TestEvaluator.h"
#include "test/core/TestFactory.h"

#include <string>
#include <tuple>

using namespace Violet;

namespace JsonSerializationTestNamespace
{
	const int ms_firstNum = 42;
	const int ms_secondNum = 14;

	bool testEmpty();
	JsonDeserializer createForInt(int num);
	JsonDeserializer createForTwoInts(int firstNum, int secondNum);
	JsonDeserializer createForNested(int num);

	bool isValid(JsonDeserializer & deserializer);
	int deserializeInt(JsonDeserializer & deserializer);
	int deserializeSecondInt(JsonDeserializer & deserializer);
	int deserializeNestedInt(JsonDeserializer & deserializer);
}

using namespace JsonSerializationTestNamespace;

void JsonSerializationTests::run(TestEvaluator & evaluator)
{
	TestFactory::makeStatelessSuite("json serialization", std::forward_as_tuple(
		TestFactory::makeStateless("empty object", false, &JsonSerializationTestNamespace::testEmpty),
		TestFactory::makeStatefulSuite("single int", std::move(createForInt(ms_firstNum)), std::forward_as_tuple(
			TestFactory::makeStateful<JsonDeserializer>("valid pre-parse", true, &JsonSerializationTestNamespace::isValid),
			TestFactory::makeStateful<JsonDeserializer>("correct", ms_firstNum, &JsonSerializationTestNamespace::deserializeInt),
			TestFactory::makeStateful<JsonDeserializer>("finished", false, &JsonSerializationTestNamespace::isValid)
		)),
		TestFactory::makeStatefulSuite("two ints", std::move(createForTwoInts(ms_firstNum, ms_secondNum)), std::forward_as_tuple(
			TestFactory::makeStateful<JsonDeserializer>("valid pre-parse", true, &JsonSerializationTestNamespace::isValid),
			TestFactory::makeStateful<JsonDeserializer>("first value correct", ms_firstNum, &JsonSerializationTestNamespace::deserializeInt),
			TestFactory::makeStateful<JsonDeserializer>("second value correct", ms_secondNum, &JsonSerializationTestNamespace::deserializeSecondInt),
			TestFactory::makeStateful<JsonDeserializer>("finished", false, &JsonSerializationTestNamespace::isValid)
		)),
		TestFactory::makeStatefulSuite("nested int", std::move(createForNested(ms_firstNum)), std::forward_as_tuple(
			TestFactory::makeStateful<JsonDeserializer>("valid pre-parse", true, &JsonSerializationTestNamespace::isValid),
			TestFactory::makeStateful<JsonDeserializer>("correct", ms_firstNum, &JsonSerializationTestNamespace::deserializeNestedInt),
			TestFactory::makeStateful<JsonDeserializer>("finished", false, &JsonSerializationTestNamespace::isValid)
		))
	)).evaluate(evaluator);
}

bool JsonSerializationTestNamespace::testEmpty()
{
	std::stringstream ss;
	ss << "{}";
	return JsonDeserializer(ss);
}

JsonDeserializer JsonSerializationTestNamespace::createForInt(const int num)
{
	Json::Value root(Json::objectValue);
	root.append("int", num);
	std::stringstream ss;
	ss << root;
	return JsonDeserializer(ss);
}

JsonDeserializer JsonSerializationTestNamespace::createForTwoInts(const int firstNum, const int secondNum)
{
	Json::Value root(Json::objectValue);
	root.append("int", firstNum);
	root.append("int2", secondNum);
	std::stringstream ss;
	ss << root;
	return JsonDeserializer(ss);
}

JsonDeserializer JsonSerializationTestNamespace::createForNested(const int num)
{
	Json::Value root(Json::objectValue);
	Json::Value nested(Json::objectValue);
	nested.append("int", num);
	root.append("object", nested);
	std::stringstream ss;
	ss << root;
	return JsonDeserializer(ss);
}

bool JsonSerializationTestNamespace::isValid(JsonDeserializer & deserializer)
{
	return deserializer;
}

int JsonSerializationTestNamespace::deserializeInt(JsonDeserializer & deserializer)
{
	return deserializer.getInt("int");
}

int JsonSerializationTestNamespace::deserializeSecondInt(JsonDeserializer & deserializer)
{
	return deserializer.getInt("int2");
}

int JsonSerializationTestNamespace::deserializeNestedInt(JsonDeserializer & deserializer)
{
	auto segment = deserializer.enterSegment("object");
	int const result = segment->getInt("int");
	return result;
}