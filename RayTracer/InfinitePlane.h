#pragma once
#include "SceneObject.h"

class alignas(16) InfinitePlane final : public SceneObject
{
public:
	InfinitePlane(const vec4& position, const vec4& normal, const Material& material) :
		position(position),
		normal(normal)
	{
		setMaterial(material);
	}

	bool intersect(const Ray& ray, IntersectionResult& result) const override;
	Ray handleRefraction(const vec4& direction, const vec4& hitPoint, const vec4& normal, float refractivity) const override;

	vec4 getTextureCoordinates(const vec4& hitPoint) const override;


private:
	vec4 position;
	vec4 normal;
};