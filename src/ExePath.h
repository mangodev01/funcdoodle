/**
 * @file ExePath.h
 * @brief Cross-platform utility for retrieving the current executable path.
 *
 * This header defines a small utility namespace that provides a function
 * for obtaining the full filesystem path of the running executable.
 *
 * Platform support:
 * - Windows: GetModuleFileNameA
 * - macOS: _NSGetExecutablePath
 * - Linux: readlink("/proc/self/exe")
 *
 * @note Internally uses a static buffer resized as needed on macOS.
 * @warning The returned pointer is to a static buffer and may be overwritten on subsequent calls.
 * @warning Not thread-safe due to shared static storage.
 */

#pragma once

#include <vector>

// ChatGPT wrote this code :)
// :(

#if defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(__linux__)
#include <unistd.h>
#endif

namespace ExePath {
	inline const char* Get() {
		static std::vector<char> buffer(1024);

#if defined(_WIN32)
		GetModuleFileNameA(nullptr, buffer.data(), buffer.size());
#elif defined(__APPLE__)
		uint32_t size = buffer.size();
		_NSGetExecutablePath(buffer.data(), &size);
		buffer.resize(size);
		_NSGetExecutablePath(buffer.data(), &size);
#elif defined(__linux__)
		ssize_t count =
			readlink("/proc/self/exe", buffer.data(), buffer.size());
		if (count != -1)
			buffer.resize(count);
#endif

		return buffer.data();
	}
}  // namespace ExePath
