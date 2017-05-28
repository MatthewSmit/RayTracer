#include "stdafx.h"
#include "RayTracer.h"

#include "Image.h"
#include "MathsHelper.h"
#include "SceneObject.h"

#if defined(_MSC_VER)
#include <SOIL.h>
#else
#include <SOIL/SOIL.h>
#endif

#include <cassert>
#include <memory>

RayTracer::RayTracer()
{
	pixelData = std::unique_ptr<float[]>{ new float[NUMDIV * NUMDIV * 3] };
}

RayTracer::~RayTracer() = default;

void RayTracer::rayTrace() const
{
	rayTraceNormal();
	//rayTraceAA();

	//saveBmp("dmp.bmp");
}

void RayTracer::add(std::unique_ptr<SceneObject> object)
{
	sceneObjects.push_back(move(object));
}

void RayTracer::addDirectionLight(const vec4& direction, const vec4& colour)
{
	lights.push_back(Light
	{
		normalise(direction),
		colour
	});
}

void RayTracer::clear()
{
	sceneObjects.clear();
}

Image* RayTracer::loadTexture(const char* path)
{
	int width;
	int height;
	int channels;
	auto pixels = SOIL_load_image(path, &width, &height, &channels, SOIL_LOAD_RGBA);
	if (pixels == nullptr)
	{
		printf("%s\n", SOIL_last_result());
		throw std::exception();
	}

	auto realPixels = std::unique_ptr<uint8_t[]>{ new uint8_t[width * height * 4] };
	auto scanLine = width * 4;
	for (auto i = 0; i < height; i++)
	{
		auto src = pixels + (height - 1 - i) * scanLine;
		auto dst = realPixels.get() + i * scanLine;
		memcpy(dst, src, scanLine);
	}

	auto image = new Image(width, height, move(realPixels));
	images.push_back(std::unique_ptr<Image>{image});
	return image;
}

void RayTracer::saveBmp(const char* fileName) const
{
	auto data = std::unique_ptr<uint8_t[]>{ new uint8_t[NUMDIV * NUMDIV * 3] };

	for (auto x = 0; x < NUMDIV; x++)
	{
		for (auto y = 0; y < NUMDIV; y++)
		{
			auto bmpY = NUMDIV - y - 1;
			auto r = static_cast<uint8_t>(pixelData[(x + y * NUMDIV) * 3 + 0] * 255);
			auto g = static_cast<uint8_t>(pixelData[(x + y * NUMDIV) * 3 + 1] * 255);
			auto b = static_cast<uint8_t>(pixelData[(x + y * NUMDIV) * 3 + 2] * 255);

			data[(x + bmpY * NUMDIV) * 3 + 0] = r;
			data[(x + bmpY * NUMDIV) * 3 + 1] = g;
			data[(x + bmpY * NUMDIV) * 3 + 2] = b;
		}
	}

	auto saveResult = SOIL_save_image
	(
		fileName,
		SOIL_SAVE_TYPE_BMP,
		NUMDIV, NUMDIV, 3, data.get()
	);
	assert(saveResult == 1);
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
		auto hit = object->intersect(ray, tmpResult);
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

void RayTracer::rayTraceNormal() const
{
	constexpr auto cellX = (XMAX - XMIN) / NUMDIV;  //cell width
	constexpr auto cellY = (YMAX - YMIN) / NUMDIV;  //cell height

	auto eye = vec4{ 0, 5, 0, 0 };  //The eye position (source of primary rays) is the origin

	for (auto x = 0; x < NUMDIV; x++)
	{
		auto xp = XMIN + x * cellX;
		for (auto y = 0; y < NUMDIV; y++)
		{
			auto yp = YMIN + y * cellY;

			auto direction = vec4{ xp + 0.5f * cellX, yp + 0.5f * cellY, -EDIST, 0 };	//direction of the primary ray

			auto ray = Ray{ eye, normalise(direction) };
			auto colour = trace(ray, nullptr, maximumSteps); //Trace the primary ray and get the colour value

			pixelData[(x + y * NUMDIV) * 3 + 0] = colour.x;
			pixelData[(x + y * NUMDIV) * 3 + 1] = colour.y;
			pixelData[(x + y * NUMDIV) * 3 + 2] = colour.z;
		}
	}
}

void RayTracer::rayTraceAA() const
{
	constexpr auto cellX = (XMAX - XMIN) / NUMDIV;  //cell width
	constexpr auto cellY = (YMAX - YMIN) / NUMDIV;  //cell height
	constexpr auto cellX4 = cellX / 4;
	constexpr auto cellY4 = cellY / 4;

	auto eye = vec4{ 0, 5, 0, 0 };  //The eye position (source of primary rays) is the origin

	for (auto x = 0; x < NUMDIV; x++)
	{
		auto xp = XMIN + x * cellX;
		for (auto y = 0; y < NUMDIV; y++)
		{
			auto yp = YMIN + y * cellY;

			//direction of the primary ray
			auto direction1 = vec4{ xp + 0.5f * cellX - cellX4, yp + 0.5f * cellY + cellY4, -EDIST, 0 };
			auto direction2 = vec4{ xp + 0.5f * cellX - cellX4, yp + 0.5f * cellY - cellY4, -EDIST, 0 };
			auto direction3 = vec4{ xp + 0.5f * cellX + cellX4, yp + 0.5f * cellY + cellY4, -EDIST, 0 };
			auto direction4 = vec4{ xp + 0.5f * cellX + cellX4, yp + 0.5f * cellY - cellY4, -EDIST, 0 };

			auto ray1 = Ray{ eye, normalise(direction1) };
			auto ray2 = Ray{ eye, normalise(direction2) };
			auto ray3 = Ray{ eye, normalise(direction3) };
			auto ray4 = Ray{ eye, normalise(direction4) };

			//Trace the primary ray and get the colour value
			auto colour1 = trace(ray1, nullptr, maximumSteps);
			auto colour2 = trace(ray2, nullptr, maximumSteps);
			auto colour3 = trace(ray3, nullptr, maximumSteps);
			auto colour4 = trace(ray4, nullptr, maximumSteps);

			auto colour = (colour1 + colour2 + colour3 + colour4) * 0.25f;

			pixelData[(x + y * NUMDIV) * 3 + 0] = colour.x;
			pixelData[(x + y * NUMDIV) * 3 + 1] = colour.y;
			pixelData[(x + y * NUMDIV) * 3 + 2] = colour.z;
		}
	}
}

vec4 RayTracer::calculateShadows(const Ray& lightRay, SceneObject* selfObject) const
{
	IntersectionResult result{};
	SceneObject* hitObject;
	auto newLightRay = lightRay;

	vec4 allowedLight{ 1 };
	while (closestPoint(newLightRay, result, hitObject, selfObject))
	{
		const auto& material = hitObject->getMaterial();
		if (material.IsTransparent)
		{
			auto colour = material.getColour(result.point, hitObject);
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

	const auto& material = hitObject->getMaterial();
	auto colour = material.getColour(result.point, hitObject);

	auto ambientResult = ambientColour * colour;

	auto intensity = ambientResult;

	for (auto light : lights)
	{
		Ray lightRay{ result.point, normalise(-light.direction) };
		auto shadowLevel = calculateShadows(lightRay, hitObject);

		auto diffuseResult = saturate(dot(-light.direction, result.normal)) * light.colour * colour;

		auto reflectionVector = reflect(light.direction, result.normal);
		float specularResult;
		if (material.Specularity == 0)
			specularResult = 0;
		else specularResult = pow(std::max(dot(reflectionVector, -ray.direction), 0.0f), material.Specularity);

		intensity += (diffuseResult + specularResult) * shadowLevel;
	}

	if (material.Reflectivity > 0)
	{
		Ray reflectionRay{ result.point, reflect(ray.direction, result.normal) };
		auto reflectionColour = trace(reflectionRay, hitObject, step - 1);
		intensity = colour + reflectionColour * material.Reflectivity;
	}

	auto refractivity = material.Refractivity;
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

		auto refractionColour = trace(refractionRay, hitObject, step - 1) * (1 - colour.w);
		intensity = colour + refractionColour;
	}

	return saturate(intensity);
}
