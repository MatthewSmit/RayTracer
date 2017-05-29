#include "InfinitePlane.h"

#include <limits>
#include "MathsHelper.h"

bool InfinitePlane::intersect(const Ray& ray, IntersectionResult& result) const
{
	result.distance = dot(this->position - ray.position, normal) / dot(ray.direction, normal);

	if (fabs(result.distance) < std::numeric_limits<float>::epsilon() || result.distance < 0)
		return false;

	result.point = ray.calculatePoint(result.distance);
	result.normal = normal;
	return true;
}

Ray InfinitePlane::handleRefraction(const vec4& direction, const vec4& hitPoint, const vec4& normal, float refractivity) const
{
	throw std::exception();
}

vec4 InfinitePlane::getTextureCoordinates(const vec4& hitPoint) const
{
	return hitPoint;
	//// Tangent vector
	//auto tangent = cross(normal, vec{ 0, 1, 0, 0 });
	//if (lengthSquared(tangent) == 0)
	//	tangent = cross(normal, vec{ 0, 0, 1, 0 });

	//tangent = normalise(tangent);

	////Calculate hypothenuse length
	//auto d = distance(hitPoint, position);

	////Calculate angle between hypothenuse and tangent vectors
	//auto alpha = acosf(dot(tangent, normalise(position - hitPoint)));

	//if (alpha > HALF_PI)
	//{
	//	tangent = -tangent;
	//	alpha = acosf(dot(tangent, normalise(position - hitPoint)));
	//}

	////Use pythagore theorem to calculate sides length
	//auto x = sinf(alpha) * d;
	//auto y = cosf(alpha) * d;
	//return vec{ x, y, 0, 0 };
}
