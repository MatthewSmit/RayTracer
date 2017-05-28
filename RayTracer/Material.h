#pragma once

class Image;
class SceneObject;

constexpr float DEFAULT_SPECULAR = 50;

enum class MaterialType
{
	Solid,
	Patterned,
	Texture
};

struct Material
{
	MaterialType Type;

	union
	{
		vec4 SolidColour;
		struct
		{
			vec4 Scaling;
			Image* MaterialImage;
		};
	};
	float Reflectivity;
	float Refractivity;
	float Specularity;

	bool IsTransparent;

	explicit Material(const vec4& colour, float reflectivity = 0, float refractivity = 0, float specularity = DEFAULT_SPECULAR) :
		Type{ MaterialType::Solid },
		SolidColour{ colour },
		Reflectivity{ reflectivity },
		Refractivity{ refractivity },
		Specularity{ specularity }
	{
		IsTransparent = colour.w != 1;
	}

	explicit Material(Image* image, const vec4& scaling, float reflectivity = 0, float refractivity = 0, float specularity = DEFAULT_SPECULAR) :
		Type{ MaterialType::Texture },
		Scaling{ scaling },
		MaterialImage{ image },
		Reflectivity{ reflectivity },
		Refractivity{ refractivity },
		Specularity{ specularity }
	{
		IsTransparent = false;
	}

	vec4 getColour(const vec4& hitPoint, const SceneObject* object) const;
	vec4 sampleTexture(const vec4& textureCoordinates) const;
};