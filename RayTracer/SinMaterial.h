#pragma once
#include "Material.h"

class SinMaterial final : public Material
{
public:
	SinMaterial(float reflectivity, float refractivity, float specularity) :
		Material{ reflectivity, refractivity, specularity, false }
	{
	}

	vec4 getColour(const vec4& hitPoint, const SceneObject* object) const override;
};