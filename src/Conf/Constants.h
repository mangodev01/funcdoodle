#pragma once

#include <array>
#include <cstdint>
#include <imgui.h>

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
	 * @brief Supported file extensions for importing/exporting images
	 */
	constexpr const char* g_SupportedExtensionsForImporting =
		"png,jpg,jpeg,bmp,tga,psd,gif,hdr,pic,ppm,pgm,pnm";

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

	/**
	 * @brief QWERTY keyboard layout.
	 *
	 * Used for the keybinds popup.
	 */
	static constexpr std::array<ImGuiKey, 92> c_Qwerty = {
		ImGuiKey_Escape,
		ImGuiKey_F1,
		ImGuiKey_F2,
		ImGuiKey_F3,
		ImGuiKey_F4,
		ImGuiKey_F5,
		ImGuiKey_F6,
		ImGuiKey_F7,
		ImGuiKey_F8,
		ImGuiKey_F9,
		ImGuiKey_F10,
		ImGuiKey_F11,
		ImGuiKey_F12,
		ImGuiKey_End,
		ImGuiKey_F13,
		ImGuiKey_F14,
		ImGuiKey_F15,
		ImGuiKey_F16,
		ImGuiKey_F17,
		ImGuiKey_F18,
		ImGuiKey_F19,
		ImGuiKey_F20,
		ImGuiKey_F21,
		ImGuiKey_F22,
		ImGuiKey_F23,
		ImGuiKey_F24,
		ImGuiKey_End,
		ImGuiKey_GraveAccent,
		ImGuiKey_1,
		ImGuiKey_2,
		ImGuiKey_3,
		ImGuiKey_4,
		ImGuiKey_5,
		ImGuiKey_6,
		ImGuiKey_7,
		ImGuiKey_8,
		ImGuiKey_9,
		ImGuiKey_0,
		ImGuiKey_Minus,
		ImGuiKey_Equal,
		ImGuiKey_Backspace,
		ImGuiKey_End,
		ImGuiKey_Tab,
		ImGuiKey_Q,
		ImGuiKey_W,
		ImGuiKey_E,
		ImGuiKey_R,
		ImGuiKey_T,
		ImGuiKey_Y,
		ImGuiKey_U,
		ImGuiKey_I,
		ImGuiKey_O,
		ImGuiKey_P,
		ImGuiKey_LeftBracket,
		ImGuiKey_RightBracket,
		ImGuiKey_Backslash,
		ImGuiKey_End,
		ImGuiKey_CapsLock,
		ImGuiKey_A,
		ImGuiKey_S,
		ImGuiKey_D,
		ImGuiKey_F,
		ImGuiKey_G,
		ImGuiKey_H,
		ImGuiKey_J,
		ImGuiKey_K,
		ImGuiKey_L,
		ImGuiKey_Semicolon,
		ImGuiKey_Apostrophe,
		ImGuiKey_Enter,
		ImGuiKey_End,
		ImGuiKey_LeftShift,
		ImGuiKey_Z,
		ImGuiKey_X,
		ImGuiKey_C,
		ImGuiKey_V,
		ImGuiKey_B,
		ImGuiKey_N,
		ImGuiKey_M,
		ImGuiKey_Comma,
		ImGuiKey_Period,
		ImGuiKey_Slash,
		ImGuiKey_RightShift,
		ImGuiKey_End,
		ImGuiKey_LeftCtrl,
		ImGuiKey_LeftSuper,
		ImGuiKey_LeftAlt,
		ImGuiKey_Space,
		ImGuiKey_RightAlt,
		ImGuiKey_RightSuper,
		ImGuiKey_Menu,
		ImGuiKey_RightCtrl,
	};
}  // namespace FuncDoodle
