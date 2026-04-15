#pragma once

#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

extern std::vector<char*> s_Logs;

#if __cplusplus >= 202002L && defined(__cpp_lib_format)
#include <format>
#define FUNC_FORMAT_AVAILABLE 1
#else
#define FUNC_FORMAT_AVAILABLE 0
#endif

#if FUNC_FORMAT_AVAILABLE
#define FUNC_FMT(...) std::format(__VA_ARGS__)
#else
#include <cstdio>
#define FUNC_FMT(...) func_format_fallback(__VA_ARGS__)
namespace FuncDoodle {
	inline std::string func_format_fallback(const char* fmt) {
		return std::string(fmt);
	}
	template <typename... Args>
	inline std::string func_format_fallback(const char* fmt, Args... args) {
		char buf[256];
		snprintf(buf, sizeof(buf), fmt, args...);
		return std::string(buf);
	}
}  // namespace FuncDoodle
#endif

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

#define PUSH_LOG(prefix, x)                               \
	do {                                                  \
		std::ostringstream _oss;                          \
		_oss << prefix << x;                              \
		std::string _str = _oss.str();                    \
		char* _buf = new char[_str.size() + 1];           \
		std::memcpy(_buf, _str.c_str(), _str.size() + 1); \
		s_Logs.push_back(_buf);                           \
	} while (0)

#ifdef DEBUG
#define FUNC_DBG(x)                                                      \
	do {                                                                 \
		std::cout << "\033[36m[Debug]: " << x << "\033[0m" << std::endl; \
		PUSH_LOG("[Debug]: ", x);                                        \
	} while (0)
#else
#define FUNC_DBG(x)
#endif

#define FUNC_INF(x)                                                     \
	do {                                                                \
		std::cout << "\033[34m[Info]: " << x << "\033[0m" << std::endl; \
		PUSH_LOG("[Info]: ", x);                                        \
	} while (0)

#define FUNC_WARN(x)                                                    \
	do {                                                                \
		std::cout << "\033[33m[Warn]: " << x << "\033[0m" << std::endl; \
		PUSH_LOG("[Warn]: ", x);                                        \
	} while (0)

#define FUNC_GRAY(x)                                                    \
	do {                                                                \
		std::cout << "\033[90m[Note]: " << x << "\033[0m" << std::endl; \
		PUSH_LOG("[Note]: ", x);                                        \
	} while (0)

#define FUNC_ERR(x)                                                        \
	do {                                                                   \
		std::cout << "\033[1;35m[Error]: " << x << "\033[0m" << std::endl; \
		PUSH_LOG("[Error]: ", x);                                          \
	} while (0)

#define FUNC_FATAL(x)                                                      \
	do {                                                                   \
		std::cout << "\033[1;31m[FATAL]: " << x << "\033[0m" << std::endl; \
		PUSH_LOG("[FATAL]: ", x);                                          \
		std::exit(-1);                                                     \
	} while (0)

#include "Gui.h"

#define INVERTED_IMCOL(r, g, b) IM_COL32(255 - r, 255 - g, 255 - b, 255)

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

// funcdoodle now uses a semver-style version system
// its just semver + an optional '-dev' suffix for debug builds

#ifdef DEBUG
#define FUNCVER "0.1.2-dev"
#else
#define FUNCVER "0.1.2"
#endif

// .fdp version
#define FDPVERMAJOR 0
#define FDPVERMINOR 4
