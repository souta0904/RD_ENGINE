#include "TestFramework.h"
#include "math/MathUtility.h"
#include "math/Vector2.h"

using namespace rd;

namespace
{
	constexpr float kEpsilon = 1.0e-4f;
}

RD_TEST(Math, Utility)
{
	RD_CHECK_EQUAL(MathUtility::Clamp(5, 0, 3), 3);
	RD_CHECK_EQUAL(MathUtility::Clamp(-1, 0, 3), 0);
	RD_CHECK_EQUAL(MathUtility::Lerp(0.0f, 10.0f, 0.5f), 5.0f);
	RD_CHECK(MathUtility::NearlyEqual(0.1f + 0.2f, 0.3f));
	RD_CHECK_EQUAL(MathUtility::Saturate(1.5f), 1.0f);
	RD_CHECK_EQUAL(MathUtility::InverseLerp(0.0f, 10.0f, 5.0f), 0.5f);
	RD_CHECK_EQUAL(MathUtility::InverseLerp(3.0f, 3.0f, 5.0f), 0.0f);
	RD_CHECK_EQUAL(MathUtility::SmoothStep(0.0f, 1.0f, 0.5f), 0.5f);
	RD_CHECK_EQUAL(MathUtility::Sign(-3.0f), -1);
	RD_CHECK_EQUAL(MathUtility::Sign(0.0f), 0);
	RD_CHECK_NEAR(MathUtility::ToDegrees(MathUtility::kPi), 180.0f, kEpsilon);
	RD_CHECK_NEAR(MathUtility::ToRadians(90.0f), MathUtility::kPiOver2, kEpsilon);
	RD_CHECK_NEAR(MathUtility::Wrap(370.0f, 0.0f, 360.0f), 10.0f, kEpsilon);
	RD_CHECK_NEAR(MathUtility::Wrap(-10.0f, 0.0f, 360.0f), 350.0f, kEpsilon);
}


RD_TEST(Math, Vector2_Op)
{
	Vector2 a(1.0f, 2.0f);
	Vector2 b(3.0f, -4.0f);
	RD_CHECK(-a == Vector2(-1.0f, -2.0f));
	RD_CHECK(a + b == Vector2(4.0f, -2.0f));
	RD_CHECK(a - b == Vector2(-2.0f, 6.0f));
	RD_CHECK(a * b == Vector2(3.0f, -8.0f));
	RD_CHECK(b / Vector2(3.0f, 2.0f) == Vector2(1.0f, -2.0f));
	RD_CHECK(2.0f * a == a * 2.0f);
	RD_CHECK(b / 2.0f == Vector2(1.5f, -2.0f));

	Vector2 c = a;
	c += b;
	c -= b;
	c *= b;
	c /= b;
	c *= 3.0f;
	c /= 3.0f;
	RD_CHECK(c == a);

	RD_CHECK_EQUAL(a[0], 1.0f);
	RD_CHECK_EQUAL(a[1], 2.0f);
	c[1] = 5.0f;
	RD_CHECK_EQUAL(c.y, 5.0f);
}

RD_TEST(Math, Vector2_Func)
{
	Vector2 a(1.0f, 2.0f);
	Vector2 b(3.0f, -4.0f);
	RD_CHECK(Abs(b) == Vector2(3.0f, 4.0f));
	RD_CHECK_EQUAL(Cross(Vector2::kUnitX, Vector2::kUnitY), 1.0f);
	RD_CHECK_EQUAL(Dot(a, b), -5.0f);
	RD_CHECK_EQUAL(Dot(Vector2::kUnitX, Vector2::kUnitY), 0.0f);
	RD_CHECK_EQUAL(LengthSq(b), 25.0f);
	RD_CHECK_EQUAL(Length(b), 5.0f);
	RD_CHECK_EQUAL(Distance(a, b), Length(a - b));
	RD_CHECK(Min(a, b) == Vector2(1.0f, -4.0f));
	RD_CHECK(Max(a, b) == Vector2(3.0f, 2.0f));
	RD_CHECK(Reflect(Vector2(1.0f, -1.0f), Vector2::kUnitY) == Vector2(1.0f, 1.0f));
	RD_CHECK(ProjectOnPlane(Vector2(1.0f, -1.0f), Vector2::kUnitY) == Vector2(1.0f, 0.0f));
}

RD_TEST(Math, Vector2_Normalize)
{
	Vector2 v(3.0f, 4.0f);
	Vector2 n = v.Normalized();
	RD_CHECK_NEAR(n.x, 0.6f, kEpsilon);
	RD_CHECK_NEAR(n.y, 0.8f, kEpsilon);
	RD_CHECK(v == Vector2(3.0f, 4.0f));

	v.Normalize();
	RD_CHECK(v == n);

	RD_CHECK(Normalize(Vector2::kZero) == Vector2::kZero);
}
