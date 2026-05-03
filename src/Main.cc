#include "Core/App.h"

int main(int argc, char** argv) {
	FuncDoodle::Application app;

	app.Run();

	// imo explicit return 0s in main are better than implicit ones :)
	return 0;
}
