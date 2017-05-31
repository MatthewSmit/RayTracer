#include "StripedMaterial.h"

#include "SceneObject.h"

vec4 StripedMaterial::getColour(const vec4& hitPoint, const SceneObject* object) const
{
	auto textureCoordinates = object->getTextureCoordinates(hitPoint);
	auto coordinates = textureCoordinates * multiplier;
	auto part = coordinates % vec4{ 1 };
	if ((horizontal && part.y >= 0.5) || (!horizontal && part.x >= 0.5))
		return colour2;
	return colour1;
}
