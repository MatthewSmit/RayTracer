#pragma once
#include "vec4.h"

class SceneObject;

class Material
{
public:
	static constexpr float DEFAULT_SPECULAR = 50;

	float reflectivity;
	float refractivity;
	float specularity;

	bool isTransparent;

	Material(float reflectivity, float refractivity, float specularity, bool isTransparent) :
		reflectivity{ reflectivity },
		refractivity{ refractivity },
		specularity{ specularity },
		isTransparent{ isTransparent }
	{
	}
	virtual ~Material() = default;

	virtual vec4 getColour(const vec4& hitPoint, const SceneObject* object) const = 0;
};