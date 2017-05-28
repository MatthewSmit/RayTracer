#include "stdafx.h"
#include "Cylinder.h"

static const vec4 xzMask{ 1, 0, 1, 0 };

bool Cylinder::intersect(const Ray& ray, IntersectionResult& result) const
{
	auto xzDirection = normalise(ray.direction * xzMask);
	auto difference = (ray.position - bottomCenter) * xzMask;
	auto b = dot(xzDirection, difference);
	auto len = length(difference);
	auto c = len * len - radius * radius;
	auto delta = b * b - c;

	if (fabs(delta) < std::numeric_limits<float>::epsilon())
		return false;
	if (delta < 0)
		return false;

	auto t0 = -b - sqrtf(delta);
	auto t1 = -b + sqrtf(delta);

	if (fabs(t0) < std::numeric_limits<float>::epsilon())
		t0 = -1;
	if (fabs(t1) < std::numeric_limits<float>::epsilon())
		t1 = -1;

	if (t0 > t1)
		std::swap(t0, t1);

	auto hit0 = ray.calculatePoint(t0) - bottomCenter;
	auto hit1 = ray.calculatePoint(t1) - bottomCenter;

	auto y0 = hit0.y;
	auto y1 = hit1.y;

	if (y0 < 0)
	{
		if (y1 < 0)
			return false;

		// hit the cap
		result.distance = t0 + (t0 - t0) * (y0 + 1) / (y0 - y1);
		if (result.distance <= 0)
			return false;

		result.point = ray.calculatePoint(result.distance);
		result.normal = vec4{ 0, -1, 0, 0 };
		return true;
	}

	if (y0 >= 0 && y0 <= height)
	{
		// hit the cylinder
		if (t0 <= 0)
			return false;

		result.distance = t0;
		result.point = ray.calculatePoint(result.distance);
		result.normal = normalise((result.point - bottomCenter) * xzMask);
		return true;
	}

	if (y0 > height)
	{
		if (y1 > height)
			return false;

		// hit the cap
		result.distance = t0 + (t0 - t0) * (y0 - 1) / (y0 - y1);
		if (result.distance <= 0)
			return false;

		result.point = ray.calculatePoint(result.distance);
		result.normal = vec4{ 0, 1, 0, 0 };
		return true;
	}

	return false;
}

Ray Cylinder::handleRefraction(const vec4& direction, const vec4& hitPoint, const vec4& normal, float refractivity) const
{
	throw std::exception();
}

vec4 Cylinder::getTextureCoordinates(const vec4& hitPoint) const
{
	throw std::exception();
}
