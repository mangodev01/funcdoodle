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
#include <iostream>
#include <string>
#include <vector>

/**
 * Global log storage.
 *
 * Stores log messages produced by logging macros.
 *
 * @warning Elements are raw pointers; lifetime is managed externally.
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
#define FUNC_DBG(...)                                                                        \
	do {                                                                                     \
		std::cout << "\033[36m[Debug]: " << FUNC_FMT(__VA_ARGS__) << "\033[0m" << std::endl; \
		PUSH_LOG("[Debug]: ", __VA_ARGS__);                                                  \
	} while (0)
#else
#define FUNC_DBG(...)
#endif

/**
 * @def FUNC_INF
 * @brief Info log.
 */
#define FUNC_INF(...)                                                                       \
	do {                                                                                    \
		std::cout << "\033[34m[Info]: " << FUNC_FMT(__VA_ARGS__) << "\033[0m" << std::endl; \
		PUSH_LOG("[Info]: ", __VA_ARGS__);                                                  \
	} while (0)

/**
 * @def FUNC_WARN
 * @brief Warning log.
 */
#define FUNC_WARN(...)                                                                      \
	do {                                                                                    \
		std::cout << "\033[33m[Warn]: " << FUNC_FMT(__VA_ARGS__) << "\033[0m" << std::endl; \
		PUSH_LOG("[Warn]: ", __VA_ARGS__);                                                  \
	} while (0)

/**
 * @def FUNC_GRAY
 * @brief Note log.
 */
#define FUNC_GRAY(...)                                                                      \
	do {                                                                                    \
		std::cout << "\033[90m[Note]: " << FUNC_FMT(__VA_ARGS__) << "\033[0m" << std::endl; \
		PUSH_LOG("[Note]: ", __VA_ARGS__);                                                  \
	} while (0)

/**
 * @def FUNC_ERR
 * @brief A non-fatal, recoverable from error.
 */
#define FUNC_ERR(...)                                                                          \
	do {                                                                                       \
		std::cout << "\033[1;35m[Error]: " << FUNC_FMT(__VA_ARGS__) << "\033[0m" << std::endl; \
		PUSH_LOG("[Error]: ", __VA_ARGS__);                                                    \
	} while (0)

/**
 * @def FUNC_FATAL
 * @brief Same as FUNC_ERR, but exits directly after logging.
 */
#define FUNC_FATAL(...)                                                                        \
	do {                                                                                       \
		std::cout << "\033[1;31m[FATAL]: " << FUNC_FMT(__VA_ARGS__) << "\033[0m" << std::endl; \
		PUSH_LOG("[FATAL]: ", __VA_ARGS__);                                                    \
		std::exit(-1);                                                                         \
	} while (0)

#include "Gui.h"

/**
 * @def INVERTED_IMCOL
 * @brief Produces an inverted opaque ImGui color from RGB components.
 */
#define INVERTED_IMCOL(r, g, b) IM_COL32(255 - (r), 255 - (g), 255 - (b), 255)

#ifdef _WIN32
#include <windows.h>
/**
 * @def OPEN_FILE_EXPLORER
 * @brief Opens a path in the platform file explorer.
 */
#define OPEN_FILE_EXPLORER(path) \
	ShellExecuteA(               \
		nullptr, "open", path.string().c_str(), nullptr, nullptr, SW_SHOW)
#undef LoadImage
#undef PlaySound
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#elif __APPLE__
#include <cstdlib>
/**
 * @def OPEN_FILE_EXPLORER
 * @brief Opens a path in the platform file explorer.
 */
#define OPEN_FILE_EXPLORER(path) \
	std::system(("open " + path.string() + " &").c_str())
#elif __linux__
#include <cstdlib>
/**
 * @def OPEN_FILE_EXPLORER
 * @brief Opens a path in the platform file explorer.
 */
#define OPEN_FILE_EXPLORER(path) \
	std::system(("xdg-open " + (path).string() + " &").c_str())
#else
/**
 * @def OPEN_FILE_EXPLORER
 * @brief Opens a path in the platform file explorer.
 */
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
