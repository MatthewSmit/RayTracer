#include "SinMaterial.h"

#include "SceneObject.h"

vec4 SinMaterial::getColour(const vec4& hitPoint, const SceneObject* object) const
{
	auto textureCoordinates = object->getTextureCoordinates(hitPoint);
	auto value = 1 - 0.5f * (1 + sinf(textureCoordinates.x * textureCoordinates.x * textureCoordinates.y * textureCoordinates.y));
	return vec4{ value, value, value, 1 };
}
