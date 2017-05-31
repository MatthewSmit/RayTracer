#pragma once
#include "Material.h"

class StripedMaterial final : public Material
{
public:
	StripedMaterial(bool horizontal, float multiplier, const vec4& colour1, const vec4& colour2, float reflectivity, float refractivity, float specularity) :
		Material{ reflectivity, refractivity, specularity, false },
		horizontal{ horizontal },
		multiplier{ multiplier },
		colour1{ colour1 },
		colour2{ colour2 }
	{
	}

	vec4 getColour(const vec4& hitPoint, const SceneObject* object) const override;

private:
	bool horizontal;
	float multiplier;
	vec4 colour1;
	vec4 colour2;
};