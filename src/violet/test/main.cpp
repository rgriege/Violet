#include <iostream>
#include <vector>
#include <sstream>
#include <cstdio>
#include "violet/extras/serialization/JsonDeserializer.h"
#include "violet/test/serialization/JsonSerializationTest.h"

using namespace Violet;

int main(int argc, char** argv) {
	runJsonSerializationTests();
	
	std::cout << "Done." << std::endl;
	char c;
	std::cin >> c;
}
