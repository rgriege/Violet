#include <iostream>
#include <Windows.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

#include "violet\plugins\graphics\Window.h"

typedef unsigned uint32;

using namespace std;
using namespace Violet;

void init(int * argc, char** argv)
{
	glutInit(argc, argv);

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glutSwapBuffers();
	glutPostRedisplay();
}

void runGame()
{
	uint32 interval = 1000;
	glutInitWindowSize(200, 200);
	glutInitWindowPosition(100, 100);

	auto wnd = Window::create("Tutorial 01");

	glClearColor(0.f, 0.f, 0.f, 0.f);

	glutDisplayFunc(render);

	glutMainLoop();
}

void showDotProgress(uint32 milliseconds, uint32 dots)
{
	uint32 const step = milliseconds / dots;
	for (uint32 i = 0; i < dots; ++i)
	{
		Sleep(step);
		cout << '.';
	}
}

int main(int argc, char** argv)
{
	init (&argc, argv);

	cout << "Welcome to... Tap Dat Window" << endl;
	cout << "Click on the windows as they appear to score points.  If more than 3 are on the screen at once, you lose!" << endl;

	char c = 'a';
	while (c != 'q')
	{
		cout << "Enter q to quit or any other key + enter to being" << endl;
		cin >> c;
		if (c != 'q')
		{
			cout << "Starting in: ";
			for (int i = 3; i > 0; --i)
			{
				cout << i;
				showDotProgress(1000, 3);
			}

			runGame();
		}
	}

	return 0;
}