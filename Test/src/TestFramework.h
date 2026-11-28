#pragma once
#include <cmath>
#include <format>
#include <string>
#include <vector>

namespace rdtest
{
	// テスト本体
	struct TestCase
	{
		const char* group;
		const char* name;
		void (*func)();
	};

	// テスト一覧
	inline std::vector<TestCase>& Registry()
	{
		static std::vector<TestCase> sTests;
		return sTests;
	}

	// 登録
	struct Registrar
	{
		Registrar(const char* group, const char* name, void (*func)())
		{
			Registry().push_back({ group, name, func });
		}
	};

	struct Context
	{
		int checks = 0;
		std::vector<std::string> failures;
	};

	inline Context*& Current()
	{
		static Context* sContext = nullptr;
		return sContext;
	}

	// 記録
	inline bool Report(
		bool isOk,
		const char* expression,
		const char* file,
		int line,
		const std::string& detail = {})
	{
		Context* ctx = Current();
		++ctx->checks;
		if (!isOk)
		{
			std::string msg = std::format("{}({}): {}", file, line, expression);
			if (!detail.empty())
			{
				msg += " [" + detail + "]";
			}
			ctx->failures.push_back(msg);
		}

		return isOk;
	}

	struct RequireFailed {};

	// 等しいか
	template <class A, class B>
	bool CheckEqual(
		const A& a,
		const B& b,
		const char* expression,
		const char* file,
		int line)
	{
		bool isOk = (a == b);
		return Report(isOk, expression, file, line, isOk ? std::string() : std::format("{} vs {}", a, b));
	}

	inline bool Near(float a, float b, float epsilon)
	{
		return std::fabs(a - b) <= epsilon;
	}

	// floatがほぼ等しいか
	inline bool CheckNear(
		float a,
		float b,
		float epsilon,
		const char* expression,
		const char* file,
		int line)
	{
		bool isOk = Near(a, b, epsilon);
		return Report(isOk, expression, file, line, isOk ? std::string() : std::format("{} vs {}", a, b));
	}
}

// マクロを展開するため2段階
#define RD_TEST_CONCAT_INNER(a, b) a##b
#define RD_TEST_CONCAT(a, b) RD_TEST_CONCAT_INNER(a, b)

// テストの宣言、登録、定義
#define RD_TEST(Group, Name) \
	static void RD_TEST_CONCAT(Test_##Group##_, Name)(); \
	static const ::rdtest::Registrar RD_TEST_CONCAT(sRegistrar_##Group##_, Name)(#Group, #Name, &RD_TEST_CONCAT(Test_##Group##_, Name)); \
	static void RD_TEST_CONCAT(Test_##Group##_, Name)()

#define RD_CHECK(expression) ::rdtest::Report(static_cast<bool>(expression), #expression, __FILE__, __LINE__)
#define RD_CHECK_EQUAL(a, b) ::rdtest::CheckEqual((a), (b), #a " == " #b, __FILE__, __LINE__)
#define RD_CHECK_NEAR(a, b, eps) ::rdtest::CheckNear((a), (b), (eps), #a " ~= " #b, __FILE__, __LINE__)
#define RD_REQUIRE(expression) do { if (!RD_CHECK(expression)) throw ::rdtest::RequireFailed{}; } while (0)
