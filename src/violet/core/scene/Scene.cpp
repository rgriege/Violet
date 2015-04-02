#include "violet/core/scene/Scene.h"

#include "violet/core/component/ComponentFactory.h"
#include "violet/core/entity/EntityFactory.h"
#include "violet/core/serialization/FileDeserializerFactory.h"

#include <iostream>

using namespace Violet;

namespace SceneNamespace
{
	const char * ms_entityLabel = "ntty";
}

using namespace SceneNamespace;

std::unique_ptr<Scene> Scene::create(const char * filename)
{
	std::unique_ptr<Scene> scene(new Scene());
	bool succeeded = true;
	auto deserializer = FileDeserializerFactory::getInstance().create(filename);
	if (deserializer == nullptr)
	{
		std::cout << "Could not open scene file " << filename << std::endl;
		succeeded = false;
	}
	else if (!*deserializer)
	{
		std::cout << "Failed to parse scene file " << filename << std::endl;
		succeeded = false;
	}
	else
	{
		while (*deserializer)
			scene->m_entities.emplace_back(EntityFactory::getInstance().create(deserializer->nextLabel(), *deserializer));
	}

	return succeeded ? std::move(scene) : nullptr;
}

Scene::Scene() :
	m_entities()
{
}