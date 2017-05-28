#include "stdafx.h"
#include "Torus.h"
#include "mathsHelper.h"

#include <complex>

bool solveQuadratic(float a, float b, float c, float& root)
{
	if (a == 0.0 || abs(a / b) < 1.0e-6f)
	{
		if (abs(b) < 1.0e-4f)
			return false;

		root = -c / b;
		return true;
	}

	auto discriminant = b * b - 4 * a * c;
	if (discriminant >= 0)
	{
		discriminant = sqrt(discriminant);
		root = (b + discriminant) * -0.5f / a;
		return true;
	}

	return false;
}
//----------------------------------------------------------------------------
bool solveCubic(float a, float b, float c, float d, float& root)
{
	if (a == 0 || abs(a / b) < 1.0e-6f)
		return solveQuadratic(b, c, d, root);

	auto B = b / a, C = c / a, D = d / a;

	auto Q = (B * B - C * 3) / 9;
	auto QQQ = Q * Q * Q;
	auto R = (2 * B * B * B - 9 * B * C + 27 * D) / 54;
	auto RR = R * R;

	// 3 real roots
	if (RR < QQQ)
	{
		/* This sqrt and division is safe, since RR >= 0, so QQQ > RR,    */
		/* so QQQ > 0.  The acos is also safe, since RR/QQQ < 1, and      */
		/* thus R/sqrt(QQQ) < 1.                                     */
		auto theta = acos(R / sqrt(QQQ));
		/* This sqrt is safe, since QQQ >= 0, and thus Q >= 0             */
		auto r1 = -2 * sqrt(Q);
		auto r2 = r1;
		auto r3 = r1;
		r1 *= cos(theta / 3);
		r2 *= cos((theta + 2 * PI) / 3);
		r3 *= cos((theta - 2 * PI) / 3);

		r1 -= B / 3;
		r2 -= B / 3;
		r3 -= B / 3;

		root = 1000000;

		if (r1 >= 0) root = r1;
		if (r2 >= 0 && r2 < root) root = r2;
		if (r3 >= 0 && r3 < root) root = r3;

		return true;
	}
	// 1 real root

	auto A2 = -pow(fabs(c) + sqrt(RR - QQQ), 1.0f / 3.0f);
	if (A2 != 0)
	{
		if (R < 0)
			A2 = -A2;
		root = A2 + Q / A2;
	}
	root -= B / 3;
	return true;
}

bool solveQuartic(float a, float b, float c, float d, float e, float& root)
{
	// When a or (a and b) are magnitudes of order smaller than C,D,E
	// just ignore them entirely. 
	if (a == 0 || abs(a / b) < 1.0e-6f || abs(a / c) < 1.0e-6f)
		return solveCubic(b, c, d, e, root);

	auto aa = a * a;
	auto aaa = aa * a;
	auto bb = b * b;
	auto bbb = bb * b;
	auto alpha = -3 * bb / (8 * aa) + c / a;
	auto alpha2 = alpha * alpha;
	auto beta = bbb / (8 * aaa) + b * c / (-2 * aa) + d / a;
	auto gamma = -3 * bbb * b / (256 * aaa * a) + c * bb / (16 * aaa) + b * d / (-4 * aa) + e / a;

	if (beta == 0)
	{
		root = b / (-4 * a) + sqrt(0.5f * (-alpha + sqrt(alpha2 + 4 * gamma)));
		return true;
	}

	std::complex<float> P = -alpha2 / 12 - gamma;
	std::complex<float> Q = -alpha2 * alpha / 108 + alpha * gamma / 3 - beta * beta / 8;
	auto R = Q * 0.5f + sqrt(Q * Q * 0.25f + P * P * P / 27.0f);
	auto U = pow(R, 1.0f / 3.0f);
	auto y = -5 * alpha / 6 - U;
	if (U != 0.0f)
		y += P / (3.0f * U);
	auto W = sqrt(alpha + y + y);

	auto foundRealRoot = false;

	auto firstPart = b / (-4 * a);
	auto secondPart = -3 * alpha - 2.0f * y;
	auto thirdPart = 2.0f * beta / W;

	auto aRoot = firstPart + 0.5f * (-W - sqrt(secondPart + thirdPart));
	if (abs(aRoot.imag()) < 1.0e-10f && aRoot.real() >= 0)
	{
		root = aRoot.real();
		foundRealRoot = true;
	}

	aRoot = firstPart + 0.5f * (-W + sqrt(secondPart + thirdPart));
	if (abs(aRoot.imag()) < 1.0e-10f && aRoot.real() >= 0 && (!foundRealRoot || aRoot.real() < root))
	{
		root = aRoot.real();
		foundRealRoot = true;
	}

	aRoot = firstPart + 0.5f * (W - sqrt(secondPart - thirdPart));
	if (abs(aRoot.imag()) < 1.0e-10f && aRoot.real() >= 0 && (!foundRealRoot || aRoot.real() < root))
	{
		root = aRoot.real();
		foundRealRoot = true;
	}

	aRoot = firstPart + 0.5f * (W + sqrt(secondPart - thirdPart));
	if (abs(aRoot.imag()) < 1.0e-10f && aRoot.real() >= 0 && (!foundRealRoot || aRoot.real() < root))
	{
		root = aRoot.real();
		foundRealRoot = true;
	}

	return foundRealRoot;
}

bool Torus::intersect(const Ray& ray, IntersectionResult& result) const
{
	auto EX = (ray.position - position).x;
	auto EY = (ray.position - position).y;
	auto EZ = (ray.position - position).z;
	auto DX = ray.direction.x;
	auto DY = ray.direction.y;
	auto DZ = ray.direction.z;

	auto A = majorRadius;

	auto G = 4 * A * A * (EX * EX + EY * EY);
	auto H = 8 * A * A * (DX * EX + DY * EY);
	auto I = 4 * A * A * (DX * DX + DY * DY);
	auto J = lengthSquared(ray.position);
	auto K = 2 * dot(ray.direction, ray.position);
	auto L = lengthSquared(ray.direction) + (A * A - minorRadius * minorRadius);

	float root;
	auto found = solveQuartic(J * J, 2 * J * K, 2 * J * L + K * K - G, 2 * K * L - H, L * L - I, root);

	if (!found || root < 0)
		return false;

	result.distance = root;
	result.point = ray.calculatePoint(root);
	result.normal = vec4{ 0, 1, 0, 0 };
	return true;
}

Ray Torus::handleRefraction(const vec4& direction, const vec4& hitPoint, const vec4& normal, float refractivity) const
{
	throw std::exception();
}

vec4 Torus::getTextureCoordinates(const vec4& hitPoint) const
{
	throw std::exception();
}
