#include <iostream>
#include <vector>
#include <sstream>
#include <cstdio>
#include <fstream>
#include "violet/core/entity/Entity.h"
#include "violet/core/math/Vec2.h"
#include "violet/core/utility/Time.h"
#include "violet/extras/serialization/JsonDeserializer.h"
#include "violet/plugins/graphics/Color.h"
#include "violet/plugins/graphics/system/RenderSystem.h"
#include <GL/glut.h>

using namespace Violet;

long int previousTime;
Entity e;

void Idle() {
	long int const currentTime = Time::getTimeInMilliseconds();
	float const deltaTime = (currentTime - previousTime)/1000.f;

	RenderSystem::update(deltaTime);

	previousTime = currentTime;
}

void printWarning(const char * msg)
{
	std::cout << msg << std::endl;
	char c;
	std::cin >> c;
	exit(1);
}

int main(int argc, char** argv) {
	Violet::RenderSystem::Settings renderSettings;
	renderSettings.argc = argc;
	renderSettings.argv = argv;
	renderSettings.x = renderSettings.y = 0;
	renderSettings.width = 800;
	renderSettings.height = 600;
	renderSettings.title = "Billiards";
	if (!Violet::RenderSystem::init(renderSettings))
		printWarning("Failed to initialize the RenderSystem");
		
	std::filebuf fb;
	if (!fb.open("square.json" ,std::ios::in))
		std::cout << "Could not open file square.json" << std::endl;

	std::istream is(&fb);
	JsonDeserializer deserializer(is);
	if (!deserializer)
		printWarning("Failed to parse square.json"); 
	Entity e;
	RenderSystem::create(e, deserializer);
	fb.close();

	previousTime = Time::getTimeInMilliseconds();

	glutIdleFunc(Idle);
	glutMainLoop();
}
