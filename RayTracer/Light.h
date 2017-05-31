#pragma once

#include "vec4.h"

enum class LightType
{
	Direction,
	Point
};

struct alignas(16) Light
{
	union
	{
		struct
		{
			vec4 direction;
			vec4 colour;
		} direction;
		struct
		{
			vec4 position;
			vec4 colour;
			float attenuation[3];
		} point;
	};

	LightType type;

	Light() :
		type{}
	{
	}
};

static Light createDirectionLight(const vec4& direction, const vec4& colour)
{
	Light light;
	light.direction.direction = direction;
	light.direction.colour = colour;
	light.type = LightType::Direction;
	return light;
}

static Light createPointLight(const vec4& position, const vec4& colour, float attenuation[3])
{
	Light light;
	light.point.position = position;
	light.point.colour = colour;
	light.point.attenuation[0] = attenuation[0];
	light.point.attenuation[1] = attenuation[1];
	light.point.attenuation[2] = attenuation[2];
	light.type = LightType::Point;
	return light;
}