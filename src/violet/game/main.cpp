#include "violet/core/component/ComponentFactory.h"
#include "violet/core/entity/Entity.h"
#include "violet/core/math/Vec2.h"
#include "violet/core/transform/TransformSystem.h"
#include "violet/core/utility/FormattedString.h"
#include "violet/core/utility/Time.h"
#include "violet/core/serialization/FileDeserializer.h"
#include "violet/extras/serialization/JsonDeserializer.h"
#include "violet/plugins/graphics/system/RenderSystem.h"
#include "violet/plugins/physics/system/PhysicsSystem.h"

#include <iostream>
#include <GL/glut.h>

using namespace Violet;

long int previousTime;
Entity e1;
Entity e2;

void Idle() {
	long int const currentTime = Time::getTimeInMilliseconds();
	float const deltaTime = (currentTime - previousTime)/1000.f;

	TransformSystem::update(deltaTime);
	RenderSystem::update(deltaTime);
	PhysicsSystem::update(deltaTime);

	previousTime = currentTime;
}

void printWarning(const char * msg)
{
	std::cout << msg << std::endl;
	char c;
	std::cin >> c;
	exit(1);
}

void createEntity(const char * filename, ComponentFactory & factory, Entity & entity)
{
	auto deserializer = FileDeserializer<JsonDeserializer>::create(filename);
	if (deserializer == nullptr)
		printWarning(FormattedString<128>().sprintf("Could not open file %s", filename));
	else if (!deserializer)
		printWarning(FormattedString<128>().sprintf("Failed to parse %s", filename));
	else
	{
		while (*deserializer)
			factory.create(entity, *deserializer);
	}
}

int main(int argc, char** argv) {
	ComponentFactory factory;

	if (!Violet::TransformSystem::init(factory))
		printWarning("Failed to initialize the TransformSystem");

	Violet::RenderSystem::Settings renderSettings;
	renderSettings.argc = argc;
	renderSettings.argv = argv;
	renderSettings.x = renderSettings.y = 0;
	renderSettings.width = 800;
	renderSettings.height = 600;
	renderSettings.title = "Billiards";
	if (!Violet::RenderSystem::init(factory, renderSettings))
		printWarning("Failed to initialize the RenderSystem");

	Violet::PhysicsSystem::Settings physicsSettings;
	physicsSettings.drag = 0.f;
	physicsSettings.gravity = Vec2f(0, -5);
	if (!Violet::PhysicsSystem::init(factory, physicsSettings))
		printWarning("Failed to initialize the PhysicsSystem");

	createEntity("square.json", factory, e1);
	createEntity("square2.json", factory, e2);
	PhysicsSystem::fetch(e1).m_velocity += Vec2f(40, 0);

	previousTime = Time::getTimeInMilliseconds();

	glutIdleFunc(Idle);
	glutMainLoop();
}
