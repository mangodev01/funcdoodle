/**
 * @file FuncPCH.h
 * @brief Logging system, assertion macros, platform utilities, and build/config
 * macros.
 *
 * This header defines a collection of global utilities used across FuncDoodle,
 * including:
 *
 * - Logging system (FUNC_DBG, FUNC_INF, FUNC_WARN, FUNC_ERR, FUNC_FATAL)
 * - Assertion/verification macros (FUNC_AOV, FUNC_AOV_EX, FUNC_DASS)
 * - Conditional compile-time utilities for C++20 std::format support
 * - Platform-specific helpers (file explorer opening, string comparisons)
 * - Global log storage (s_Logs)
 * - Versioning macros (FUNCVER, FDPVERMAJOR, FDPVERMINOR)
 * - Utility macros (e.g., color inversion helpers)
 *
 * @warning This file defines macros with side effects (logging, exiting, memory
 * allocation).
 * @warning s_Logs stores raw pointers and requires external cleanup.
 *
 * @note Many macros behave differently depending on DEBUG / NDEBUG builds.
 */

#pragma once

#include <algorithm>
#include <filesystem>
#include <print>
#include <string>
#include <vector>

/**
 * Global log storage.
 *
 * Stores log messages produced by logging macros.
 */
extern std::vector<std::string> s_Logs;

#if __cplusplus >= 202002L && defined(__cpp_lib_format)
#include <format>
/**
 * @def FUNC_FORMAT_AVAILABLE
 * @brief Indicates whether std::format support is available.
 */
#define FUNC_FORMAT_AVAILABLE 1
#else
/**
 * @def FUNC_FORMAT_AVAILABLE
 * @brief Indicates whether std::format support is available.
 */
#define FUNC_FORMAT_AVAILABLE 0
#endif

#if __cplusplus < 202002L
#error \
	"std::format is not supported by your compiler configuration. Please consider using -std=c++20 or later."
#else
#define FUNC_FMT(...) std::format(__VA_ARGS__)
#endif

/**
 * @def FUNC_AOV
 * @brief Stands for "Assert Or Verify."
 *
 * Behavior depends on build configuration:
 * - DEBUG: assertion-style logging
 * - NDEBUG: verification-style logging
 */
#ifdef DEBUG
#define FUNC_AOV(x)                                                       \
	do {                                                                  \
		if (!(x)) {                                                       \
			std::println("ASSERTION FAILED(DEBUG) at line {} in file {}", \
				__LINE__, __FILE__);                                      \
		}                                                                 \
	} while (0)
#else
#ifdef NDEBUG
#define FUNC_AOV(x)                                                   \
	do {                                                              \
		if (!(x)) {                                                   \
			std::println("VERIFICATION FAILED at line {} in file {}", \
				__LINE__, __FILE__);                                  \
		}                                                             \
	} while (0)
#endif
#endif

/**
 * @def FUNC_AOV_EX
 * @brief Stands for "Assert Or Verify Ex."
 *
 * @note The difference between FUNC_AOV and FUNC_AOV_EX is that FUNC_AOV_EX
 * allows you to provide a message
 */
#ifdef DEBUG
#define FUNC_AOV_EX(x, str)                        \
	do {                                           \
		if (!(x)) {                                \
			std::println("VERIFICATION FAILED: "); \
			FUNC_INF(str);                         \
		}                                          \
	} while (0)
#else
#ifdef NDEBUG
#define FUNC_AOV_EX(x, str)                                  \
	do {                                                     \
		if (!(x)) {                                          \
			FUNC_FATAL("VERIFICATION FAILED, description:"); \
			FUNC_INF(str);                                   \
		}                                                    \
	} while (0)
#endif
#endif

/**
 * @def FUNC_DASS
 * @brief Stands for "Debug Assert"
 *
 * Only present in DEBUG builds. Has no effect in release builds.
 */
#ifdef DEBUG
#define FUNC_DASS(x)                                                        \
	do {                                                                    \
		if (!(x)) {                                                         \
			std::println("STRIPPED ASSERTION FAILED at line {} in file {}", \
				__LINE__, __FILE__);                                        \
		}                                                                   \
	} while (0)
#else
#define FUNC_DASS(x)
#endif

/**
 * @def PUSH_LOG
 * @brief Internal utility macro for pushing a string to s_Logs
 */
#define PUSH_LOG(prefix, ...)                                             \
	do {                                                                  \
		s_Logs.emplace_back(std::string(prefix) + FUNC_FMT(__VA_ARGS__)); \
	} while (0)

/**
 * @def FUNC_DBG
 * @brief Debug log, only present in debug builds.
 */
#ifdef DEBUG
#define FUNC_DBG(...)                                                      \
	do {                                                                   \
		std::println("\033[36m[Debug]: {}\033[0m", FUNC_FMT(__VA_ARGS__)); \
		PUSH_LOG("[Debug]: ", __VA_ARGS__);                                \
	} while (0)
#else
#define FUNC_DBG(...)
#endif

/**
 * @def FUNC_INF
 * @brief Info log.
 */
#define FUNC_INF(...)                                                     \
	do {                                                                  \
		std::println("\033[34m[Info]: {}\033[0m", FUNC_FMT(__VA_ARGS__)); \
		PUSH_LOG("[Info]: ", __VA_ARGS__);                                \
	} while (0)

/**
 * @def FUNC_WARN
 * @brief Warning log.
 */
#define FUNC_WARN(...)                                                    \
	do {                                                                  \
		std::println("\033[33m[Warn]: {}\033[0m", FUNC_FMT(__VA_ARGS__)); \
		PUSH_LOG("[Warn]: ", __VA_ARGS__);                                \
	} while (0)

/**
 * @def FUNC_GRAY
 * @brief Note log.
 */
#define FUNC_GRAY(...)                                                    \
	do {                                                                  \
		std::println("\033[90m[Note]: {}\033[0m", FUNC_FMT(__VA_ARGS__)); \
		PUSH_LOG("[Note]: ", __VA_ARGS__);                                \
	} while (0)

/**
 * @def FUNC_ERR
 * @brief A non-fatal, recoverable from error.
 */
#define FUNC_ERR(...)                                                        \
	do {                                                                     \
		std::println("\033[1;35m[Error]: {}\033[0m", FUNC_FMT(__VA_ARGS__)); \
		PUSH_LOG("[Error]: ", __VA_ARGS__);                                  \
	} while (0)

/**
 * @def FUNC_FATAL
 * @brief Same as FUNC_ERR, but exits directly after logging.
 */
#define FUNC_FATAL(...)                                                      \
	do {                                                                     \
		std::println("\033[1;31m[FATAL]: {}\033[0m", FUNC_FMT(__VA_ARGS__)); \
		PUSH_LOG("[FATAL]: ", __VA_ARGS__);                                  \
		std::exit(-1);                                                       \
	} while (0)

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <shellapi.h>
#include <windows.h>

#undef LoadImage
#undef PlaySound

#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

/**
 * @fn OpenFileExplorer
 * @brief Opens a path in the platform file explorer.
 */
inline void OpenFileExplorer(const std::filesystem::path& path) {
#if defined(_WIN32)
	ShellExecuteA(
		nullptr, "open", path.string().c_str(), nullptr, nullptr, SW_SHOW);
#elif defined(__APPLE__)
	std::system(("open " + path.string() + " &").c_str());
#elif defined(__linux__)
	std::system(("xdg-open " + path.string() + " &").c_str());
#else
	FUNC_ERR("Unsupported platform!");
#endif
}
