#pragma once
#include <algorithm>
#include <cmath>
#include <limits>
#include <numbers>

namespace rd::MathUtility
{
	inline constexpr float kPi = std::numbers::pi_v<float>;
	inline constexpr float k2Pi = kPi * 2.0f;
	inline constexpr float kPiOver2 = kPi / 2.0f;
	inline constexpr float kInfinity = std::numeric_limits<float>::infinity();
	inline constexpr float kEpsilon = 1.0e-6f;

	template <class T>
	constexpr T Clamp(const T& value, const T& low, const T& high)
	{
		return std::min(high, std::max(low, value));
	}

	template <class T>
	constexpr T Lerp(const T& a, const T& b, float t)
	{
		return a + (b - a) * t;
	}

	inline bool NearlyEqual(float a, float b, float epsilon = 1.0e-4f)
	{
		return std::fabs(a - b) <= epsilon;
	}

	inline constexpr float Saturate(float value)
	{
		return Clamp(value, 0.0f, 1.0f);
	}

	inline constexpr float InverseLerp(float a, float b, float value)
	{
		if (a == b) return 0.0f;

		return Saturate((value - a) / (b - a));
	}

	inline constexpr float SmoothStep(float from, float to, float t)
	{
		t = Saturate(t);
		t = t * t * (3.0f - 2.0f * t);
		return Lerp(from, to, t);
	}

	inline constexpr int Sign(float value)
	{
		return (value > 0.0f) - (value < 0.0f);
	}

	inline constexpr float ToDegrees(float radians)
	{
		return radians * (180.0f / kPi);
	}

	inline constexpr float ToRadians(float degrees)
	{
		return degrees * (kPi / 180.0f);
	}

	inline float Wrap(float value, float min, float max)
	{
		float range = max - min;
		return value - range * std::floor((value - min) / range);
	}
}
