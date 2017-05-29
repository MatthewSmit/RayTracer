#include "RayTracer.h"

#include <GL/freeglut.h>

#include <chrono>
#include "JsonSceneLoader.h"

static RayTracer rayTracer{};
static GLuint texture{};

void display()
{
	const auto start = std::chrono::high_resolution_clock::now();
	rayTracer.rayTrace();
	const auto end = std::chrono::high_resolution_clock::now();
	const auto duration = std::chrono::duration_cast<std::chrono::duration<float>>(end - start).count();
	printf("Took %f seconds\n", duration);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rayTracer.getSize(), rayTracer.getSize(), 0, GL_RGB, GL_FLOAT, rayTracer.getPixels());

	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
	glTexCoord2f(0, 0);
	glVertex3f(-1, 1, 0);
	glTexCoord2f(1, 0);
	glVertex3f(1, 1, 0);
	glTexCoord2f(1, 1);
	glVertex3f(1, -1, 0);
	glTexCoord2f(0, 1);
	glVertex3f(-1, -1, 0);
	glEnd();

	glutPostRedisplay();
	glutSwapBuffers();
}

void initialise()
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glEnable(GL_TEXTURE_2D);
	glClearColor(0, 0, 0, 1);

	loadSceneJson(&rayTracer, "scene1.json");
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(1024, 1024);
	glutCreateWindow("Raytracer");

	glutDisplayFunc(display);
	initialise();

	glutMainLoop();
	return 0;
}