/**
 * @file Test.h
 * @brief Lightweight testing framework for FuncDoodle.
 *
 * This file implements a minimal unit testing system used for both:
 * - Standard build tests (via FUNCDOODLE_BUILD_TESTS)
 * - Optional ImGui-based test integration (via FUNCDOODLE_BUILD_IMTESTS)
 *
 * Features:
 * - Test scopes with automatic reporting
 * - Assertions via CHECK macros
 * - Summary statistics (pass/fail/percentage)
 * - Optional ImGui test engine integration
 *
 * The framework is designed to be simple, header-only, and zero-dependency
 * aside from the optional ImGui test engine.
 */

#pragma once

#ifdef FUNCDOODLE_BUILD_IMTESTS
#include "imgui_te_engine.h"
#include "imgui_te_ui.h"
#endif

#include <atomic>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#ifdef FUNCDOODLE_BUILD_TESTS
int FuncDoodle_RunTests();
#endif

#ifdef _MSC_VER
/** @def CURRENT_FUNC @brief Expands to the current function name on MSVC. */
#define CURRENT_FUNC __FUNCTION__
#else
/** @def CURRENT_FUNC @brief Expands to the current function name on non-MSVC compilers. */
#define CURRENT_FUNC __func__
#endif

#ifdef FUNCDOODLE_BUILD_IMTESTS
int FuncDoodle_RegisterImTests();
extern ImGuiTestEngine* s_TestEngine;
#endif

namespace FuncDoodle {

	/**
	 * @brief Repeats a string N times.
	 *
	 * @param s Input string to repeat.
	 * @param n Number of repetitions.
	 * @return Concatenated string repeated N times.
	 */
	inline std::string Repeat(std::string_view s, size_t n) {
		std::string out;
		out.reserve(s.size() * n);
		while (n--)
			out += s;
		return out;
	}

	/**
	 * @struct TestResult
	 * @brief Represents a single failed or recorded test assertion.
	 */
	struct TestResult {
		std::string condition;  ///< Stringified test condition
		std::string message;    ///< Optional failure message
		bool passed;            ///< Whether the test passed
	};

	/**
	 * @class TestRegistry
	 * @brief Global singleton collecting test scope results.
	 *
	 * Aggregates results from all TestScope instances and prints a final summary.
	 */
	class TestRegistry {
		public:
		/**
		 * @fn Instance
		 * @brief Returns the global test registry singleton.
		 *
		 * @return Singleton test registry instance.
		 */
		static TestRegistry& Instance() {
			static TestRegistry instance;
			return instance;
		}

		/**
		 * @brief Registers a completed test scope.
		 */
		void RegisterScope(const char* name, int passed, int total, int failed) {
			m_Results.push_back({name, passed, total, failed});
		}

		/**
		 * @brief Prints a full summary of all executed tests.
		 *
		 * Displays total, passed, failed, and success percentage.
		 */
		void PrintSummary() {
			int total_tests = 0;
			int total_passed = 0;
			int total_failed = 0;

			for (const auto& r : m_Results) {
				total_tests += r.total;
				total_passed += r.passed;
				total_failed += r.failed;
			}

			double percentage = total_tests > 0
				? (double)total_passed / total_tests * 100.0
				: 0.0;

			std::string percent_str = std::to_string(percentage);
			percent_str = percent_str.substr(0, percent_str.find('.') + 2);

			std::cout << "\n";
			std::cout << "╔" << Repeat("═", 62) << "╗\n";
			std::cout << "║                      TEST SUMMARY                            ║\n";
			std::cout << "╠" << Repeat("═", 62) << "╣\n";

			std::cout << "║  Total:  " << total_tests
					  << std::string(48 - std::to_string(total_tests).length(), ' ')
					  << "    ║\n";

			std::cout << "║  \033[32mPassed:  " << total_passed << "\033[0m"
					  << std::string(47 - std::to_string(total_passed).length(), ' ')
					  << "    ║\n";

			std::cout << "║  \033[31mFailed:  " << total_failed << "\033[0m"
					  << std::string(47 - std::to_string(total_failed).length(), ' ')
					  << "    ║\n";

			std::string color =
				(percentage == 100.0)
					? "\033[32m"
					: (percentage >= 80.0 ? "\033[33m" : "\033[31m");

			std::cout << "║  " << color << "Success: " << percent_str
					  << "%\033[0m"
					  << std::string(46 - percent_str.length(), ' ')
					  << "    ║\n";

			std::cout << "╚" << Repeat("═", 62) << "╝\n";
		}

		private:
		struct ScopeResult {
			const char* name;
			int passed, total, failed;
		};

		std::vector<ScopeResult> m_Results;
	};

	/**
	 * @class TestScope
	 * @brief RAII-style test container for grouping assertions.
	 *
	 * Automatically prints results when leaving scope.
	 */
	class TestScope {
		public:
		/**
		 * @fn TestScope
		 * @brief Creates a named test scope.
		 *
		 * @param name Name printed in test output.
		 */
		explicit TestScope(const char* name)
			: m_Name(name) {}

		/**
		 * @brief Finalizes and prints test scope results.
		 */
		~TestScope() {
			TestRegistry::Instance().RegisterScope(
				m_Name.c_str(), m_Passed, m_Total, m_Failed);

			std::cout << "\n";
			std::cout << "╔" << Repeat("═", 62) << "╗\n";
			std::cout << "║  \033[1m" << m_Name << "\033[0m"
					  << std::string(60 - m_Name.length(), ' ') << "║\n";
			std::cout << "╠" << Repeat("═", 62) << "╣\n";

			if (m_Failed == 0) {
				std::cout << "║  \033[32m[PASS]\033[0m " << m_Passed << "/"
						  << m_Total << " passed"
						  << std::string(45 -
							 std::to_string(m_Passed).length() -
							 std::to_string(m_Total).length(), ' ')
						  << "║\n";
			} else {
				std::cout << "║  \033[32m[PASS]\033[0m " << m_Passed << "/"
						  << m_Total << " passed"
						  << std::string(44 -
							 std::to_string(m_Passed).length() -
							 std::to_string(m_Total).length(), ' ')
						  << "║\n";

				std::cout << "║  \033[31m[FAIL]\033[0m " << m_Failed
						  << " failed"
						  << std::string(47 - std::to_string(m_Failed).length(), ' ')
						  << "║\n";
			}

			std::cout << "╚" << Repeat("═", 62) << "╝\n";
		}

		/**
		 * @brief Evaluates a test condition.
		 */
		void Check(bool cond, const char* condStr, const char* msg = "") {
			m_Total++;
			if (cond)
				m_Passed++;
			else {
				m_Failed++;
				m_Results.push_back({condStr, msg ? msg : "", false});
			}
		}

		private:
		std::string m_Name;
		int m_Total = 0, m_Passed = 0, m_Failed = 0;
		std::vector<TestResult> m_Results;
	};

} // namespace FuncDoodle

// ================= MACROS =================

/**
 * @def TEST_SCOPE
 * @brief Starts a named RAII test scope.
 */
#define TEST_SCOPE(name) FuncDoodle::TestScope _test_scope(name)

/**
 * @def CHECK
 * @brief Records whether a condition is true.
 */
#define CHECK(cond, msg) _test_scope.Check(cond, #cond, msg)
/**
 * @def CHECK_EQ
 * @brief Records whether two expressions are equal.
 */
#define CHECK_EQ(a, b, msg) _test_scope.Check((a) == (b), #a " == " #b, msg)
/**
 * @def CHECK_NE
 * @brief Records whether two expressions are not equal.
 */
#define CHECK_NE(a, b, msg) _test_scope.Check((a) != (b), #a " != " #b, msg)
/**
 * @def CHECK_LT
 * @brief Records whether the left expression is less than the right.
 */
#define CHECK_LT(a, b, msg) _test_scope.Check((a) < (b), #a " < " #b, msg)
/**
 * @def CHECK_LE
 * @brief Records whether the left expression is less than or equal to the right.
 */
#define CHECK_LE(a, b, msg) _test_scope.Check((a) <= (b), #a " <= " #b, msg)
/**
 * @def CHECK_GT
 * @brief Records whether the left expression is greater than the right.
 */
#define CHECK_GT(a, b, msg) _test_scope.Check((a) > (b), #a " > " #b, msg)
/**
 * @def CHECK_GE
 * @brief Records whether the left expression is greater than or equal to the right.
 */
#define CHECK_GE(a, b, msg) _test_scope.Check((a) >= (b), #a " >= " #b, msg)
/**
 * @def CHECK_NULL
 * @brief Records whether a pointer is null.
 */
#define CHECK_NULL(ptr, msg) _test_scope.Check((ptr) == nullptr, #ptr " == nullptr", msg)
/**
 * @def CHECK_NOT_NULL
 * @brief Records whether a pointer is non-null.
 */
#define CHECK_NOT_NULL(ptr, msg) _test_scope.Check((ptr) != nullptr, #ptr " != nullptr", msg)
