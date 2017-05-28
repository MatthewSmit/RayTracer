#include "stdafx.h"
#include "Image.h"

Image::Image(int width, int height, std::unique_ptr<uint8_t[]>&& pixels) :
	width{ width },
	height{ height },
	pixels{ move(pixels) }
{
}

Image::~Image() = default;

vec4 Image::sample(const vec4& textureCoordinate) const
{
	auto x = textureCoordinate.x;
	auto y = textureCoordinate.y;

	x *= width;
	y *= height;

	auto ix = static_cast<int>(x);
	auto iy = static_cast<int>(y);
	auto ix1 = (ix + 1) % width;
	auto iy1 = (iy + 1) % height;

	auto fx = x - ix;
	auto fy = y - iy;

	auto tl = sample(ix, iy);
	return tl;
	auto bl = sample(ix1, iy);
	auto tr = sample(ix, iy1);
	auto br = sample(ix1, iy1);

	auto l = lerp(tl, bl, fy);
	auto r = lerp(tr, br, fy);
	return lerp(l, r, fx);
}

vec4 Image::sample(int x, int y) const
{
	auto pixel = &pixels[(x + y * width) * 4];
	auto colour = vec4
	{
		static_cast<float>(pixel[0]),
		static_cast<float>(pixel[1]),
		static_cast<float>(pixel[2]),
		static_cast<float>(pixel[3])
	};
	return colour / vec4{ 255.0f };
}
