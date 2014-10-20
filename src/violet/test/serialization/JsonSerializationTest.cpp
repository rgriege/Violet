#include "violet/test/serialization/JsonSerializationTest.h"

#include "violet/core/utility/FormattedString.h"
#include "violet/extras/serialization/JsonDeserializer.h"
#include "violet/test/TestSuite.h"

#include <string>

using namespace Violet;

namespace JsonSerializationTestNamespace
{
	std::string const ms_emptyObject = "{}";
	std::string const ms_objectWrapper = "{%s}";
	std::string const ms_indexedInt = "\"int%d\":%d";
	std::string const ms_indexedFloat = "\"float%d\":%d";

	std::string testEmpty();
	std::string testInt();
	std::string testTwo();
	std::string testNested();

	Test ms_tests[] =
	{
		Test(&testEmpty, "empty"),
		Test(&testInt, "int"),
		Test(&testTwo, "two values"),
		Test(&testNested, "nested"),
	};
}

using namespace JsonSerializationTestNamespace;

void Violet::runJsonSerializationTests()
{
	TestSuite suite(std::vector<Test>(std::begin(ms_tests), std::end(ms_tests)), "Json Serialization");
	suite.run();
}

std::string JsonSerializationTestNamespace::testEmpty()
{
	std::stringstream ss;
	ss << ms_emptyObject;
	JsonDeserializer deserializer(ss);
	return deserializer ? "has a value" : "";
}

std::string JsonSerializationTestNamespace::testInt()
{
	int const num = 42;

	Json::Value root(Json::objectValue);
	root["int"] = num;
	std::stringstream ss;
	ss << root;

	JsonDeserializer deserializer(ss);
	if (!deserializer)
		return "has no values";
	else
	{
		int result = 0;
		deserializer >> result;
		if (result != num)
			return FormattedString<256>().sprintf("input %d doesn't match output %d", num, result);
		else if (deserializer)
			return "has too many values";
		else
			return "";
	}
}

std::string JsonSerializationTestNamespace::testTwo()
{
	int const num = 42;

	Json::Value root(Json::objectValue);
	root["int"] = num;
	root["int2"] = num + 1;
	std::stringstream ss;
	ss << root;

	JsonDeserializer deserializer(ss);
	if (!deserializer)
		return "has no values";
	else
	{
		int result = 0;
		deserializer >> result;
		if (result != num)
			return FormattedString<256>().sprintf("first input %d doesn't match output %d", num, result);

		deserializer >> result;
		if (result != num + 1)
			return FormattedString<256>().sprintf("second input %d doesn't match output %d", num, result);
		else if (deserializer)
			return "has too many values";
		else
			return "";
	}
}

std::string JsonSerializationTestNamespace::testNested()
{
	int const num = 42;

	Json::Value root(Json::objectValue);
	Json::Value nested(Json::objectValue);
	nested["int"] = num;
	root["object"] = nested;
	std::stringstream ss;
	ss << root;

	JsonDeserializer deserializer(ss);
	if (!deserializer)
		return "has no values";
	else
	{
		int result = 0;
		deserializer >> result;
		if (result != num)
			return FormattedString<256>().sprintf("input %d doesn't match output %d", num, result);
		else if (deserializer)
			return "has too many values";
		else
			return "";
	}
}