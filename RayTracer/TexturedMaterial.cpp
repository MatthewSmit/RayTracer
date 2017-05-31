#include "TexturedMaterial.h"

#include "Image.h"
#include "SceneObject.h"

vec4 TexturedMaterial::getColour(const vec4& hitPoint, const SceneObject* object) const
{
	auto textureCoordinates = object->getTextureCoordinates(hitPoint);
	textureCoordinates /= scaling;
	textureCoordinates = (textureCoordinates % vec4{ 1.0f } + vec4{ 1.0f }) % vec4 { 1.0f };
	return image->sample(textureCoordinates);
}