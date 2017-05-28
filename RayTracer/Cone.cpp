#include "stdafx.h"
#include "Cone.h"

bool Cone::intersect(const Ray& ray, IntersectionResult& result) const
{
	auto axis = bottomCenter - vec4{ 0,-1, 0, 0 };
	auto theta = normalise(axis);
	auto m = pow(radius, 2) / pow(length(axis), 2);
	auto w = ray.position - vec4{ 0, -1, 0, 0 };

	auto a = dot(ray.direction, ray.direction) - m * pow(dot(ray.direction, theta), 2) - pow(dot(ray.direction, theta), 2);
	auto b = 2 * (dot(ray.direction, w) - m * dot(ray.direction, theta) * dot(w, theta) - dot(ray.direction, theta) * dot(w, theta));
	auto c = dot(w, w) - m * pow(dot(w, theta), 2) - pow(dot(w, theta), 2);

	auto discriminant = pow(b, 2) - 4 * a * c;

	if (discriminant < 0)
		return false;

	auto t1 = static_cast<float>((-b - sqrt(discriminant)) / (2 * a));
	auto t2 = static_cast<float>((-b + sqrt(discriminant)) / (2 * a));

	result.distance = t1 < t2 ? t1 : t2;
	if (result.distance < 0)
		return false;

	result.point = ray.calculatePoint(result.distance);
	result.normal = normalise(result.point - bottomCenter);
	return true;
}

Ray Cone::handleRefraction(const vec4& direction, const vec4& hitPoint, const vec4& normal, float refractivity) const
{
	throw std::exception();
}

vec4 Cone::getTextureCoordinates(const vec4& hitPoint) const
{
	throw std::exception();
}
