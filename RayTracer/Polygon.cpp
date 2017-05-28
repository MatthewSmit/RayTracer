#include "Polygon.h"

float planeIntersection(const vec4& planePosition, const vec4& planeNormal, const Ray& ray)
{
	const auto d = dot(planePosition - ray.position, planeNormal) / dot(ray.direction, planeNormal);

	if (fabs(d) < std::numeric_limits<float>::epsilon())
		return -1;

	return d;
}

vec4 calculateNormal(const vec4& p0, const vec4& p1, const vec4& p2)
{
	return normalise(cross(p2 - p0, p1 - p0));
}

void calculateBarycentricCoordinates(const vec4& hitPoint, const vec4& p0, const vec4& p1, const vec4& p2, float& u, float& v)
{
	const auto v0 = p2 - p0;
	const auto v1 = p1 - p0;
	const auto v2 = hitPoint - p0;

	const auto dot00 = dot(v0, v0);
	const auto dot01 = dot(v0, v1);
	const auto dot02 = dot(v0, v2);
	const auto dot11 = dot(v1, v1);
	const auto dot12 = dot(v1, v2);

	// Compute barycentric coordinates
	const auto invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	u = dot11 * dot02 - dot01 * dot12;
	v = dot00 * dot12 - dot01 * dot02;
	u *= invDenom;
	v *= invDenom;
}
