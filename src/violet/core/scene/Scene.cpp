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
	std::unique_ptr<Scene> scene(new Scene());
	bool succeeded = true;
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
			Entity & entity = scene->createEntity(entitySegment->getUint("id"));
			while (*entitySegment)
				ComponentFactory::getInstance().create(entitySegment->nextLabel(), entity, *entitySegment);
		}
	}

	return succeeded ? std::move(scene) : nullptr;
}

Scene::Scene() :
	m_entities(new std::vector<Entity>)
{
}

Entity & Scene::createEntity(uint32 id)
{
	m_entities->emplace_back(id);
	return m_entities->back();
}