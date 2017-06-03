#include "RayTracer.h"

#include <SDL.h>

#if defined(_WIN32)
#include <Windows.h>
#endif
#include <GL/GL.h>

#include <chrono>
#include "JsonSceneLoader.h"

static RayTracer rayTracer{};
static GLuint texture{};

void renderString(float x, float y, const char* string)
{
	glColor3f(0.5f, 1, 0.5f);
	glRasterPos2f(x - 1, -(y - 1 + 0.05f));

	//glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, reinterpret_cast<const unsigned char*>(string));
}

void display()
{
	//if (rayTracer.isRayTraceDone())
	//	rayTracer.startRayTrace();
	const auto start = std::chrono::high_resolution_clock::now();
	rayTracer.rayTrace();
	const auto end = std::chrono::high_resolution_clock::now();
	const auto duration = std::chrono::duration_cast<std::chrono::duration<float>>(end - start).count();
	printf("Took %f seconds\n", duration);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rayTracer.getSize(), rayTracer.getSize(), 0, GL_RGB, GL_FLOAT, rayTracer.getPixels());

	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);
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

	char buffer[1024];
	snprintf(buffer, 1024, "Anti Aliasing (A): %s\nCurrent Size (-/+): %d",
			antiAliasingModeToString(rayTracer.getAntiAliasing().mode),
	         rayTracer.getSize());
	renderString(0.0f, 0.0f, buffer);
}

void onKey(SDL_Keycode key)
{
	if (key == SDLK_a)
	{
		auto antiAliasing = rayTracer.getAntiAliasing();
		antiAliasing.mode = static_cast<AntiAliasingMode>((static_cast<int>(antiAliasing.mode) + 1) % static_cast<int>(AntiAliasingMode::Last));
		rayTracer.setAntiAliasing(antiAliasing);
	}

	if (key == SDLK_s)
	{
		auto antiAliasing = rayTracer.getAntiAliasing();
		antiAliasing.sampleDivision = (antiAliasing.sampleDivision + 1) % 11;
		if (antiAliasing.sampleDivision < 2)
			antiAliasing.sampleDivision = 2;
		rayTracer.setAntiAliasing(antiAliasing);
	}

	if (key == SDLK_MINUS || key == SDLK_KP_MINUS)
	{
		auto size = rayTracer.getSize();
		size >>= 1;
		if (size < 16)
			size = 16;
		rayTracer.setSize(size);
	}

	if (key == SDLK_PLUS || key == SDLK_EQUALS || key == SDLK_KP_PLUS)
	{
		auto size = rayTracer.getSize();
		size <<= 1;
		if (size > 2048)
			size = 2048;
		rayTracer.setSize(size);
	}

	if (key == SDLK_PRINTSCREEN)
	{
		rayTracer.saveBmp("dmp.bmp");
	}
}

void initialise()
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glEnable(GL_TEXTURE_2D);
	glClearColor(0, 0, 0, 1);

	loadSceneJson(&rayTracer, "scene8.json");
}

int main(int, char*[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		exit(-1);
	}

	auto window = SDL_CreateWindow("Raytracer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 1024, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (window == nullptr)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		exit(-1);
	}

	auto glContext = SDL_GL_CreateContext(window);
	if (glContext == nullptr)
	{
		printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
		exit(-2);
	}

	initialise();

	auto quit = false;
	while (!quit)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
			else if (event.type == SDL_KEYDOWN)
			{
				onKey(event.key.keysym.sym);
			}
		}

		display();
		SDL_GL_SwapWindow(window);
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
