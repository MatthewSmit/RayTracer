#pragma once

#include <algorithm>

//#define GLM

#ifdef GLM
//#define GLM_FORCE_SSE42
//#define GLM_FORCE_ALIGNED

#include <glm/glm.hpp>

using namespace glm;

typedef vec4 vec;

static vec normalise(vec x)
{
	return normalize(x);
}

static vec cross(vec lhs, vec rhs)
{
	return vec4{ glm::cross(vec3{ lhs }, vec3{ rhs }), 0 };
}

static vec saturate(vec value)
{
	return clamp(value, 0.0f, 1.0f);
}

static void getValues(vec v, float& r, float& g, float& b)
{
	r = v.r;
	g = v.g;
	b = v.b;
}

static float getAlpha(vec v)
{
	return v.a;
}

static vec lerp(vec lhs, vec rhs, float delta)
{
	auto tmp = -delta * lhs + lhs;
	return delta * rhs + tmp;
}

static float saturate(float value)
{
	return std::min(std::max(value, 0.0f), 1.0f);
}
#else
#include "vec4.h"
#endif