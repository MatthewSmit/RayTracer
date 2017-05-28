#pragma once
#include "SceneObject.h"

class alignas(16) Torus final : public SceneObject
{
public:
	Torus(const vec4& position, float majorRadius, float minorRadius, const Material& material) :
		position(position),
		majorRadius{ majorRadius },
		minorRadius{ minorRadius }
	{
		setMaterial(material);
	}

	bool intersect(const Ray& ray, IntersectionResult& result) const override;
	Ray handleRefraction(const vec4& direction, const vec4& hitPoint, const vec4& normal, float refractivity) const override;

	vec4 getTextureCoordinates(const vec4& hitPoint) const override;


private:
	vec4 position;
	// R
	float majorRadius;
	// r
	float minorRadius;
};