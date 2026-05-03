#pragma once

#include <cstdint>

/**
 * @file Constants.h
 * @brief Global constants used throughout FuncDoodle.
 *
 * This file defines commonly used fixed values such as UI sizes,
 * buffer limits, color constants, timing constraints, and other
 * application-wide configuration constants.
 *
 * These values are intended to remain immutable at runtime and
 * provide a single source of truth for engine-wide parameters.
 */

namespace FuncDoodle {
	/**
	 * @brief Maximum value for an 8-bit color channel.
	 */
	constexpr uint8_t g_MaxColorValue = 255;

	/**
	 * @brief Fully white RGB channel value.
	 */
	constexpr uint8_t g_ColorWhite = 255;

	/**
	 * @brief Fully opaque alpha value.
	 */
	constexpr uint8_t g_AlphaOpaque = 255;

	/**
	 * @brief Default gray color value in RGB space.
	 */
	constexpr uint8_t g_ColorGrayRgb = 150;

	/**
	 * @brief Size of an internal buffer chunk (used for batching/processing).
	 */
	constexpr int g_BufferChunkSize = 32;

	/**
	 * @brief Default canvas width in pixels.
	 */
	constexpr int g_DefaultCanvasWidth = 32;

	/**
	 * @brief Default canvas height in pixels.
	 */
	constexpr int g_DefaultCanvasHeight = 32;

	/**
	 * @brief Height of the application toolbar in pixels.
	 */
	constexpr int g_ToolbarHeight = 32;

	/**
	 * @brief Default application window width in pixels.
	 */
	constexpr int g_DefaultWindowWidth = 1920;

	/**
	 * @brief Default application window height in pixels.
	 */
	constexpr int g_DefaultWindowHeight = 1080;

	/**
	 * @brief Width of the timeline UI panel.
	 */
	constexpr int g_TimelineWidth = 1074;

	/**
	 * @brief Height of the timeline UI panel.
	 */
	constexpr int g_TimelineHeight = 160;

	/**
	 * @brief Maximum buffer size for menu name strings.
	 */
	constexpr int g_MenuNameBufferSize = 32;

	/**
	 * @brief Maximum buffer size for file path strings.
	 */
	constexpr int g_FilePathBufferSize = 512;

	/**
	 * @brief Large general-purpose buffer size for temporary storage.
	 */
	constexpr int g_LargeBufferSize = 1024;

	/**
	 * @brief Buffer size for keybind serialization/deserialization.
	 */
	constexpr int g_KeybindBufferSize = 1024;

	/**
	 * @brief Buffer size for storing key name strings.
	 */
	constexpr int g_KeyNameBufferSize = 256;

	/**
	 * @brief Maximum rotation angle in degrees.
	 */
	constexpr float g_MaxRotationDeg = 360.0f;

	/**
	 * @brief Half alpha value (semi-transparent).
	 */
	constexpr int g_HalfAlpha = 128;

	/**
	 * @brief Primary dash color intensity (used in UI/grid rendering).
	 */
	constexpr int g_DashColorPrimary = 150;

	/**
	 * @brief Secondary dash color intensity (used in UI/grid rendering).
	 */
	constexpr int g_DashColorSecondary = 255;

	/**
	 * @brief Maximum range used for UUID random generation byte selection.
	 */
	constexpr int g_UuidRandomRange = 255;

	/**
	 * @brief Minimum allowed delta time between frames (seconds).
	 */
	constexpr double g_MinDeltaTime = 1.0 / 1000.0;

	/**
	 * @brief Maximum allowed delta time between frames (seconds).
	 */
	constexpr double g_MaxDeltaTime = 1.0;

	/**
	 * @brief Default target FPS for timing calculations.
	 */
	constexpr float g_DefaultFps = 60.0f;

	/**
	 * @brief Divisor used when converting milliseconds to frame time.
	 */
	constexpr float g_FrameTimeDivisor = 1000.0f;


#ifdef DEBUG
	inline constexpr bool c_Debug = true;
#else
	inline constexpr bool c_Debug = false;
#endif

	/**
	 * @fn FuncVer
	 * @note Helper for initializing c_FuncVer
	*/
	consteval const char* FuncVer() {
		if constexpr (c_Debug) {
			return "0.1.2-dev";
		} else {
			return "0.1.2";
		}
	}

	/**
	 * @brief The FuncDoodle version string.
	 *
	 * Evaluated entirely at compile-time via FuncVer().
	 */
	static constexpr const char* c_FuncVer = FuncVer();

	/**
	 * @brief FuncDoodle project file format major version.
	 *
	 * Indicates breaking changes in the .fdp file format.
	 */
	static constexpr uint8_t c_FdpVerMajor = 0;

	/**
	 * @brief FuncDoodle project file format minor version.
	 *
	 * Indicates minor changes in the .fdp file format.
	 */
	static constexpr uint8_t c_FdpVerMinor = 4;
}  // namespace FuncDoodle
