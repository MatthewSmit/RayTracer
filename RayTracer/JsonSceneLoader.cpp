#include "JsonSceneLoader.h"

#include "Camera.h"
#include "Cone.h"
#include "Cylinder.h"
#include "InfinitePlane.h"
#include "Material.h"
#include "Polygon.h"
#include "RayTracer.h"
#include "SolidMaterial.h"
#include "Sphere.h"
#include "SinMaterial.h"
#include "StripedMaterial.h"
#include "TexturedMaterial.h"
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
		float a = 1;
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

	std::unique_ptr<Material> parseSolid(const rapidjson::Value& material)
	{
		auto colour = parseColour(material["colour"]);

		float reflectivity = 0;
		if (material.HasMember("reflectivity"))
			reflectivity = static_cast<float>(material["reflectivity"].GetDouble());

		float refractivity = 0;
		if (material.HasMember("refractivity"))
			refractivity = static_cast<float>(material["refractivity"].GetDouble());

		auto specularity = Material::DEFAULT_SPECULAR;
		if (material.HasMember("specularity"))
			specularity = static_cast<float>(material["specularity"].GetDouble());

		return std::make_unique<SolidMaterial>(colour, reflectivity, refractivity, specularity);
	}

	std::unique_ptr<Material> parseTexture(RayTracer* rayTracer, const rapidjson::Value& material)
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

		auto specularity = Material::DEFAULT_SPECULAR;
		if (material.HasMember("specularity"))
			specularity = static_cast<float>(material["specularity"].GetDouble());

		return std::make_unique<TexturedMaterial>(texture, scaling, reflectivity, refractivity, specularity);
	}

	std::unique_ptr<Material> parsePatternStripe(RayTracer* rayTracer, const rapidjson::Value& material)
	{
		auto direction = material["direction"].GetString();

		bool horizontal;
		if (strcmp(direction, "horizontal") == 0)
			horizontal = true;
		else if (strcmp(direction, "vertical") == 0)
			horizontal = false;
		else throw std::exception();

		auto multiplier = static_cast<float>(material["multiplier"].GetDouble());
		auto colour1 = parseColour(material["colour1"]);
		auto colour2 = parseColour(material["colour2"]);

		float reflectivity = 0;
		if (material.HasMember("reflectivity"))
			reflectivity = static_cast<float>(material["reflectivity"].GetDouble());

		float refractivity = 0;
		if (material.HasMember("refractivity"))
			refractivity = static_cast<float>(material["refractivity"].GetDouble());

		auto specularity = Material::DEFAULT_SPECULAR;
		if (material.HasMember("specularity"))
			specularity = static_cast<float>(material["specularity"].GetDouble());

		return std::make_unique<StripedMaterial>(horizontal, multiplier, colour1, colour2, reflectivity, refractivity, specularity);
	}

	std::unique_ptr<Material> parsePatternSin(RayTracer* rayTracer, const rapidjson::Value& material)
	{
		float reflectivity = 0;
		if (material.HasMember("reflectivity"))
			reflectivity = static_cast<float>(material["reflectivity"].GetDouble());

		float refractivity = 0;
		if (material.HasMember("refractivity"))
			refractivity = static_cast<float>(material["refractivity"].GetDouble());

		auto specularity = Material::DEFAULT_SPECULAR;
		if (material.HasMember("specularity"))
			specularity = static_cast<float>(material["specularity"].GetDouble());

		return std::make_unique<SinMaterial>(reflectivity, refractivity, specularity);
	}

	std::unique_ptr<Material> parseMaterial(RayTracer* rayTracer, const rapidjson::Value& material)
	{
		auto type = material["type"].GetString();

		if (strcmp(type, "solid") == 0)
			return parseSolid(material);
		if (strcmp(type, "texture") == 0)
			return parseTexture(rayTracer, material);
		if (strcmp(type, "pattern-stripe") == 0)
			return parsePatternStripe(rayTracer, material);
		if (strcmp(type, "pattern-sin") == 0)
			return parsePatternSin(rayTracer, material);

		throw std::exception();
	}

	void parseSphere(RayTracer* rayTracer, const rapidjson::Value& object)
	{
		auto position = parseVector(object["position"]);
		auto radius = static_cast<float>(object["radius"].GetDouble());
		auto material = parseMaterial(rayTracer, object["material"]);

		rayTracer->add(std::make_unique<Sphere>(position, radius, move(material)));
	}

	void parsePlane(RayTracer* rayTracer, const rapidjson::Value& object)
	{
		auto position = parseVector(object["position"]);
		auto normal = parseVector(object["normal"]);
		auto material = parseMaterial(rayTracer, object["material"]);

		rayTracer->add(std::make_unique<InfinitePlane>(position, normal, move(material)));
	}

	std::unique_ptr<SceneObject> createPolygon(vec4* points, vec4* texCoords, int size, std::unique_ptr<Material> material)
	{
		SceneObject* object = nullptr;
		switch (size)
		{
		case 3:
			object = new Polygon<3>(points, texCoords, move(material));
			break;
		case 4:
			object = new Polygon<4>(points, texCoords, move(material));
			break;
		default:
			throw std::exception();
		}

		return std::unique_ptr<SceneObject>{object};
	}

	void parsePolygon(RayTracer* rayTracer, const rapidjson::Value& object)
	{
		auto& jsonPoints = object["points"];
		auto material = parseMaterial(rayTracer, object["material"]);

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

		rayTracer->add(createPolygon(points.get(), texCoords.get(), numberPoints, move(material)));
	}

	void parseCylinder(RayTracer* rayTracer, const rapidjson::Value& object)
	{
		auto position = parseVector(object["position"]);
		auto radius = static_cast<float>(object["radius"].GetDouble());
		auto height = static_cast<float>(object["height"].GetDouble());
		auto material = parseMaterial(rayTracer, object["material"]);

		rayTracer->add(std::make_unique<Cylinder>(position, radius, height, move(material)));
	}

	void parseCone(RayTracer* rayTracer, const rapidjson::Value& object)
	{
		auto position = parseVector(object["position"]);
		auto radius = static_cast<float>(object["radius"].GetDouble());
		auto height = static_cast<float>(object["height"].GetDouble());
		auto material = parseMaterial(rayTracer, object["material"]);

		rayTracer->add(std::make_unique<Cone>(position, radius, height, move(material)));
	}

	void parseTorus(RayTracer* rayTracer, const rapidjson::Value& object)
	{
		auto position = parseVector(object["position"]);
		auto majorRadius = static_cast<float>(object["majorRadius"].GetDouble());
		auto minorRadius = static_cast<float>(object["minorRadius"].GetDouble());
		auto material = parseMaterial(rayTracer, object["material"]);

		rayTracer->add(std::make_unique<Torus>(position, majorRadius, minorRadius, move(material)));
	}

	void parseCube(RayTracer* rayTracer, const rapidjson::Value& object)
	{
		auto position = parseVector(object["position"]);
		auto material = parseMaterial(rayTracer, object["material"]);

		throw std::exception();

		//rayTracer->add(std::make_unique<Torus>(position, majorRadius, minorRadius, move(material)));
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
		else if (strcmp(type, "cube") == 0)
			parseCube(rayTracer, object);
		else
			throw std::exception();
	}

	void parseDirectionLight(RayTracer* rayTracer, const rapidjson::Value& object)
	{
		auto direction = parseVector(object["direction"]);
		auto colour = parseColour(object["colour"]);

		rayTracer->addDirectionLight(direction, colour);
	}

	void parsePointLight(RayTracer* rayTracer, const rapidjson::Value& object)
	{
		auto position = parseVector(object["position"]);
		auto colour = parseColour(object["colour"]);
		float attenuation[3];
		attenuation[0] = static_cast<float>(object["attenuation-constant"].GetDouble());
		attenuation[1] = static_cast<float>(object["attenuation-linear"].GetDouble());
		attenuation[2] = static_cast<float>(object["attenuation-quadratic"].GetDouble());

		rayTracer->addPointLight(position, colour, attenuation);
	}

	void parseLight(RayTracer* rayTracer, const rapidjson::Value& object)
	{
		auto type = object["type"].GetString();

		if (strcmp(type, "direction") == 0)
			parseDirectionLight(rayTracer, object);
		else if (strcmp(type, "point") == 0)
			parsePointLight(rayTracer, object);
		else
			throw std::exception();
	}

	Camera parseCamera(const rapidjson::Value& object)
	{
		auto position = vec4{ 0, 0, 0, 0 };
		auto direction = vec4{ 0, 0, -1, 0 };
		auto up = vec4{ 0, 1, 0, 0 };

		if (object.HasMember("position"))
			position = parseVector(object["position"]);

		if (object.HasMember("direction"))
			direction = parseVector(object["direction"]);

		if (object.HasMember("up"))
			up = parseVector(object["up"]);

		return Camera{ position, direction, up };
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

	const auto& camera = json["camera"];
	const auto& objects = json["objects"];
	const auto& lights = json["lights"];
	const auto& ambientColour = json["ambientColour"];
	const auto& backgroundColour = json["backgroundColour"];

	rayTracer->setCamera(parseCamera(camera));

	for (auto i = objects.Begin(); i != objects.End(); i++)
		parseObject(rayTracer, *i);

	for (auto i = lights.Begin(); i != lights.End(); i++)
		parseLight(rayTracer, *i);

	rayTracer->setAmbientColour(parseColour(ambientColour));
	rayTracer->setBackgroundColour(parseColour(backgroundColour));
}
