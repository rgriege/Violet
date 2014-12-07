#include "violet/core/Engine.h"
#include "violet/core/serialization/FileDeserializerFactory.h"
#include "Setup.h"

#include <iostream>

int main(int argc, char** argv)
{
	auto factory = setup(argc, argv);

	auto deserializer = FileDeserializerFactory::getInstance().create("config.json");
	if (deserializer == nullptr || !*deserializer)
	{
		std::cout << "failed to read config file" << std::endl;
		exit(1);
	}

	auto engine = Violet::Engine::init(factory, *deserializer);
	if (engine == nullptr)
	{
		std::cout << "failed to init engine" << std::endl;
		exit(1);
	}

	engine->begin();
}
