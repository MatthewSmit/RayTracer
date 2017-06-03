#pragma once
#include "AntiAliasingController.h"
#include "Camera.h"
#include "Image.h"
#include "Light.h"
#include "mat4.h"
#include "SceneObject.h"

//#include <atomic>
#include <memory>
//#include <mutex>
//#include <thread>
#include <vector>

struct IntersectionResult;
struct Ray;

struct Task
{
	int y;
	float* pixels;
};

class RayTracer
{
public:
	RayTracer();
	~RayTracer();

	//void startRayTrace();
	void rayTrace();

	void add(std::unique_ptr<SceneObject> object);
	void addDirectionLight(const vec4& direction, const vec4& colour);
	void addPointLight(const vec4& position, const vec4& colour, float attenuation[3]);
	void clear();

	Image* loadTexture(const char* path);
	void saveBmp(const char* fileName) const;

	void setAmbientColour(const vec4& value) { ambientColour = value; }
	void setAntiAliasing(const AntiAliasingController& value);
	void setBackgroundColour(const vec4& value) { backgroundColour = value; }
	void setCamera(const Camera& value);
	void setSize(int size);

	/*bool isRayTraceDone() const
	{
		return tasksDone == tasks.size();
	}*/

	const float* getPixels() const { return pixelData.get(); }

	const AntiAliasingController& getAntiAliasing() const { return antiAliasing; }
	int getSize() const { return size; }

private:
	std::vector<std::unique_ptr<SceneObject>> sceneObjects{};
	std::vector<std::unique_ptr<Image>> images{};
	std::vector<Light> lights{};
	std::unique_ptr<float[]> pixelData{};
	std::vector<Task> tasks{};
	//std::vector<std::thread> threads{};

	Camera camera{};

	vec4 ambientColour;
	vec4 backgroundColour;

	AntiAliasingController antiAliasing{};
	int maximumSteps = 5;

	int size;
	float cellWidth;
	float cellHeight;

	mat4 cameraMatrix;

	/*std::mutex mutex{};
	bool running = true;
	bool workToDo = false;
	std::condition_variable workConditionVariable{};
	std::atomic_int taskPtr;
	std::atomic_int tasksDone;*/

	vec4 calculateShadows(const Ray& lightRay, SceneObject* selfObject, int step) const;
	vec4 trace(const Ray& ray, SceneObject* selfObject, int step) const;
	bool closestPoint(const Ray& ray, IntersectionResult& result, SceneObject*& hitObject, SceneObject* selfObject = nullptr) const;

	void createTasks();
	void rayTrace(const Task& task) const;
	void rayTraceRegularAA(const Task& task) const;

	//static void threadFunc(RayTracer* rayTracer);
};
