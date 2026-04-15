#pragma once

#include <cstdint>

namespace FuncDoodle {
	constexpr uint8_t MAX_COLOR_VALUE = 255;
	constexpr uint8_t COLOR_WHITE = 255;
	constexpr uint8_t ALPHA_OPAQUE = 255;
	constexpr uint8_t COLOR_GRAY_RGB = 150;
	constexpr int BUFFER_CHUNK_SIZE = 32;
	constexpr int DEFAULT_CANVAS_WIDTH = 32;
	constexpr int DEFAULT_CANVAS_HEIGHT = 32;
	constexpr int TOOLBAR_HEIGHT = 32;
	constexpr int DEFAULT_WINDOW_WIDTH = 1920;
	constexpr int DEFAULT_WINDOW_HEIGHT = 1080;
	constexpr int TIMELINE_WIDTH = 1074;
	constexpr int TIMELINE_HEIGHT = 160;
	constexpr int MENU_NAME_BUFFER_SIZE = 32;
	constexpr int FILE_PATH_BUFFER_SIZE = 512;
	constexpr int LARGE_BUFFER_SIZE = 1024;
	constexpr int KEYBIND_BUFFER_SIZE = 1024;
	constexpr int KEY_NAME_BUFFER_SIZE = 256;
	constexpr float MAX_ROTATION_DEG = 360.0f;
	constexpr int HALF_ALPHA = 128;
	constexpr int DASH_COLOR_PRIMARY = 150;
	constexpr int DASH_COLOR_SECONDARY = 255;
	constexpr int UUID_RANDOM_RANGE = 255;
	constexpr double MIN_DELTA_TIME = 1.0 / 1000.0;
	constexpr double MAX_DELTA_TIME = 1.0;
	constexpr float DEFAULT_FPS = 60.0f;
	constexpr float FRAME_TIME_DIVISOR = 1000.0;
}  // namespace FuncDoodle