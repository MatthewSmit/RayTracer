#pragma once
#include "Material.h"

class SolidMaterial final : public Material
{
public:
	SolidMaterial(const vec4& colour, float reflectivity, float refractivity, float specularity) :
		Material{ reflectivity, refractivity, specularity, colour.w != 1 },
		colour{colour}
	{
	}

	vec4 getColour(const vec4&, const SceneObject*) const override
	{
		return colour;
	}

private:
	vec4 colour;
};