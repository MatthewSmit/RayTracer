#pragma once
#include "SceneObject.h"

class alignas(16) Cone final : public SceneObject
{
public:
	Cone(const vec4& bottomCenter, float radius, float height, const Material& material) :
		bottomCenter{bottomCenter},
		radius{ radius },
		height{ height }
	{
		setMaterial(material);
	}

	bool intersect(const Ray& ray, IntersectionResult& result) const override;
	Ray handleRefraction(const vec4& direction, const vec4& hitPoint, const vec4& normal, float refractivity) const override;

	vec4 getTextureCoordinates(const vec4& hitPoint) const override;


private:
	vec4 bottomCenter;
	float radius;
	float height;
};