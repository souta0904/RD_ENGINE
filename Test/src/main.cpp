#include "TestFramework.h"
#include <cstdio>
#include <exception>
#include <string>
#include <Windows.h>

int main(int argc, char** argv)
{
	// utf-8で出力
	SetConsoleOutputCP(CP_UTF8);

	std::string filter = (argc > 1) ? argv[1] : "";
	int checks = 0;
	int passed = 0;
	int failed = 0;

	for (const rdtest::TestCase& test : rdtest::Registry())
	{
		std::string testName = std::string(test.group) + "." + test.name;
		if (!filter.empty() && testName.find(filter) == std::string::npos) continue;

		rdtest::Context context;
		rdtest::Current() = &context;
		try
		{
			test.func();
		}
		catch (const rdtest::RequireFailed&)
		{
		}
		catch (const std::exception& e)
		{
			context.failures.push_back(std::string("exception: ") + e.what());
		}
		rdtest::Current() = nullptr;
		checks += context.checks;

		if (context.failures.empty())
		{
			++passed;
			std::printf(" ok   %s\n", testName.c_str());
		}
		else
		{
			++failed;
			std::printf(" FAIL %s\n", testName.c_str());
			for (const std::string& failure : context.failures)
			{
				std::printf("      %s\n", failure.c_str());
			}
		}
	}

	std::printf("\n%d passed, %d failed (%d checks)\n", passed, failed, checks);
	return failed;
}
