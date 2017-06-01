#pragma once
#include "AlignedObject.h"

#include <immintrin.h>
#include <xmmintrin.h>
#include <algorithm>
#include <cassert>
#include <cmath>

struct alignas(16) vec4 : AlignedObject
{
	vec4()
	{
		vector = _mm_setzero_ps();
	}

	explicit vec4(float value)
	{
		vector = _mm_set_ps1(value);
	}

	vec4(float x, float y, float z, float w)
	{
		vector = _mm_setr_ps(x, y, z, w);
	}

	vec4(const __m128& vector) :
		vector{ vector }
	{
	}

	vec4& operator +=(float x);
	vec4& operator +=(const vec4& x);
	vec4& operator -=(float x);
	vec4& operator -=(const vec4& x);
	vec4& operator *=(float x);
	vec4& operator *=(const vec4& x);
	vec4& operator /=(float x);
	vec4& operator /=(const vec4& x);

	operator __m128&()
	{
		return vector;
	}

	operator __m128() const
	{
		return vector;
	}

	float operator[](int index) const
	{
		assert(index >= 0 && index < 4);
		return (&x)[index];
	}

	union
	{
		__m128 vector;
		struct
		{
			float x, y, z, w;
		};
	};
};

inline vec4 operator +(const vec4& lhs, const vec4& rhs)
{
	return _mm_add_ps(lhs, rhs);
}

inline vec4 operator +(const vec4& lhs, float rhs)
{
	auto vRhs = _mm_set_ps1(rhs);
	return _mm_add_ps(lhs, vRhs);
}

inline vec4 operator +(float lhs, const vec4& rhs)
{
	const auto vLhs = _mm_set_ps1(lhs);
	return _mm_add_ps(vLhs, rhs);
}

inline vec4 operator -(const vec4& lhs, const vec4& rhs)
{
	return _mm_sub_ps(lhs, rhs);
}

inline vec4 operator -(const vec4& lhs, float rhs)
{
	const auto vRhs = _mm_set_ps1(rhs);
	return _mm_sub_ps(lhs, vRhs);
}

inline vec4 operator -(float lhs, const vec4& rhs)
{
	const auto vLhs = _mm_set_ps1(lhs);
	return _mm_sub_ps(vLhs, rhs);
}

inline vec4 operator *(const vec4& lhs, const vec4& rhs)
{
	return _mm_mul_ps(lhs, rhs);
}

inline vec4 operator *(const vec4& lhs, float rhs)
{
	const auto vRhs = _mm_set_ps1(rhs);
	return _mm_mul_ps(lhs, vRhs);
}

inline vec4 operator *(float lhs, const vec4& rhs)
{
	const auto vLhs = _mm_set_ps1(lhs);
	return _mm_mul_ps(vLhs, rhs);
}

inline vec4 operator /(const vec4& lhs, const vec4& rhs)
{
	return _mm_div_ps(lhs, rhs);
}

inline vec4 operator %(const vec4& lhs, const vec4& rhs)
{
	const auto c = lhs / rhs;

#if defined(_MSC_VER) && defined(__clang__)
	const auto trunc = vec4
	{
		(float)(int)c.x,
		(float)(int)c.y,
		(float)(int)c.z,
		(float)(int)c.w
};
#else
	const auto i = _mm_cvttps_epi32(c);
	const auto trunc = _mm_cvtepi32_ps(i);
#endif

	const auto base = _mm_mul_ps(trunc, rhs);
	return _mm_sub_ps(lhs, base);
}

inline vec4 operator /(const vec4& lhs, float rhs)
{
	const auto vRhs = _mm_set_ps1(rhs);
	return _mm_div_ps(lhs, vRhs);
}

inline vec4 operator /(float lhs, const vec4& rhs)
{
	const auto vLhs = _mm_set_ps1(lhs);
	return _mm_div_ps(vLhs, rhs);
}

inline vec4 operator -(const vec4& value)
{
	return _mm_sub_ps(_mm_setzero_ps(), value);
}

inline vec4& vec4::operator +=(const vec4& value)
{
	*this = *this + value;
	return *this;
}

inline vec4& vec4::operator +=(float value)
{
	*this = *this + value;
	return *this;
}

inline vec4& vec4::operator -=(const vec4& value)
{
	*this = *this - value;
	return *this;
}

inline vec4& vec4::operator -=(float value)
{
	*this = *this - value;
	return *this;
}

inline vec4& vec4::operator *=(const vec4& value)
{
	*this = *this * value;
	return *this;
}

inline vec4& vec4::operator *=(float value)
{
	*this = *this * value;
	return *this;
}

inline vec4& vec4::operator /=(const vec4& value)
{
	*this = *this / value;
	return *this;
}

inline vec4& vec4::operator /=(float value)
{
	*this = *this / value;
	return *this;
}

inline vec4 vdot4(const vec4& lhs, const vec4& rhs)
{
#if defined(SSE41)
	return _mm_dp_ps(lhs, rhs, 0xF1);
#else
	const auto value = lhs * rhs;

	// Calculates the sum of SSE Register - https://stackoverflow.com/a/35270026/195787
	auto shufReg = _mm_movehdup_ps(value);        // Broadcast elements 3,1 to 2,0
	auto sumsReg = _mm_add_ps(value, shufReg);
	shufReg = _mm_movehl_ps(shufReg, sumsReg); // High Half -> Low Half
	sumsReg = _mm_add_ss(sumsReg, shufReg);
	return _mm_shuffle_ps(sumsReg, sumsReg, 0);
#endif
}

inline float dot(const vec4& lhs, const vec4& rhs)
{
	const auto result = vdot4(lhs, rhs);
	return _mm_cvtss_f32(result);
}

inline float length(const vec4& value)
{
	auto result = vdot4(value, value);
#if defined(_MSC_VER) && defined(__clang__)
	float tmp = _mm_cvtss_f32(result);
	return sqrt(tmp);
#else
	result = _mm_sqrt_ss(result);
	return _mm_cvtss_f32(result);
#endif
}

inline float lengthSquared(const vec4& value)
{
	const auto result = vdot4(value, value);
	return _mm_cvtss_f32(result);
}

inline vec4 normalise(const vec4& value)
{
	auto result = vdot4(value, value);
#if defined(_MSC_VER) && defined(__clang__)
	float tmp = _mm_cvtss_f32(result);
	result = vec4{ sqrt(tmp) };
#else
	result = _mm_sqrt_ps(result);
#endif
	return _mm_div_ps(value, result);
}

inline float distance(const vec4& lhs, const vec4& rhs)
{
	return length(rhs - lhs);
}

inline vec4 cross(const vec4& lhs, const vec4& rhs)
{
	const auto a_yzx = _mm_shuffle_ps(lhs, lhs, _MM_SHUFFLE(3, 0, 2, 1));
	const auto b_yzx = _mm_shuffle_ps(rhs, rhs, _MM_SHUFFLE(3, 0, 2, 1));
	const auto c = _mm_sub_ps(_mm_mul_ps(lhs, b_yzx), _mm_mul_ps(a_yzx, rhs));
	return _mm_shuffle_ps(c, c, _MM_SHUFFLE(3, 0, 2, 1));
}

inline vec4 reflect(const vec4& direction, const vec4& normal)
{
	const auto result1 = vdot4(direction, normal); //tmp1 = d.n
	const auto result2 = _mm_mul_ps(normal, _mm_set_ps1(2)); //tmp2 = n*2
	const auto result = _mm_mul_ps(result1, result2); // result = tmp1 * tmp2; result = d.n * n * 2
	return _mm_sub_ps(direction, result); // result = direction - result
}

inline vec4 refract(const vec4& incident, const vec4& normal, float eta)
{
	const auto dotValue = dot(normal, incident);
	const auto k = 1 - eta * eta * (1 - dotValue * dotValue);
	return (eta * incident - (eta * dotValue + sqrt(k)) * normal) * static_cast<float>(k >= static_cast<float>(0));
}

static vec4 saturate(const vec4& value)
{
#if defined(_MSC_VER) && defined(__clang__)
	return vec4
	{
		std::min(std::max(value.x, 0.0f), 1.0f),
		std::min(std::max(value.y, 0.0f), 1.0f),
		std::min(std::max(value.z, 0.0f), 1.0f),
		std::min(std::max(value.w, 0.0f), 1.0f)
	};
#else
	return _mm_min_ps(_mm_max_ps(value, _mm_set_ps1(0)), _mm_set_ps1(1));
#endif
}

static vec4 lerp(const vec4& lhs, const vec4& rhs, float delta)
{
	const auto vdelta = _mm_set_ps1(delta);

	const auto tmp = _mm_fmadd_ps(_mm_sub_ps(_mm_setzero_ps(), vdelta), lhs, lhs);
	return _mm_fmadd_ps(vdelta, rhs, tmp);
}