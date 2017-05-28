#pragma once
#include <algorithm>

constexpr float PI = 3.14159265358979323846f;
constexpr float TWO_PI = 2 * PI;
constexpr float HALF_PI = PI / 2;

static float saturate(float value)
{
	return std::min(std::max(value, 0.0f), 1.0f);
}