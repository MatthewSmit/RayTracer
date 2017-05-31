#pragma once
#include "SceneObject.h"

class alignas(16) Sphere final : public SceneObject
{
public:
	Sphere(const vec4& center, float radius, std::unique_ptr<Material> material) :
		SceneObject{ move(material) },
		center(center),
		radius(radius)
	{
	}

	bool intersect(const Ray& ray, IntersectionResult& result) const override;
	Ray handleRefraction(const vec4& direction, const vec4& hitPoint, const vec4& normal, float refractivity) const override;

	vec4 getTextureCoordinates(const vec4& hitPoint) const override;

private:
	vec4 center;
	float radius;

	float farIntersect(const vec4& position, const vec4& direction) const;
};