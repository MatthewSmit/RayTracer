#pragma once
#include "vec4.h"

#include <cassert>

struct alignas(16) mat4 : AlignedObject
{
private:
	vec4 value[4];

public:
	mat4()
	{
		value[0] = vec4{ 1, 0, 0, 0 };
		value[1] = vec4{ 0, 1, 0, 0 };
		value[2] = vec4{ 0, 0, 1, 0 };
		value[3] = vec4{ 0, 0, 0, 1 };
	}
	
	mat4(float x0, float y0, float z0, float w0,
		 float x1, float y1, float z1, float w1,
		 float x2, float y2, float z2, float w2,
		 float x3, float y3, float z3, float w3)
	{
		value[0] = vec4{ x0, y0, z0, w0 };
		value[1] = vec4{ x1, y1, z1, w1 };
		value[2] = vec4{ x2, y2, z2, w2 };
		value[3] = vec4{ x3, y3, z3, w3 };
	}

	const vec4& operator[](int index) const
	{
		assert(index >= 0 && index < 4);
		return value[index];
	}
	
	mat4& operator /=(float s)
	{
		value[0] /= s;
		value[1] /= s;
		value[2] /= s;
		value[3] /= s;
		return *this;
	}
};


inline vec4 operator *(const mat4& m, const vec4& v)
{
	auto Mul0 = m[0] * v.x;
	auto Mul1 = m[1] * v.y;
	auto Add0 = Mul0 + Mul1;
	auto Mul2 = m[2] * v.z;
	auto Mul3 = m[3] * v.w;
	auto Add1 = Mul2 + Mul3;
	return Add0 + Add1;
}

inline mat4 lookAtLH(const vec4& eye, const vec4& center, const vec4& up)
{
	auto f = normalise(center - eye);
	auto s = normalise(cross(up, f));
	auto u = cross(f, s);

	return mat4
	{
		s.x, u.x, f.x, 0,
		s.y, u.y, f.y, 0,
		s.z, u.z, f.z, 0,
		-dot(s, eye), -dot(u, eye), -dot(f, eye), 1
	};
}
inline mat4 inverseTranspose(const mat4& matrix)
{
	auto SubFactor00 = matrix[2][2] * matrix[3][3] - matrix[3][2] * matrix[2][3];
	auto SubFactor01 = matrix[2][1] * matrix[3][3] - matrix[3][1] * matrix[2][3];
	auto SubFactor02 = matrix[2][1] * matrix[3][2] - matrix[3][1] * matrix[2][2];
	auto SubFactor03 = matrix[2][0] * matrix[3][3] - matrix[3][0] * matrix[2][3];
	auto SubFactor04 = matrix[2][0] * matrix[3][2] - matrix[3][0] * matrix[2][2];
	auto SubFactor05 = matrix[2][0] * matrix[3][1] - matrix[3][0] * matrix[2][1];
	auto SubFactor06 = matrix[1][2] * matrix[3][3] - matrix[3][2] * matrix[1][3];
	auto SubFactor07 = matrix[1][1] * matrix[3][3] - matrix[3][1] * matrix[1][3];
	auto SubFactor08 = matrix[1][1] * matrix[3][2] - matrix[3][1] * matrix[1][2];
	auto SubFactor09 = matrix[1][0] * matrix[3][3] - matrix[3][0] * matrix[1][3];
	auto SubFactor10 = matrix[1][0] * matrix[3][2] - matrix[3][0] * matrix[1][2];
	auto SubFactor11 = matrix[1][1] * matrix[3][3] - matrix[3][1] * matrix[1][3];
	auto SubFactor12 = matrix[1][0] * matrix[3][1] - matrix[3][0] * matrix[1][1];
	auto SubFactor13 = matrix[1][2] * matrix[2][3] - matrix[2][2] * matrix[1][3];
	auto SubFactor14 = matrix[1][1] * matrix[2][3] - matrix[2][1] * matrix[1][3];
	auto SubFactor15 = matrix[1][1] * matrix[2][2] - matrix[2][1] * matrix[1][2];
	auto SubFactor16 = matrix[1][0] * matrix[2][3] - matrix[2][0] * matrix[1][3];
	auto SubFactor17 = matrix[1][0] * matrix[2][2] - matrix[2][0] * matrix[1][2];
	auto SubFactor18 = matrix[1][0] * matrix[2][1] - matrix[2][0] * matrix[1][1];

	auto inverse = mat4
	{
		+(matrix[1][1] * SubFactor00 - matrix[1][2] * SubFactor01 + matrix[1][3] * SubFactor02),
		-(matrix[1][0] * SubFactor00 - matrix[1][2] * SubFactor03 + matrix[1][3] * SubFactor04),
		+(matrix[1][0] * SubFactor01 - matrix[1][1] * SubFactor03 + matrix[1][3] * SubFactor05),
		-(matrix[1][0] * SubFactor02 - matrix[1][1] * SubFactor04 + matrix[1][2] * SubFactor05),

		-(matrix[0][1] * SubFactor00 - matrix[0][2] * SubFactor01 + matrix[0][3] * SubFactor02),
		+(matrix[0][0] * SubFactor00 - matrix[0][2] * SubFactor03 + matrix[0][3] * SubFactor04),
		-(matrix[0][0] * SubFactor01 - matrix[0][1] * SubFactor03 + matrix[0][3] * SubFactor05),
		+(matrix[0][0] * SubFactor02 - matrix[0][1] * SubFactor04 + matrix[0][2] * SubFactor05),

		+(matrix[0][1] * SubFactor06 - matrix[0][2] * SubFactor07 + matrix[0][3] * SubFactor08),
		-(matrix[0][0] * SubFactor06 - matrix[0][2] * SubFactor09 + matrix[0][3] * SubFactor10),
		+(matrix[0][0] * SubFactor11 - matrix[0][1] * SubFactor09 + matrix[0][3] * SubFactor12),
		-(matrix[0][0] * SubFactor08 - matrix[0][1] * SubFactor10 + matrix[0][2] * SubFactor12),

		-(matrix[0][1] * SubFactor13 - matrix[0][2] * SubFactor14 + matrix[0][3] * SubFactor15),
		+(matrix[0][0] * SubFactor13 - matrix[0][2] * SubFactor16 + matrix[0][3] * SubFactor17),
		-(matrix[0][0] * SubFactor14 - matrix[0][1] * SubFactor16 + matrix[0][3] * SubFactor18),
		+(matrix[0][0] * SubFactor15 - matrix[0][1] * SubFactor17 + matrix[0][2] * SubFactor18)
	};

	auto determinant = +matrix[0][0] * inverse[0][0] +
		matrix[0][1] * inverse[0][1] +
		matrix[0][2] * inverse[0][2] +
		matrix[0][3] * inverse[0][3];

	inverse /= determinant;

	return inverse;
}