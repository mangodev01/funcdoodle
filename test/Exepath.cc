#include "ExePath.h"
#include "Test.h"
#include <cstring>

int FuncDoodle_RunExePathTests() {
	TEST_SCOPE("ExePath Tests");

	const char* path = ExePath::Get();
	CHECK((path != nullptr), "get() should return non-null");
	CHECK((strlen(path) > 0), "get() should return non-empty string");
	CHECK((strstr(path, "/") != nullptr || strstr(path, "\\") != nullptr),
		"path should contain directory separator");

	return 0;
}
