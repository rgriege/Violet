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
/*bool mouse_down = false;
Vec2f mouse_start;
Vec2f mouse_cur;
bool level_complete = false;
int level_num = 1;
Level level;
GLfloat power_ratio = 0.75;
GLfloat drag_ratio = 0.5;*/

/*void draw_line(Vec2f v1, Vec2f v2) {
	glColor3f(0, 0, 0);
	glBegin(GL_LINES);
	glVertex2f(v1.x, v1.y);
	glVertex2f(v2.x, v2.y);
	glEnd();
}

void Display() {
	level.draw_objects();
	if (mouse_down) { 
		draw_line(mouse_start, mouse_cur);
		level.draw_aim_line(mouse_cur, mouse_start);
	}
}*/

/*void Mouse(int button, int button_mode, int x, int y) {
	if (!level_complete) {
		if (button == GLUT_LEFT_BUTTON) {
			if (button_mode == GLUT_DOWN) {
				mouse_start = Vec2f(x, window_height-y);
				mouse_cur = mouse_start;
				mouse_down = true;
			} else if (button_mode == GLUT_UP) {
				Vec2f impulse = mouse_start;
				impulse-=(Vec2f(x, window_height-y));
				impulse = power_ratio*impulse;
				level.apply_cue_impulse(impulse);
				mouse_down = false;
				if (!last_time.tv_sec)
					gettimeofday(&last_time, NULL);
			}
		}
	} else {
		level.setup(++level_num);
		mouse_start = Vec2f(x, window_height-y);
		level_complete = false;
	}
}

void Motion(int x, int y) {
	mouse_cur = Vec2f(x, window_height-y);
	glutPostRedisplay();
}

void Keyboard(unsigned char key, int xmouse, int ymouse) {
	if (key == 'r') {
		level.toggle_rainbows();
	}
}*/

void Idle() {
	long int const currentTime = Time::getTimeInMilliseconds();
	float const deltaTime = (currentTime - previousTime)/1000.f;

	RenderSystem::update(deltaTime);
	
	/*if (!level_complete) {
		timeval current_time;
		gettimeofday(&current_time, NULL);
		GLfloat time_diff = current_time.tv_sec + current_time.tv_usec/1000000.0 -
			(last_time.tv_sec + last_time.tv_usec/1000000.0);
		level.move_bodies(time_diff);
		last_time = current_time;
		level.resolve_collisions();
		if (level.goal_reached()) {
			stringstream ss;
			ss << "Level complete in " << level.get_num_moves() << " moves!";
			cout << ss.str() << endl;
			level_complete = true;
			last_time.tv_sec = 0;
		}
		glutPostRedisplay();
	}*/

	previousTime = currentTime;
}

void printWarning(const char * msg)
{
	std::cout << msg << std::endl;
	char c;
	std::cin >> c;
}

int main(int argc, char** argv) {
	Violet::RenderSystem::Settings settings;
	settings.argc = argc;
	settings.argv = argv;
	settings.x = settings.y = 0;
	settings.width = 800;
	settings.height = 600;
	settings.title = "Billiards";
	if (Violet::RenderSystem::init(settings))
	{
		std::filebuf fb;
		if (fb.open("square.json" ,std::ios::in)) {
			std::istream is(&fb);
			JsonDeserializer deserializer(is);
			if (deserializer)
			{
				Entity e;
				RenderSystem::create(e, deserializer);
				/*glutMouseFunc(Mouse);
				glutMotionFunc(Motion);
				glutKeyboardFunc(Keyboard);*/
				glutIdleFunc(Idle);

				glutMainLoop();

				previousTime = Time::getTimeInMilliseconds();
			}
			else
				printWarning("Failed to parse square.json"); 
			fb.close();
		}
		else
			std::cout << "Could not open file square.json" << std::endl;
	}
	else
		printWarning("Failed to initialize the RenderSystem");
	char c;
	std::cin >> c;
}
