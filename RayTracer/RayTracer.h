#pragma once
#include "Light.h"

#include <memory>
#include <vector>

struct IntersectionResult;

static constexpr float WIDTH = 20.0;
static constexpr float HEIGHT = 20.0;
static constexpr float EDIST = 40.0;
static constexpr int NUMDIV = 500;
static constexpr int MAX_STEPS = 5;
static constexpr float XMIN = -WIDTH * 0.5;
static constexpr float XMAX = WIDTH * 0.5;
static constexpr float YMIN = -HEIGHT * 0.5;
static constexpr float YMAX = HEIGHT * 0.5;

class Image;
class SceneObject;

struct Ray;

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

private:
	std::vector<std::unique_ptr<SceneObject>> sceneObjects{};
	std::vector<std::unique_ptr<Image>> images{};
	std::vector<Light> lights{};
	std::unique_ptr<float[]> pixelData{};

	vec4 ambientColour;
	vec4 backgroundColour;

	int maximumSteps = 5;

	vec4 calculateShadows(const Ray& lightRay, SceneObject* selfObject) const;
	vec4 trace(const Ray& ray, SceneObject* selfObject, int step) const;
	bool closestPoint(const Ray& ray, IntersectionResult& result, SceneObject*& hitObject, SceneObject* selfObject = nullptr) const;

	void rayTraceNormal() const;
	void rayTraceAA() const;
};
