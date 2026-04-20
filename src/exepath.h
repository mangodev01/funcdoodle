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

namespace exepath {
	inline const char* get() {
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
}  // namespace exepath
