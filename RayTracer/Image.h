#pragma once
#include "vec4.h"

#include <memory>

class Image
{
public:
	Image(int width, int height, std::unique_ptr<uint8_t[]>&& pixels);
	~Image();

	vec4 sample(const vec4& textureCoordinate) const;
	vec4 sample(int x, int y) const;

private:
	int width;
	int height;
	std::unique_ptr<uint8_t[]> pixels;
};
