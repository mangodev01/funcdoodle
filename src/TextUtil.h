#pragma once

struct ImFont;

namespace FuncDoodle {
	struct TextMetrics {
		float width = 0.0f;
		float height = 0.0f;
	};

	class TextUtil {
		public:
		static TextMetrics Measure(ImFont* font, float size, const char* text);
		static float TextWidth(ImFont* font, float size, const char* text);
		static float TextHeight(ImFont* font, float size, const char* text);
		static float MaxWidth(ImFont* titleFont, float titleSize,
			const char* titleA, const char* titleB, ImFont* descFont,
			float descSize, const char* descA, const char* descB);
	};
}  // namespace FuncDoodle
