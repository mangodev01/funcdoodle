/**
 * @file MacroUtils.h
 * @brief Logging system, assertion macros, platform utilities, and build/config macros.
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
 * @warning This file defines macros with side effects (logging, exiting, memory allocation).
 * @warning s_Logs stores raw pointers and requires external cleanup.
 *
 * @note Many macros behave differently depending on DEBUG / NDEBUG builds.
 */

#pragma once

#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

/**
 * Global log storage.
 *
 * Stores heap-allocated copies of log messages produced by logging macros.
 * Ownership and cleanup are handled elsewhere in the application lifecycle.
 *
 * @warning Elements are raw pointers; lifetime is managed externally.
 */
extern std::vector<char*> s_Logs;

#if __cplusplus >= 202002L && defined(__cpp_lib_format)
#include <format>
#define FUNC_FORMAT_AVAILABLE 1
#else
#define FUNC_FORMAT_AVAILABLE 0
#endif

#if __cplusplus < 202002L
#error "std::format is not supported by your compiler configuration. Please consider using -std=c++20 or later."
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
#define FUNC_AOV(x)                                                    \
	do {                                                               \
		if (!(x)) {                                                    \
			std::cout << "ASSERTION FAILED(DEBUG) at line" << __LINE__ \
					  << " in file " << __FILE__ << std::endl;         \
		}                                                              \
	} while (0)
#else
#ifdef NDEBUG
#define FUNC_AOV(x)                                                \
	do {                                                           \
		if (!(x)) {                                                \
			std::cout << "VERIFICATION FAILED at line" << __LINE__ \
					  << " in file " << __FILE__ << std::endl;     \
		}                                                          \
	} while (0)
#endif
#endif

/**
 * @def FUNC_AOV_EX
 * @brief Stands for "Assert Or Verify Ex."
 *
 * @note The difference between FUNC_AOV and FUNC_AOV_EX is that FUNC_AOV_EX allows you to provide a message
 */
#ifdef DEBUG
#define FUNC_AOV_EX(x, str)                                    \
	do {                                                       \
		if (!(x)) {                                            \
			std::cout << "VERIFICATION FAILED: " << std::endl; \
			FUNC_INF(str);                                     \
		}                                                      \
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
#define FUNC_DASS(x)                                                     \
	do {                                                                 \
		if (!(x)) {                                                      \
			std::cout << "STRIPPED ASSERTION FAILED at line" << __LINE__ \
					  << " in file " << __FILE__ << std::endl;           \
		}                                                                \
	} while (0)
#else
#define FUNC_DASS(x)
#endif

/**
 * @def PUSH_LOG
 * @brief Internal utility macro for pushing a string to s_Logs
 *
 * @warning Allocated memory using new char[]; everything in s_Logs should be cleared before program exits.
 */
#define PUSH_LOG(prefix, x)                               \
	do {                                                  \
		std::ostringstream _oss;                          \
		_oss << prefix << x;                              \
		std::string _str = _oss.str();                    \
		char* _buf = new char[_str.size() + 1];           \
		std::memcpy(_buf, _str.c_str(), _str.size() + 1); \
		s_Logs.push_back(_buf);                           \
	} while (0)

/**
 * @def FUNC_DBG
 * @brief Debug log, only present in debug builds.
 */
#ifdef DEBUG
#define FUNC_DBG(x)                                                      \
	do {                                                                 \
		std::cout << "\033[36m[Debug]: " << x << "\033[0m" << std::endl; \
		PUSH_LOG("[Debug]: ", x);                                        \
	} while (0)
#else
#define FUNC_DBG(x)
#endif

/**
 * @def FUNC_INF
 * @brief Info log.
 */
#define FUNC_INF(x)                                                     \
	do {                                                                \
		std::cout << "\033[34m[Info]: " << x << "\033[0m" << std::endl; \
		PUSH_LOG("[Info]: ", x);                                        \
	} while (0)


/**
 * @def FUNC_INF
 * @brief Info log.
 */
#define FUNC_WARN(x)                                                    \
	do {                                                                \
		std::cout << "\033[33m[Warn]: " << x << "\033[0m" << std::endl; \
		PUSH_LOG("[Warn]: ", x);                                        \
	} while (0)

/**
 * @def FUNC_GRAY
 * @brief Note log.
 */
#define FUNC_GRAY(x)                                                    \
	do {                                                                \
		std::cout << "\033[90m[Note]: " << x << "\033[0m" << std::endl; \
		PUSH_LOG("[Note]: ", x);                                        \
	} while (0)

/**
 * @def FUNC_ERR
 * @brief A non-fatal, recoverable from error.
 */
#define FUNC_ERR(x)                                                        \
	do {                                                                   \
		std::cout << "\033[1;35m[Error]: " << x << "\033[0m" << std::endl; \
		PUSH_LOG("[Error]: ", x);                                          \
	} while (0)

/**
 * @def FUNC_FATAL
 * @brief Same as FUNC_ERR, but exits directly after logging.
 */
#define FUNC_FATAL(x)                                                      \
	do {                                                                   \
		std::cout << "\033[1;31m[FATAL]: " << x << "\033[0m" << std::endl; \
		PUSH_LOG("[FATAL]: ", x);                                          \
		std::exit(-1);                                                     \
	} while (0)

#include "Gui.h"

#define INVERTED_IMCOL(r, g, b) IM_COL32(255 - (r), 255 - (g), 255 - (b), 255)

#ifdef _WIN32
#include <windows.h>
#define OPEN_FILE_EXPLORER(path) \
	ShellExecuteA(               \
		nullptr, "open", path.string().c_str(), nullptr, nullptr, SW_SHOW)
#undef LoadImage
#undef PlaySound
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#elif __APPLE__
#include <cstdlib>
#define OPEN_FILE_EXPLORER(path) \
	std::system(("open " + path.string() + " &").c_str())
#elif __linux__
#include <cstdlib>
#define OPEN_FILE_EXPLORER(path) \
	std::system(("xdg-open " + path.string() + " &").c_str())
#else
#define OPEN_FILE_EXPLORER(path) \
	std::cerr << "Unsupported platform!" << std::endl
#endif

/**
 * @def FUNCVER
 * @brief FuncDoodle version
 *
 * FuncDoodle now uses a semver-style version system
 * its just semver + an optional '-dev' suffix for debug builds
 */

#ifdef DEBUG
#define FUNCVER "0.1.2-dev"
#else
#define FUNCVER "0.1.2"
#endif

/**
 * @def FDPVERMAJOR
 * @brief FuncDoodle project file format major version.
 *
 * Indicates breaking changes in the .fdp file format.
 */
#define FDPVERMAJOR 0

/**
 * @def FDPVERMINOR
 * @brief FuncDoodle project file format minor version.
 *
 * Indicates minor changes in the .fdp file format.
 */
#define FDPVERMINOR 4
