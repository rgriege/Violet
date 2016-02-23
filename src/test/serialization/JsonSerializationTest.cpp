#include "test/serialization/jsonserializationtest.h"

#include "violet/serialization/json/json_deserializer.h"
#include "test/core/testevaluator.h"
#include "test/core/testfactory.h"

#include <string>
#include <tuple>

using namespace vlt;

namespace JsonSerializationTestNamespace
{
	const int ms_firstNum = 42;
	const int ms_secondNum = 14;

	bool testEmpty();
	Json_Deserializer createForInt(int num);
	Json_Deserializer createForTwoInts(int firstNum, int secondNum);
	Json_Deserializer createForNested(int num);

	bool is_valid(Json_Deserializer & Deserializer);
	int deserializeInt(Json_Deserializer & Deserializer);
	int deserializeSecondInt(Json_Deserializer & Deserializer);
	int deserializeNestedInt(Json_Deserializer & Deserializer);
}

using namespace JsonSerializationTestNamespace;

void JsonSerializationTests::run(TestEvaluator & evaluator)
{
	TestFactory::makeStatelessSuite("json serialization", std::forward_as_tuple(
		TestFactory::makeStateless("empty object", false, &JsonSerializationTestNamespace::testEmpty),
		TestFactory::makeStatefulSuite("single int", std::move(createForInt(ms_firstNum)), std::forward_as_tuple(
			TestFactory::makeStateful<Json_Deserializer>("valid pre-parse", true, &JsonSerializationTestNamespace::is_valid),
			TestFactory::makeStateful<Json_Deserializer>("correct", ms_firstNum, &JsonSerializationTestNamespace::deserializeInt),
			TestFactory::makeStateful<Json_Deserializer>("finished", false, &JsonSerializationTestNamespace::is_valid)
		)),
		TestFactory::makeStatefulSuite("two ints", std::move(createForTwoInts(ms_firstNum, ms_secondNum)), std::forward_as_tuple(
			TestFactory::makeStateful<Json_Deserializer>("valid pre-parse", true, &JsonSerializationTestNamespace::is_valid),
			TestFactory::makeStateful<Json_Deserializer>("first value correct", ms_firstNum, &JsonSerializationTestNamespace::deserializeInt),
			TestFactory::makeStateful<Json_Deserializer>("second value correct", ms_secondNum, &JsonSerializationTestNamespace::deserializeSecondInt),
			TestFactory::makeStateful<Json_Deserializer>("finished", false, &JsonSerializationTestNamespace::is_valid)
		)),
		TestFactory::makeStatefulSuite("nested int", std::move(createForNested(ms_firstNum)), std::forward_as_tuple(
			TestFactory::makeStateful<Json_Deserializer>("valid pre-parse", true, &JsonSerializationTestNamespace::is_valid),
			TestFactory::makeStateful<Json_Deserializer>("correct", ms_firstNum, &JsonSerializationTestNamespace::deserializeNestedInt),
			TestFactory::makeStateful<Json_Deserializer>("finished", false, &JsonSerializationTestNamespace::is_valid)
		))
	)).evaluate(evaluator);
}

bool JsonSerializationTestNamespace::testEmpty()
{
	std::stringstream ss;
	ss << "{}";
	return Json_Deserializer(ss);
}

Json_Deserializer JsonSerializationTestNamespace::createForInt(const int num)
{
	Json::Value root(Json::objectValue);
	root.append("int", num);
	std::stringstream ss;
	ss << root;
	return Json_Deserializer(ss);
}

Json_Deserializer JsonSerializationTestNamespace::createForTwoInts(const int firstNum, const int secondNum)
{
	Json::Value root(Json::objectValue);
	root.append("int", firstNum);
	root.append("int2", secondNum);
	std::stringstream ss;
	ss << root;
	return Json_Deserializer(ss);
}

Json_Deserializer JsonSerializationTestNamespace::createForNested(const int num)
{
	Json::Value root(Json::objectValue);
	Json::Value nested(Json::objectValue);
	nested.append("int", num);
	root.append("object", nested);
	std::stringstream ss;
	ss << root;
	return Json_Deserializer(ss);
}

bool JsonSerializationTestNamespace::is_valid(Json_Deserializer & Deserializer)
{
	return Deserializer;
}

int JsonSerializationTestNamespace::deserializeInt(Json_Deserializer & Deserializer)
{
	return Deserializer.get_s32("int");
}

int JsonSerializationTestNamespace::deserializeSecondInt(Json_Deserializer & Deserializer)
{
	return Deserializer.get_s32("int2");
}

int JsonSerializationTestNamespace::deserializeNestedInt(Json_Deserializer & Deserializer)
{
	auto segment = Deserializer.enter_segment("object");
	int const result = segment->get_s32("int");
	return result;
}