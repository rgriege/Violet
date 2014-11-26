#include <iostream>
#include <vector>
#include <sstream>
#include <cstdio>
#include <fstream>
#include "violet/core/entity/Entity.h"
#include "violet/core/math/Vec2.h"
#include "violet/core/transform/TransformSystem.h"
#include "violet/core/utility/FormattedString.h"
#include "violet/core/utility/Time.h"
#include "violet/extras/serialization/JsonDeserializer.h"
#include "violet/plugins/graphics/Color.h"
#include "violet/plugins/graphics/system/RenderSystem.h"
#include "violet/plugins/physics/system/PhysicsSystem.h"
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

void createEntity(Entity & e, const char * filename)
{
	std::filebuf fb;
	if (!fb.open(filename, std::ios::in))
		std::cout << "Could not open file " << filename << std::endl;

	std::istream is(&fb);
	JsonDeserializer deserializer(is);
	if (!deserializer)
		printWarning(FormattedString<128>().sprintf("Failed to parse %s", filename));
	TransformSystem::create(e, deserializer);
	RenderSystem::create(e, deserializer);
	PhysicsSystem::create(e, deserializer);
	fb.close();
}

int main(int argc, char** argv) {
	if (!Violet::TransformSystem::init())
		printWarning("Failed to initialize the TransformSystem");

	Violet::RenderSystem::Settings renderSettings;
	renderSettings.argc = argc;
	renderSettings.argv = argv;
	renderSettings.x = renderSettings.y = 0;
	renderSettings.width = 800;
	renderSettings.height = 600;
	renderSettings.title = "Billiards";
	if (!Violet::RenderSystem::init(renderSettings))
		printWarning("Failed to initialize the RenderSystem");

	Violet::PhysicsSystem::Settings physicsSettings;
	physicsSettings.drag = 0.f;
	physicsSettings.gravity = Vec2f(0, -5);
	if (!Violet::PhysicsSystem::init(physicsSettings))
		printWarning("Failed to initialize the PhysicsSystem");

	createEntity(e1, "square.json");
	createEntity(e2, "square2.json");
	PhysicsSystem::fetch(e1).m_velocity += Vec2f(40, 0);

	previousTime = Time::getTimeInMilliseconds();

	glutIdleFunc(Idle);
	glutMainLoop();
}
