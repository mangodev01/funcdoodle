#include "TextUtil.h"

#include <algorithm>
#include <imgui.h>

namespace FuncDoodle {
	TextMetrics TextUtil::Measure(ImFont* font, float size, const char* text) {
		ImGui::PushFont(font, size);
		ImVec2 sizePx = ImGui::CalcTextSize(text);
		ImGui::PopFont();
		return {.Width = sizePx.x, .Height = sizePx.y};
	}

	float TextUtil::TextWidth(ImFont* font, float size, const char* text) {
		return Measure(font, size, text).Width;
	}

	float TextUtil::TextHeight(ImFont* font, float size, const char* text) {
		return Measure(font, size, text).Height;
	}

	float TextUtil::MaxWidth(ImFont* titleFont, float titleSize,
		const char* titleA, const char* titleB, ImFont* descFont,
		float descSize, const char* descA, const char* descB) {
		float t1 = TextWidth(titleFont, titleSize, titleA);
		float t2 = TextWidth(titleFont, titleSize, titleB);
		float d1 = TextWidth(descFont, descSize, descA);
		float d2 = TextWidth(descFont, descSize, descB);
		return std::max({t1, t2, d1, d2});
	}
}  // namespace FuncDoodle
