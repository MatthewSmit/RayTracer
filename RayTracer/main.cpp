#include "stdafx.h"

#include "RayTracer.h"

#include <GL/freeglut.h>

#include <chrono>
#include "JsonSceneLoader.h"

static RayTracer rayTracer{};

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	constexpr auto cellX = (XMAX - XMIN) / NUMDIV;  //cell width
	constexpr auto cellY = (YMAX - YMIN) / NUMDIV;  //cell height

	auto start = std::chrono::high_resolution_clock::now();
	rayTracer.rayTrace();
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::duration<float>>(end - start).count();
	printf("Took %f seconds\n", duration);

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a quad.

	for (auto x = 0; x < NUMDIV; x++)
	{
		auto xp = XMIN + x * cellX;
		for (auto y = 0; y < NUMDIV; y++)
		{
			auto yp = YMIN + y * cellY;

			glColor3fv(rayTracer.getPixels() + (x + y * NUMDIV) * 3);
			glVertex2f(xp, yp);				//Draw each cell with its colour value
			glVertex2f(xp + cellX, yp);
			glVertex2f(xp + cellX, yp + cellY);
			glVertex2f(xp, yp + cellY);
		}
	}

	glEnd();

	glutPostRedisplay();
	glutSwapBuffers();
}

void initialise()
{
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
	glClearColor(0, 0, 0, 1);

	loadSceneJson(&rayTracer, "scene1.json");
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(1000, 1000);
	glutCreateWindow("Raytracer");

	glutDisplayFunc(display);
	initialise();

	glutMainLoop();
	return 0;
}