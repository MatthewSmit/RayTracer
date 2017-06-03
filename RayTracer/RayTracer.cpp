#include "RayTracer.h"

#include "mat4.h"

#include "Image.h"
#include "MathsHelper.h"
#include "SceneObject.h"

#include <cassert>
#include <memory>
#include <future>

#if defined(_MSC_VER)
#include <SOIL.h>
#else
#include <SOIL/SOIL.h>
#endif

static constexpr int DEFAULT_SIZE = 512;

static constexpr float WIDTH = 20.0f;
static constexpr float HEIGHT = 20.0f;
static constexpr float EDIST = 40.0f;
static constexpr float XMIN = -WIDTH * 0.5f;
static constexpr float XMAX = WIDTH * 0.5f;
static constexpr float YMIN = -HEIGHT * 0.5f;
static constexpr float YMAX = HEIGHT * 0.5f;
static constexpr int THREADS = 4;

RayTracer::RayTracer()
{
	setSize(DEFAULT_SIZE);
	antiAliasing =
	{
		AntiAliasingMode::None,
		2
	};

	/*running = true;
	threads.resize(THREADS);
	for (auto i = 0; i < THREADS; i++)
		threads[i] = std::thread{ threadFunc, this };*/
}

RayTracer::~RayTracer()
{
	/*running = false;
	for (auto i = 0; i < THREADS; i++)
	{
		threads[i].join();
	}*/
}

//void RayTracer::startRayTrace()
//{
//	createTasks();
//
//	taskPtr = 0;
//	tasksDone = 0;
//
//	{
//		std::lock_guard<std::mutex> lock(mutex);
//		workToDo = true;
//	}
//
//	workConditionVariable.notify_all();
//}

void RayTracer::rayTrace()
{
	createTasks();

	switch (antiAliasing.mode)
	{
	case AntiAliasingMode::None:
		for (const auto& task : tasks)
			rayTrace(task);
		break;
	case AntiAliasingMode::Regular:
		for (const auto& task : tasks)
			rayTraceRegularAA(task);
		break;
	default:
		assert(0);
		break;
	}
}

void RayTracer::add(std::unique_ptr<SceneObject> object)
{
	sceneObjects.push_back(move(object));
}

void RayTracer::addDirectionLight(const vec4& direction, const vec4& colour)
{
	lights.push_back(createDirectionLight(normalise(direction), colour));
}

void RayTracer::addPointLight(const vec4& position, const vec4& colour, float attenuation[3])
{
	lights.push_back(createPointLight(position, colour, attenuation));
}

void RayTracer::clear()
{
	sceneObjects.clear();
}

Image* RayTracer::loadTexture(const char* path)
{
	auto image = Image::loadTexture(path);
	images.push_back(std::unique_ptr<Image>{image});
	return image;
}

void RayTracer::saveBmp(const char* fileName) const
{
	auto data = std::unique_ptr<uint8_t[]>{ new uint8_t[size * size * 3] };

	for (auto x = 0; x < size; x++)
	{
		for (auto y = 0; y < size; y++)
		{
			const auto r = static_cast<uint8_t>(pixelData[(x + y * size) * 3 + 0] * 255);
			const auto g = static_cast<uint8_t>(pixelData[(x + y * size) * 3 + 1] * 255);
			const auto b = static_cast<uint8_t>(pixelData[(x + y * size) * 3 + 2] * 255);

			data[(x + y * size) * 3 + 0] = r;
			data[(x + y * size) * 3 + 1] = g;
			data[(x + y * size) * 3 + 2] = b;
		}
	}

	auto saveResult = SOIL_save_image
	(
		fileName,
		SOIL_SAVE_TYPE_BMP,
		size, size, 3, data.get()
	);
	assert(saveResult == 1);
}

void RayTracer::setAntiAliasing(const AntiAliasingController& value)
{
	// Wait for raytrace to be done
	//while (!isRayTraceDone())
	//{
	//}

	antiAliasing = value;
}

void RayTracer::setCamera(const Camera& value)
{
	camera = value;
	auto cameraMatrix = lookAtLH(vec4{}, camera.direction, camera.up);
	this->cameraMatrix = inverseTranspose(cameraMatrix);
}

void RayTracer::setSize(int size)
{
	// Wait for raytrace to be done
	//while (!isRayTraceDone())
	//{
	//}

	this->size = size;
	cellWidth = (XMAX - XMIN) / size;
	cellHeight = (YMAX - YMIN) / size;
	pixelData = std::unique_ptr<float[]>{ new float[size * size * 3] };
}

//Finds the closest point of intersection of the current ray with scene objects
bool RayTracer::closestPoint(const Ray& ray, IntersectionResult& result, SceneObject*& hitObject, SceneObject* selfObject) const
{
	hitObject = nullptr;

	result.distance = 1.0e+6;
	for (auto& object : sceneObjects)
	{
		if (object.get() == selfObject)
			continue;

		IntersectionResult tmpResult;
		const auto hit = object->intersect(ray, tmpResult);
		if (hit)
		{
			assert(tmpResult.distance >= 0);
			if (tmpResult.distance < result.distance)
			{
				result.distance = tmpResult.distance;
				result.point = tmpResult.point;
				result.normal = tmpResult.normal;
				hitObject = object.get();
			}
		}
	}

	return hitObject != nullptr;
}

void RayTracer::createTasks()
{
	tasks.resize(size);

	for (auto y = 0; y < size; y++)
	{
		tasks[y].y = y;
		tasks[y].pixels = pixelData.get() + y * size * 3;
	}

	for (auto i = 0; i < size * size * 3; i++)
		pixelData[i] = 0;
}

void RayTracer::rayTrace(const Task& task) const
{
	const auto yp = YMAX - task.y * cellHeight;

	for (auto x = 0; x < size; x++)
	{
		const auto xp = XMIN + x * cellWidth;

		auto direction = vec4{ -(xp + 0.5f * cellWidth), yp + 0.5f * cellHeight, EDIST, 0 };	//direction of the primary ray
		direction = cameraMatrix * direction;

		const auto ray = Ray{ camera.position, normalise(direction) };
		const auto colour = trace(ray, nullptr, maximumSteps); //Trace the primary ray and get the colour value

		task.pixels[x * 3 + 0] = colour.x;
		task.pixels[x * 3 + 1] = colour.y;
		task.pixels[x * 3 + 2] = colour.z;
	}
}

void RayTracer::rayTraceRegularAA(const Task& task) const
{
	auto divisions = antiAliasing.sampleDivision;
	auto divisions2 = divisions * divisions;

	auto halfWidth = cellWidth * 0.5f;
	auto halfHeight = cellHeight * 0.5f;
	auto segments = divisions * 2 + 1;
	auto widthAdvance = cellWidth / segments;
	auto heightAdvance = cellHeight / segments;

	const auto yp = YMAX - task.y * cellHeight;

	auto ray = Ray{ camera.position, vec4{} };

	for (auto x = 0; x < size; x++)
	{
		const auto xp = XMIN + x * cellWidth;

		auto colour = vec4{};
		for (auto ay = 0; ay < divisions; ay++)
		{
			auto heightAddition = -halfHeight + heightAdvance * (ay * 2 + 1);
			for (auto ax = 0; ax < divisions; ax++)
			{
				auto widthAddition = -halfWidth + widthAdvance * (ax * 2 + 1);
				const auto direction = vec4{ -(xp + 0.5f * cellWidth + widthAddition), yp + 0.5f * cellHeight + heightAddition, EDIST, 0 };
				ray.direction = normalise(cameraMatrix * direction);
				colour += trace(ray, nullptr, maximumSteps); //Trace the primary ray and get the colour value
			}
		}

		colour /= divisions2;

		task.pixels[x * 3 + 0] = colour.x;
		task.pixels[x * 3 + 1] = colour.y;
		task.pixels[x * 3 + 2] = colour.z;
	}
}

//void RayTracer::threadFunc(RayTracer* rayTracer)
//{
//	while (rayTracer->running)
//	{
//		{
//			std::unique_lock<std::mutex> lock(rayTracer->mutex);
//			rayTracer->workConditionVariable.wait(lock, [rayTracer] {return rayTracer->workToDo; });
//		}
//		
//		while (true)
//		{
//			auto nextTask = rayTracer->taskPtr++;
//			if (nextTask >= rayTracer->tasks.size())
//				break;
//
//			const auto& task = rayTracer->tasks[nextTask];
//
//			if (rayTracer->antiAliasing)
//				rayTracer->rayTraceAA(task);
//			else rayTracer->rayTraceNormal(task);
//
//			++rayTracer->tasksDone;
//		}
//	}
//}

vec4 RayTracer::calculateShadows(const Ray& lightRay, SceneObject* selfObject, int step) const
{
	IntersectionResult result{};
	SceneObject* hitObject;
	auto newLightRay = lightRay;

	vec4 allowedLight{ 1 };
	while (closestPoint(newLightRay, result, hitObject, selfObject))
	{
		if (step < 0)
			return allowedLight;
		step--;

		const auto material = hitObject->getMaterial();
		if (material->isTransparent)
		{
			const auto colour = material->getColour(result.point, hitObject);
			if (colour.w > 0)
				allowedLight *= colour / colour.w * (1 - colour.w);

			newLightRay.position = result.point;
			selfObject = hitObject;
		}
		else return vec4{ 0 };
	}

	return allowedLight;
}

vec4 RayTracer::trace(const Ray& ray, SceneObject* selfObject, int step) const
{
	if (step == 0)
		return backgroundColour;

	// Cast a ray
	IntersectionResult result{};
	SceneObject* hitObject;
	if (!closestPoint(ray, result, hitObject, selfObject))
		return backgroundColour;

	const auto material = hitObject->getMaterial();
	const auto colour = material->getColour(result.point, hitObject);

	const auto ambientResult = ambientColour * colour;

	auto intensity = ambientResult;

	for (const auto& light : lights)
	{
		switch (light.type)
		{
		case LightType::Direction:
		{
			const Ray lightRay{ result.point, -light.direction.direction };
			const auto shadowLevel = calculateShadows(lightRay, hitObject, step - 1);

			const auto diffuseResult = saturate(dot(-light.direction.direction, result.normal)) * light.direction.colour * colour;

			const auto reflectionVector = reflect(light.direction.direction, result.normal);
			float specularResult;
			if (material->specularity == 0)
				specularResult = 0;
			else specularResult = powf(std::max(dot(reflectionVector, -ray.direction), 0.0f), material->specularity);

			intensity += (diffuseResult + specularResult) * shadowLevel;
			break;
		}
		case LightType::Point:
		{
			const auto difference = light.point.position - result.point;
			const auto distance = length(difference);
			const auto direction = normalise(difference);
			const Ray lightRay{ result.point, direction };
			const auto shadowLevel = calculateShadows(lightRay, hitObject, step - 1);

			const auto attenuation = light.point.attenuation[0] + light.point.attenuation[1] * distance + light.point.attenuation[2] * distance * distance;
			const auto diffuseResult = saturate(dot(direction, result.normal)) * light.point.colour * colour / attenuation;

			const auto reflectionVector = reflect(-direction, result.normal);
			float specularResult;
			if (material->specularity == 0)
				specularResult = 0;
			else specularResult = powf(std::max(dot(reflectionVector, -ray.direction), 0.0f), material->specularity);

			intensity += (diffuseResult + specularResult) * shadowLevel;
			break;
		}
		default:
			assert(0);
			break;
		}
	}

	if (material->reflectivity > 0)
	{
		const Ray reflectionRay{ result.point, reflect(ray.direction, result.normal) };
		const auto reflectionColour = trace(reflectionRay, hitObject, step - 1);
		intensity += reflectionColour * material->reflectivity;
	}

	const auto refractivity = material->refractivity;
	if (refractivity != 0)
	{
		Ray refractionRay;
		if (refractivity != 1)
		{
			refractionRay = hitObject->handleRefraction(ray.direction, result.point, result.normal, refractivity);
		}
		else
		{
			refractionRay = Ray{ result.point, ray.direction };
		}

		const auto refractionColour = trace(refractionRay, hitObject, step - 1) * (1 - colour.w);
		intensity = colour + refractionColour;
	}

	return saturate(intensity);
}
