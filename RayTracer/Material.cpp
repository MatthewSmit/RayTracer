#include "stdafx.h"
#include "Material.h"

#include "Image.h"
#include "SceneObject.h"

#include <cassert>

vec4 Material::getColour(const vec4& hitPoint, const SceneObject* object) const
{
	switch (Type)
	{
	case MaterialType::Solid:
		return SolidColour;
	case MaterialType::Texture:
	{
		auto textureCoordinates = object->getTextureCoordinates(hitPoint);
		return sampleTexture(textureCoordinates);
	}
	default:
		assert(false);
		return SolidColour;
	}
}

vec4 Material::sampleTexture(const vec4& textureCoordinates) const
{
	auto coordinates = textureCoordinates / Scaling;
	coordinates = (coordinates % vec4{ 1.0f } +vec4{ 1.0f }) % vec4 { 1.0f };
	return MaterialImage->sample(coordinates);
}
