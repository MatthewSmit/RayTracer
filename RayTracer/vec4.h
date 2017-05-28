#pragma once
#include <immintrin.h>
#include <xmmintrin.h>
#include <cmath>

#include "AlignedObject.h"

struct alignas(16) vec4 : AlignedObject
{
	vec4()
	{
		value = _mm_setzero_ps();
	}

	explicit vec4(float value)
	{
		this->value = _mm_set_ps1(value);
	}

	vec4(float x, float y, float z, float w)
	{
		value = _mm_setr_ps(x, y, z, w);
	}

	explicit vec4(const __m128& value) :
		value{ value }
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

	union
	{
		__m128 value;
		struct
		{
			float x, y, z, w;
		};
	};
};

inline vec4 operator +(const vec4& lhs, const vec4& rhs)
{
	auto result = _mm_add_ps(lhs.value, rhs.value);
	return vec4{ result };
}

inline vec4 operator +(const vec4& lhs, float rhs)
{
	auto vRhs = _mm_set_ps1(rhs);
	auto result = _mm_add_ps(lhs.value, vRhs);
	return vec4{ result };
}

inline vec4 operator +(float lhs, const vec4& rhs)
{
	auto vLhs = _mm_set_ps1(lhs);
	auto result = _mm_add_ps(vLhs, rhs.value);
	return vec4{ result };
}

inline vec4 operator -(const vec4& lhs, const vec4& rhs)
{
	auto result = _mm_sub_ps(lhs.value, rhs.value);
	return vec4{ result };
}

inline vec4 operator -(const vec4& lhs, float rhs)
{
	auto vRhs = _mm_set_ps1(rhs);
	auto result = _mm_sub_ps(lhs.value, vRhs);
	return vec4{ result };
}

inline vec4 operator -(float lhs, const vec4& rhs)
{
	auto vLhs = _mm_set_ps1(lhs);
	auto result = _mm_sub_ps(vLhs, rhs.value);
	return vec4{ result };
}

inline vec4 operator *(const vec4& lhs, const vec4& rhs)
{
	auto result = _mm_mul_ps(lhs.value, rhs.value);
	return vec4{ result };
}

inline vec4 operator *(const vec4& lhs, float rhs)
{
	auto vRhs = _mm_set_ps1(rhs);
	auto result = _mm_mul_ps(lhs.value, vRhs);
	return vec4{ result };
}

inline vec4 operator *(float lhs, const vec4& rhs)
{
	auto vLhs = _mm_set_ps1(lhs);
	auto result = _mm_mul_ps(vLhs, rhs.value);
	return vec4{ result };
}

inline vec4 operator %(const vec4& lhs, const vec4& rhs)
{
	auto c = _mm_div_ps(lhs.value, rhs.value);
	auto i = _mm_cvttps_epi32(c);
	auto trunc = _mm_cvtepi32_ps(i);
	auto base = _mm_mul_ps(trunc, rhs.value);
	auto result = _mm_sub_ps(lhs.value, base);
	return vec4{ result };
}

inline vec4 operator /(const vec4& lhs, const vec4& rhs)
{
	auto result = _mm_div_ps(lhs.value, rhs.value);
	return vec4{ result };
}

inline vec4 operator /(const vec4& lhs, float rhs)
{
	auto vRhs = _mm_set_ps1(rhs);
	auto result = _mm_div_ps(lhs.value, vRhs);
	return vec4{ result };
}

inline vec4 operator /(float lhs, const vec4& rhs)
{
	auto vLhs = _mm_set_ps1(lhs);
	auto result = _mm_div_ps(vLhs, rhs.value);
	return vec4{ result };
}

inline vec4 operator -(const vec4& value)
{
	auto result = _mm_sub_ps(_mm_setzero_ps(), value.value);
	return vec4{ result };
}

inline vec4& vec4::operator +=(const vec4& x)
{
	*this = *this + x;
	return *this;
}

inline vec4& vec4::operator +=(float x)
{
	*this = *this + x;
	return *this;
}

inline vec4& vec4::operator -=(const vec4& x)
{
	*this = *this - x;
	return *this;
}

inline vec4& vec4::operator -=(float x)
{
	*this = *this - x;
	return *this;
}

inline vec4& vec4::operator *=(const vec4& x)
{
	*this = *this * x;
	return *this;
}

inline vec4& vec4::operator *=(float x)
{
	*this = *this * x;
	return *this;
}

inline vec4& vec4::operator /=(const vec4& x)
{
	*this = *this / x;
	return *this;
}

inline vec4& vec4::operator /=(float x)
{
	*this = *this / x;
	return *this;
}

inline float dot(const vec4& lhs, const vec4& rhs)
{
	auto result = _mm_dp_ps(lhs.value, rhs.value, 0xF1);
	return _mm_cvtss_f32(result);
}

inline float length(const vec4& value)
{
	auto result = _mm_dp_ps(value.value, value.value, 0xF1);
	result = _mm_sqrt_ss(result);
	return _mm_cvtss_f32(result);
}

inline float lengthSquared(const vec4& value)
{
	auto result = _mm_dp_ps(value.value, value.value, 0xF1);
	return _mm_cvtss_f32(result);
}

inline vec4 normalise(const vec4& value)
{
	auto result = _mm_dp_ps(value.value, value.value, 0xFF);

	//auto halfResult = _mm_mul_ps(result, _mm_set_ps1(.5f));
	//result = _mm_rsqrt_ps(result);
	//
	//auto threehalfs4 = _mm_set_ps1(1.5f);
	//result =  _mm_mul_ps(result, _mm_sub_ps(threehalfs4, _mm_mul_ps(halfResult, _mm_mul_ps(result, result))));
	//
	//result = _mm_mul_ps(value.value, result);

	result = _mm_sqrt_ps(result);
	result = _mm_div_ps(value.value, result);
	return vec4{ result };
}

inline float distance(const vec4& lhs, const vec4& rhs)
{
	return length(rhs - lhs);
}

inline vec4 cross(const vec4& lhs, const vec4& rhs)
{
	auto a_yzx = _mm_shuffle_ps(lhs.value, lhs.value, _MM_SHUFFLE(3, 0, 2, 1));
	auto b_yzx = _mm_shuffle_ps(rhs.value, rhs.value, _MM_SHUFFLE(3, 0, 2, 1));
	auto c = _mm_sub_ps(_mm_mul_ps(lhs.value, b_yzx), _mm_mul_ps(a_yzx, rhs.value));
	return vec4{ _mm_shuffle_ps(c, c, _MM_SHUFFLE(3, 0, 2, 1)) };
}

inline vec4 reflect(const vec4& direction, const vec4& normal)
{
	auto result1 = _mm_dp_ps(direction.value, normal.value, 0xFF); //tmp1 = d.n
	auto result2 = _mm_mul_ps(normal.value, _mm_set_ps1(2)); //tmp2 = n*2
	auto result = _mm_mul_ps(result1, result2); // result = tmp1 * tmp2; result = d.n * n * 2
	result = _mm_sub_ps(direction.value, result); // result = direction - result
	return vec4{ result };
}

inline vec4 refract(const vec4& incident, const vec4& normal, float eta)
{
	auto dotValue = dot(normal, incident);
	auto k = 1 - eta * eta * (1 - dotValue * dotValue);
	return (eta * incident - (eta * dotValue + std::sqrt(k)) * normal) * static_cast<float>(k >= static_cast<float>(0));

	/*auto etav = _mm_set_ps1(eta);
	auto mul0 = _mm_mul_ps(etav, etav);
	auto mul1 = _mm_mul_ps(dot0, dot0);
	auto sub0 = _mm_sub_ps(_mm_set_ps1(1.0f), mul0);
	auto sub1 = _mm_sub_ps(_mm_set_ps1(1.0f), mul1);
	auto mul2 = _mm_mul_ps(sub0, sub1);

	if (_mm_movemask_ps(_mm_cmplt_ss(mul2, _mm_set_ps1(0.0f))) == 0)
	return vec4{ _mm_set_ps1(0.0f) };

	auto sqt0 = _mm_sqrt_ps(mul2);
	//AVX2
	auto mad0 = _mm_fmadd_ps(etav, dot0, sqt0);
	//glm_vec4_add(glm_vec4_mul(a, b), c)
	auto mul4 = _mm_mul_ps(mad0, normal.value);
	auto mul5 = _mm_mul_ps(etav, incident.value);
	auto sub2 = _mm_sub_ps(mul5, mul4);

	return vec4{ sub2 };*/
}

static vec4 saturate(const vec4& value)
{
	return vec4{ _mm_min_ps(_mm_max_ps(value.value, _mm_set_ps1(0)), _mm_set_ps1(1)) };
}

static vec4 lerp(const vec4& lhs, const vec4& rhs, float delta)
{
	auto vdelta = _mm_set_ps1(delta);

	auto tmp = _mm_fmadd_ps(_mm_sub_ps(_mm_setzero_ps(), vdelta), lhs.value, lhs.value);
	return vec4{ _mm_fmadd_ps(vdelta, rhs.value, tmp) };
}