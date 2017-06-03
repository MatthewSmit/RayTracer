#include "Sphere.h"

#include <limits>
#include <math.h>
#include "MathsHelper.h"

bool Sphere::intersect(const Ray& ray, IntersectionResult& result) const
{
	const auto difference = ray.position - center;
	const auto b = dot(ray.direction, difference);
	const auto len = length(difference);
	const auto c = len * len - radius * radius;
	const auto discriminant = b * b - c;

	if (discriminant <= 0)
		return false;

	auto t1 = -b - sqrtf(discriminant);
	auto t2 = -b + sqrtf(discriminant);
	if (fabs(t1) < std::numeric_limits<float>::epsilon())
		t1 = -1;
	if (fabs(t2) < std::numeric_limits<float>::epsilon())
		t2 = -1;

	result.distance = t1 < t2 ? t1 : t2;
	if (result.distance < 0)
		return false;

	result.point = ray.calculatePoint(result.distance);
	result.normal = normalise(result.point - center);
	return true;
}

Ray Sphere::handleRefraction(const vec4& direction, const vec4& hitPoint, const vec4& normal, float refractivity) const
{
	auto insideDirection = refract(direction, normal, refractivity);
	auto secondHitPoint = hitPoint + insideDirection * farIntersect(hitPoint, insideDirection);
	auto outsideDirection = refract(insideDirection, -normalise(secondHitPoint - center), 1 / refractivity);
	return Ray{ secondHitPoint, outsideDirection };
}

vec4 Sphere::getTextureCoordinates(const vec4& hitPoint) const
{
	auto normal = normalise(center - hitPoint);
	auto phi = (atan2f(1 - normal.z, normal.x) + PI) / TWO_PI;
	auto theta = acosf(normal.y) / PI;
	return vec4{ phi, theta, 0, 0 };
}

float Sphere::farIntersect(const vec4& position, const vec4& direction) const
{
	auto vdif = position - center;
	auto b = dot(direction, vdif);
	auto len = length(vdif);
	auto c = len * len - radius * radius;
	auto delta = b * b - c;

	if (fabs(delta) < std::numeric_limits<float>::epsilon())
		return -1;
	if (delta < 0) return -1;

	auto t1 = -b - sqrtf(delta);
	auto t2 = -b + sqrtf(delta);
	if (fabs(t1) < std::numeric_limits<float>::epsilon())
	{
		if (t2 > 0) return t2;
		t1 = -1;
	}
	if (fabs(t2) < std::numeric_limits<float>::epsilon()) t2 = -1;

	return t1 > t2 ? t1 : t2;
}
