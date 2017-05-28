#pragma once

#include "vec4.h"

struct alignas(16) Light
{
	vec4 direction;
	vec4 colour;
};