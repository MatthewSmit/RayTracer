#pragma once
#include "Material.h"

class Image;

class TexturedMaterial final : public Material
{
public:
	TexturedMaterial(Image* image, const vec4& scaling, float reflectivity, float refractivity, float specularity) :
		Material{ reflectivity, refractivity, specularity, false },
		image{ image },
		scaling{ scaling }
	{
	}

	vec4 getColour(const vec4& hitPoint, const SceneObject* object) const override;

private:
	Image* image;
	vec4 scaling;
};