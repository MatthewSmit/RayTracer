#pragma once
#include "vec4.h"

struct Ray
{
	vec4 position;
	vec4 direction;

	Ray()
	{
		position = vec4{};
		direction = vec4{ 0, 0, -1, 0 };
	}

	Ray(const vec4& point, const vec4& direction) :
		position(point),
		direction(direction)
	{
	}

	vec4 calculatePoint(float time) const
	{
		return position + direction * time;
	}
};