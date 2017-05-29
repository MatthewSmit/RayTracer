#pragma once
#include "Image.h"
#include "Light.h"
#include "SceneObject.h"

#include <memory>
#include <vector>

struct IntersectionResult;
struct Ray;

struct Task
{
	int x;
	int y;
	int stride;
	float* pixels;
};

class RayTracer
{
public:
	RayTracer();
	~RayTracer();

	void rayTrace() const;

	void add(std::unique_ptr<SceneObject> object);
	void addDirectionLight(const vec4& direction, const vec4& colour);
	void clear();

	Image* loadTexture(const char* path);
	void saveBmp(const char* fileName) const;

	void setAmbientColour(const vec4& colour) { ambientColour = colour; }
	void setBackgroundColour(const vec4& colour) { backgroundColour = colour; }

	const float* getPixels() const { return pixelData.get(); }

	int getSize() const;

private:
	std::vector<std::unique_ptr<SceneObject>> sceneObjects{};
	std::vector<std::unique_ptr<Image>> images{};
	std::vector<Light> lights{};
	std::unique_ptr<float[]> pixelData{};

	vec4 ambientColour;
	vec4 backgroundColour;

	bool antiAliasing = false;
	int maximumSteps = 5;

	vec4 calculateShadows(const Ray& lightRay, SceneObject* selfObject) const;
	vec4 trace(const Ray& ray, SceneObject* selfObject, int step) const;
	bool closestPoint(const Ray& ray, IntersectionResult& result, SceneObject*& hitObject, SceneObject* selfObject = nullptr) const;

	void rayTraceNormal(const std::vector<Task>& tasks) const;
	void rayTraceAA(const std::vector<Task>& tasks) const;
};
