#pragma once
#include "vec4.h"

struct alignas(16) mat4 : AlignedObject
{
private:
	vec4 value[4];
};