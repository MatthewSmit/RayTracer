#pragma once
#include "AlignedObject.h"
#include "Material.h"
#include "Ray.h"
#include "vec4.h"
#include <memory>

struct IntersectionResult
{
	vec4 point;
	vec4 normal;
	float distance;
};

class SceneObject : public AlignedObject
{
public:
	explicit SceneObject(std::unique_ptr<Material> material) :
		material{ move(material) }
	{
	}
	virtual ~SceneObject() = default;

	virtual bool intersect(const Ray& ray, IntersectionResult& result) const = 0;
	virtual Ray handleRefraction(const vec4& direction, const vec4& hitPoint, const vec4& normal, float refractivity) const = 0;

	virtual vec4 getTextureCoordinates(const vec4& hitPoint) const = 0;

	const Material* getMaterial() const
	{
		return material.get();
	}

private:
	std::unique_ptr<Material> material{};
};
