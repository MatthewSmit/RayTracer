#pragma once
#include "SceneObject.h"

#include <array>

float planeIntersection(const vec4& planePosition, const vec4& planeNormal, const Ray& ray);
vec4 calculateNormal(const vec4& p0, const vec4& p1, const vec4& p2);
void calculateBarycentricCoordinates(const vec4& hitPoint, const vec4& p0, const vec4& p1, const vec4& p2, float& u, float& v);

inline bool sameSign(float lhs, float rhs)
{
	return ~(*reinterpret_cast<int*>(&lhs) ^ *reinterpret_cast<int*>(&rhs)) >> 31;
}

template<int size>
class alignas(16) Polygon final : public SceneObject
{
public:
	Polygon(const vec4* points, const vec4* texCoords, const Material& material)
	{
		memcpy(this->points, points, sizeof(vec4) * size);
		memcpy(this->texCoords, texCoords, sizeof(vec4) * size);
		normal = calculateNormal(points[0], points[1], points[2]);
		setMaterial(material);
	}

	bool intersect(const Ray& ray, IntersectionResult& result) const override
	{
		result.distance = planeIntersection(points[0], normal, ray);
		if (result.distance < 0)
			return false;

		result.point = ray.calculatePoint(result.distance);
		result.normal = normal;
		return inside(result.point);
	}
	Ray handleRefraction(const vec4& direction, const vec4& hitPoint, const vec4& normal, float refractivity) const override
	{
		throw std::exception();
	}

	vec4 getTextureCoordinates(const vec4& hitPoint) const override;

private:
	vec4 points[size];
	vec4 texCoords[size];
	vec4 normal;

	bool inside(const vec4& point) const;
};

template<>
inline bool Polygon<3>::inside(const vec4& point) const
{
	vec4 lines[]
	{
		points[1] - points[0],
		points[2] - points[1],
		points[0] - points[2],
	};
	vec4 values[]
	{
		point - points[0],
		point - points[1],
		point - points[2],
	};

	auto original = dot(cross(lines[0], values[0]), normal);

	auto result = dot(cross(lines[1], values[1]), normal);
	if (!sameSign(original, result))
		return false;

	result = dot(cross(lines[2], values[2]), normal);
	if (!sameSign(original, result))
		return false;

	return true;
}

template<>
inline bool Polygon<4>::inside(const vec4& point) const
{
	vec4 lines[]
	{
		points[1] - points[0],
		points[2] - points[1],
		points[3] - points[2],
		points[0] - points[3],
	};
	vec4 values[]
	{
		point - points[0],
		point - points[1],
		point - points[2],
		point - points[3],
	};

	const auto original = dot(cross(lines[0], values[0]), normal);

	auto result = dot(cross(lines[1], values[1]), normal);
	if (!sameSign(original, result))
		return false;

	result = dot(cross(lines[2], values[2]), normal);
	if (!sameSign(original, result))
		return false;

	result = dot(cross(lines[3], values[3]), normal);
	if (!sameSign(original, result))
		return false;

	return true;
}

template<int size>
bool Polygon<size>::inside(const vec4& point) const
{
	vec4 lines[size];
	vec4 values[size];

	for (auto i = 0; i < size; i++)
	{
		lines[i] = points[(i + 1) % size] - points[i];
		values[i] = point - points[i];
	}

	auto original = dot(cross(lines[0], values[0]), normal);

	for (auto i = 0; i < size; i++)
	{
		auto result = dot(cross(lines[i], values[i]), normal);
		if (!sameSign(original, result))
			return false;
	}

	return true;
}

template<>
inline vec4 Polygon<3>::getTextureCoordinates(const vec4& hitPoint) const
{
	float u, v;
	calculateBarycentricCoordinates(hitPoint, points[0], points[1], points[2], u, v);
	auto w = 1 - u - v;

	return u * texCoords[0] + v * texCoords[1] + w * texCoords[2];
}

template<>
inline vec4 Polygon<4>::getTextureCoordinates(const vec4& hitPoint) const
{
	float u1, v1;
	calculateBarycentricCoordinates(hitPoint, points[1], points[0], points[2], u1, v1);
	auto w1 = 1 - u1 - v1;

	if (u1 < 0 || v1 < 0 || w1 < 0)
	{
		float u2, v2;
		calculateBarycentricCoordinates(hitPoint, points[3], points[0], points[2], u2, v2);
		auto w2 = 1 - u2 - v2;

		return u2 * texCoords[2] + v2 * texCoords[0] + w2 * texCoords[3];
	}

	return u1 * texCoords[2] + v1 * texCoords[0] + w1 * texCoords[1];
}

template<int size>
vec4 Polygon<size>::getTextureCoordinates(const vec4& hitPoint) const
{
	throw std::exception();
}