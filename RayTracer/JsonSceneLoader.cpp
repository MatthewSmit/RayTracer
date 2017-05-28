#include "JsonSceneLoader.h"

#include "Cone.h"
#include "Cylinder.h"
#include "InfinitePlane.h"
#include "Material.h"
#include "Polygon.h"
#include "RayTracer.h"
#include "Sphere.h"
#include "Torus.h"

#include <rapidjson/document.h>
#include <cstring>
#include <fstream>

namespace
{
	vec4 parseVector(const rapidjson::Value& value)
	{
		float x = 0;
		float y = 0;
		float z = 0;
		if (value.IsArray())
		{
			const auto number = value.Capacity();
			if (number > 3)
				throw std::exception();

			if (number > 0)
				x = static_cast<float>(value[0].GetDouble());

			if (number > 1)
				y = static_cast<float>(value[1].GetDouble());

			if (number > 2)
				z = static_cast<float>(value[2].GetDouble());
		}
		else if (value.IsObject())
		{
			if (value.HasMember("x"))
				x = static_cast<float>(value["x"].GetDouble());

			if (value.HasMember("y"))
				y = static_cast<float>(value["y"].GetDouble());

			if (value.HasMember("z"))
				z = static_cast<float>(value["z"].GetDouble());

		}
		else throw std::exception();

		return vec4{ x, y, z, 0 };
	}

	vec4 parseColour(const rapidjson::Value& value)
	{
		float r = 0;
		float g = 0;
		float b = 0;
		float a = 0;
		if (value.IsArray())
		{
			const auto number = value.Capacity();
			if (number > 4)
				throw std::exception();

			if (number > 0)
				r = static_cast<float>(value[0].GetDouble());

			if (number > 1)
				g = static_cast<float>(value[1].GetDouble());

			if (number > 2)
				b = static_cast<float>(value[2].GetDouble());

			if (number > 3)
				a = static_cast<float>(value[3].GetDouble());
		}
		else if (value.IsObject())
		{
			if (value.HasMember("r"))
				r = static_cast<float>(value["r"].GetDouble());

			if (value.HasMember("g"))
				g = static_cast<float>(value["g"].GetDouble());

			if (value.HasMember("b"))
				b = static_cast<float>(value["b"].GetDouble());

			if (value.HasMember("a"))
				a = static_cast<float>(value["a"].GetDouble());
		}
		else throw std::exception();

		return vec4{ r, g, b, a };
	}

	Material parseSolid(const rapidjson::Value& material)
	{
		auto colour = parseColour(material["colour"]);

		float reflectivity = 0;
		if (material.HasMember("reflectivity"))
			reflectivity = static_cast<float>(material["reflectivity"].GetDouble());

		float refractivity = 0;
		if (material.HasMember("refractivity"))
			refractivity = static_cast<float>(material["refractivity"].GetDouble());

		auto specularity = DEFAULT_SPECULAR;
		if (material.HasMember("specularity"))
			specularity = static_cast<float>(material["specularity"].GetDouble());

		return Material{ colour, reflectivity, refractivity, specularity };
	}

	Material parseTexture(RayTracer* rayTracer, const rapidjson::Value& material)
	{
		auto path = material["path"].GetString();

		auto scaling = vec4{ 1, 1, 0, 0 };
		if (material.HasMember("scaling"))
			scaling = parseVector(material["scaling"]);

		auto texture = rayTracer->loadTexture(path);

		float reflectivity = 0;
		if (material.HasMember("reflectivity"))
			reflectivity = static_cast<float>(material["reflectivity"].GetDouble());

		float refractivity = 0;
		if (material.HasMember("refractivity"))
			refractivity = static_cast<float>(material["refractivity"].GetDouble());

		auto specularity = DEFAULT_SPECULAR;
		if (material.HasMember("specularity"))
			specularity = static_cast<float>(material["specularity"].GetDouble());

		return Material{ texture, scaling, reflectivity, refractivity, specularity };
	}

	Material parseMaterial(RayTracer* rayTracer, const rapidjson::Value& material)
	{
		auto type = material["type"].GetString();

		if (strcmp(type, "solid") == 0)
			return parseSolid(material);
		if (strcmp(type, "texture") == 0)
			return parseTexture(rayTracer, material);

		throw std::exception();
	}

	void parseSphere(RayTracer* rayTracer, const rapidjson::Value& object)
	{
		auto position = parseVector(object["position"]);
		auto radius = static_cast<float>(object["radius"].GetDouble());
		auto material = parseMaterial(rayTracer, object["material"]);

		rayTracer->add(std::make_unique<Sphere>(position, radius, material));
	}

	void parsePlane(RayTracer* rayTracer, const rapidjson::Value& object)
	{
		auto position = parseVector(object["position"]);
		auto normal = parseVector(object["normal"]);
		auto material = parseMaterial(rayTracer, object["material"]);

		rayTracer->add(std::make_unique<InfinitePlane>(position, normal, material));
	}

	std::unique_ptr<SceneObject> createPolygon(vec4* points, vec4* texCoords, int size, const Material& material)
	{
		SceneObject* object = nullptr;
		switch (size)
		{
		case 3:
			object = new Polygon<3>(points, texCoords, material);
			break;
		case 4:
			object = new Polygon<4>(points, texCoords, material);
			break;
		default:
			throw std::exception();
		}

		return std::unique_ptr<SceneObject>{object};
	}

	void parsePolygon(RayTracer* rayTracer, const rapidjson::Value& object)
	{
		auto& jsonPoints = object["points"];
		const auto material = parseMaterial(rayTracer, object["material"]);

		const auto numberPoints = jsonPoints.Capacity();
		const auto points = std::unique_ptr<vec4[]>{ new vec4[numberPoints] };
		for (auto i = 0u; i < numberPoints; i++)
			points[i] = parseVector(jsonPoints[i]);

		const auto texCoords = std::unique_ptr<vec4[]>{ new vec4[numberPoints] };
		if (object.HasMember("texCoords"))
		{
			const auto& jsonTexCoords = object["texCoords"];
			if (numberPoints != jsonTexCoords.Capacity())
				throw std::exception();

			for (auto i = 0u; i < numberPoints; i++)
				texCoords[i] = parseVector(jsonTexCoords[i]);
		}

		rayTracer->add(createPolygon(points.get(), texCoords.get(), numberPoints, material));
	}

	void parseCylinder(RayTracer* rayTracer, const rapidjson::Value& object)
	{
		auto position = parseVector(object["position"]);
		auto radius = static_cast<float>(object["radius"].GetDouble());
		auto height = static_cast<float>(object["height"].GetDouble());
		auto material = parseMaterial(rayTracer, object["material"]);

		rayTracer->add(std::make_unique<Cylinder>(position, radius, height, material));
	}

	void parseCone(RayTracer* rayTracer, const rapidjson::Value& object)
	{
		auto position = parseVector(object["position"]);
		auto radius = static_cast<float>(object["radius"].GetDouble());
		auto height = static_cast<float>(object["height"].GetDouble());
		auto material = parseMaterial(rayTracer, object["material"]);

		rayTracer->add(std::make_unique<Cone>(position, radius, height, material));
	}

	void parseTorus(RayTracer* rayTracer, const rapidjson::Value& object)
	{
		auto position = parseVector(object["position"]);
		auto majorRadius = static_cast<float>(object["majorRadius"].GetDouble());
		auto minorRadius = static_cast<float>(object["minorRadius"].GetDouble());
		auto material = parseMaterial(rayTracer, object["material"]);

		rayTracer->add(std::make_unique<Torus>(position, majorRadius, minorRadius, material));
	}

	void parseObject(RayTracer* rayTracer, const rapidjson::Value& object)
	{
		auto type = object["type"].GetString();

		if (strcmp(type, "sphere") == 0)
			parseSphere(rayTracer, object);
		else if (strcmp(type, "plane") == 0)
			parsePlane(rayTracer, object);
		else if (strcmp(type, "polygon") == 0)
			parsePolygon(rayTracer, object);
		else if (strcmp(type, "cylinder") == 0)
			parseCylinder(rayTracer, object);
		else if (strcmp(type, "cone") == 0)
			parseCone(rayTracer, object);
		else if (strcmp(type, "torus") == 0)
			parseTorus(rayTracer, object);
		else
			throw std::exception();
	}

	void parseDirectionLight(RayTracer* rayTracer, const rapidjson::Value& object)
	{
		auto direction = parseVector(object["direction"]);
		auto colour = parseColour(object["colour"]);

		rayTracer->addDirectionLight(direction, colour);
	}

	void parseLight(RayTracer* rayTracer, const rapidjson::Value& object)
	{
		auto type = object["type"].GetString();

		if (strcmp(type, "direction") == 0)
			parseDirectionLight(rayTracer, object);
		else
			throw std::exception();
	}
}

void loadSceneJson(RayTracer* rayTracer, const char* fileName)
{
	rayTracer->clear();

	std::ifstream file{ fileName };

	if (!file)
		throw std::exception();

	std::string jsonString;

	file.seekg(0, std::ios::end);
	jsonString.reserve(file.tellg());
	file.seekg(0, std::ios::beg);

	jsonString.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

	rapidjson::Document json{};
	if (json.Parse(jsonString.c_str()).HasParseError())
	{
		throw std::exception();
	}

	const auto& objects = json["objects"];
	const auto& lights = json["lights"];
	const auto& ambientColour = json["ambientColour"];
	const auto& backgroundColour = json["backgroundColour"];

	for (auto i = objects.Begin(); i != objects.End(); i++)
		parseObject(rayTracer, *i);

	for (auto i = lights.Begin(); i != lights.End(); i++)
		parseLight(rayTracer, *i);

	rayTracer->setAmbientColour(parseColour(ambientColour));
	rayTracer->setBackgroundColour(parseColour(backgroundColour));
}
