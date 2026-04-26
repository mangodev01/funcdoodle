#pragma once

#include <cstdint>

namespace FuncDoodle {
	constexpr uint8_t g_MaxColorValue = 255;
	constexpr uint8_t g_ColorWhite = 255;
	constexpr uint8_t g_AlphaOpaque = 255;
	constexpr uint8_t g_ColorGrayRgb = 150;
	constexpr int g_BufferChunkSize = 32;
	constexpr int g_DefaultCanvasWidth = 32;
	constexpr int g_DefaultCanvasHeight = 32;
	constexpr int g_ToolbarHeight = 32;
	constexpr int g_DefaultWindowWidth = 1920;
	constexpr int g_DefaultWindowHeight = 1080;
	constexpr int g_TimelineWidth = 1074;
	constexpr int g_TimelineHeight = 160;
	constexpr int g_MenuNameBufferSize = 32;
	constexpr int g_FilePathBufferSize = 512;
	constexpr int g_LargeBufferSize = 1024;
	constexpr int g_KeybindBufferSize = 1024;
	constexpr int g_KeyNameBufferSize = 256;
	constexpr float g_MaxRotationDeg = 360.0f;
	constexpr int g_HalfAlpha = 128;
	constexpr int g_DashColorPrimary = 150;
	constexpr int g_DashColorSecondary = 255;
	constexpr int g_UuidRandomRange = 255;
	constexpr double g_MinDeltaTime = 1.0 / 1000.0;
	constexpr double g_MaxDeltaTime = 1.0;
	constexpr float g_DefaultFps = 60.0f;
	constexpr float g_FrameTimeDivisor = 1000.0f;
}  // namespace FuncDoodle