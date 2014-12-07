#include "violet/core/scene/Scene.h"

#include "violet/core/component/ComponentFactory.h"
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
	bool succeeded = true;
	std::vector<Entity> entities;
	auto deserializer = FileDeserializerFactory::getInstance().create(filename);
	if (deserializer == nullptr)
	{
		std::cout << "Could not open scene file " << filename << std::endl;
		succeeded = false;
	}
	else if (!deserializer)
	{
		std::cout << "Failed to parse scene file " << filename << std::endl;
		succeeded = false;
	}
	else
	{
		while (*deserializer)
		{
			auto entitySegment = deserializer->enterSegment(ms_entityLabel);
			entities.emplace_back();
			while (*entitySegment)
				ComponentFactory::getInstance().create(entitySegment->nextLabel(), entities.back(), *entitySegment);
		}
	}

	return succeeded ? std::unique_ptr<Scene>(new Scene(std::move(entities))) : nullptr;
}

Scene::Scene(std::vector<Entity> && entities) :
	m_entities(std::move(entities))
{
}