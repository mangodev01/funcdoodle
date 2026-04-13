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

#if defined(_MSC_VER)
#define CURRENT_FUNC __FUNCTION__
#else
#define CURRENT_FUNC __func__
#endif

#ifdef FUNCDOODLE_BUILD_IMTESTS
int FuncDoodle_RegisterImTests();
extern ImGuiTestEngine* s_TestEngine;
#endif

namespace FuncDoodle {
	inline std::string repeat(std::string_view s, size_t n) {
		std::string out;
		out.reserve(s.size() * n);
		while (n--)
			out += s;
		return out;
	}

	struct TestResult {
			std::string condition;
			std::string message;
			bool passed;
	};

	class TestRegistry {
		public:
			static TestRegistry& Instance() {
				static TestRegistry instance;
				return instance;
			}

			void RegisterScope(
				const char* name, int passed, int total, int failed) {
				m_Results.push_back({name, passed, total, failed});
			}

			void PrintSummary() {
				int total_tests = 0, total_passed = 0, total_failed = 0;
				for (const auto& r : m_Results) {
					total_tests += r.total;
					total_passed += r.passed;
					total_failed += r.failed;
				}

				double percentage =
					total_tests > 0 ? (double)total_passed / total_tests * 100.0
									: 0.0;
				std::string percent_str = std::to_string(percentage);
				percent_str = percent_str.substr(0, percent_str.find('.') + 2);

				std::cout << "\n";
				std::cout << "╔" << repeat("═", 62) << "╗\n";
				std::cout << "║                      TEST SUMMARY              "
							 "              ║\n";
				std::cout << "╠" << repeat("═", 62) << "╣\n";
				std::cout << "║  Total:  " << total_tests
						  << std::string(
								 48 - std::to_string(total_tests).length(), ' ')
						  << "    ║\n";
				std::cout << "║  \033[32mPassed:  " << total_passed << "\033[0m"
						  << std::string(
								 47 - std::to_string(total_passed).length(),
								 ' ')
						  << "    ║\n";
				std::cout << "║  \033[31mFailed:  " << total_failed << "\033[0m"
						  << std::string(
								 47 - std::to_string(total_failed).length(),
								 ' ')
						  << "    ║\n";

				std::string color =
					(percentage == 100.0)
						? "\033[32m"
						: (percentage >= 80.0 ? "\033[33m" : "\033[31m");
				std::cout << "║  " << color << "Success: " << percent_str
						  << "%\033[0m"
						  << std::string(46 - percent_str.length(), ' ')
						  << "    ║\n";
				std::cout << "╚" << repeat("═", 62) << "╝\n";
			}

		private:
			struct ScopeResult {
					const char* name;
					int passed, total, failed;
			};
			std::vector<ScopeResult> m_Results;
	};

	class TestScope {
		public:
			explicit TestScope(const char* name)
				: m_Name(name), m_Total(0), m_Passed(0), m_Failed(0) {}

			~TestScope() {
				FuncDoodle::TestRegistry::Instance().RegisterScope(
					m_Name.c_str(), m_Passed, m_Total, m_Failed);
				std::cout << "\n";
				std::cout << "╔" << repeat("═", 62) << "╗\n";
				std::cout << "║  \033[1m" << m_Name << "\033[0m"
						  << std::string(60 - m_Name.length(), ' ') << "║\n";
				std::cout << "╠" << repeat("═", 62) << "╣\n";

				if (m_Failed == 0) {
					std::cout
						<< "║  \033[32m[PASS]\033[0m " << m_Passed << "/"
						<< m_Total << " passed"
						<< std::string(45 - std::to_string(m_Passed).length() -
										   std::to_string(m_Total).length(),
							   ' ')
						<< "║\n";
				} else {
					std::cout
						<< "║  \033[32m[PASS]\033[0m " << m_Passed << "/"
						<< m_Total << " passed"
						<< std::string(44 - std::to_string(m_Passed).length() -
										   std::to_string(m_Total).length(),
							   ' ')
						<< "║\n";
					std::cout
						<< "║  \033[31m[FAIL]\033[0m " << m_Failed << " failed"
						<< std::string(
							   47 - std::to_string(m_Failed).length(), ' ')
						<< "║\n";
					std::cout << "╠" << repeat("═", 62) << "╣\n";
					std::cout << "║  \033[31mFailures:\033[0m"
							  << std::string(49, ' ') << " ║\n";

					for (const auto& r : m_Results) {
						if (!r.passed) {
							std::string line = "  • " + r.condition;
							if (!r.message.empty())
								line += ": " + r.message;
							size_t pad =
								line.length() < 60 ? (60 - line.length()) : 0;
							std::cout << "║    " << line
									  << std::string(pad, ' ') << " ║\n";
						}
					}
				}
				std::cout << "╚" << repeat("═", 62) << "╝\n";
			}

			void Check(bool cond, const char* condStr, const char* msg = "") {
				m_Total++;
				if (cond) {
					m_Passed++;
				} else {
					m_Failed++;
					m_Results.push_back({std::string(condStr),
						std::string(msg ? msg : ""), false});
				}
			}

			int PassedCount() const { return m_Passed; }
			int FailedCount() const { return m_Failed; }
			int TotalCount() const { return m_Total; }

		private:
			std::string m_Name;
			int m_Total = 0, m_Passed = 0, m_Failed = 0;
			std::vector<TestResult> m_Results;
	};

}  // namespace FuncDoodle

#define TEST_SCOPE(name) FuncDoodle::TestScope _test_scope(name)
#define CHECK(cond, msg) _test_scope.Check(cond, #cond, msg)
#define CHECK_EQ(a, b, msg) _test_scope.Check((a) == (b), #a " == " #b, msg)
#define CHECK_NE(a, b, msg) _test_scope.Check((a) != (b), #a " != " #b, msg)
#define CHECK_LT(a, b, msg) _test_scope.Check((a) < (b), #a " < " #b, msg)
#define CHECK_LE(a, b, msg) _test_scope.Check((a) <= (b), #a " <= " #b, msg)
#define CHECK_GT(a, b, msg) _test_scope.Check((a) > (b), #a " > " #b, msg)
#define CHECK_GE(a, b, msg) _test_scope.Check((a) >= (b), #a " >= " #b, msg)
#define CHECK_NULL(ptr, msg) \
	_test_scope.Check((ptr) == nullptr, #ptr " == nullptr", msg)
#define CHECK_NOT_NULL(ptr, msg) \
	_test_scope.Check((ptr) != nullptr, #ptr " != nullptr", msg)
