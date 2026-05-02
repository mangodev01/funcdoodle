/**
 * @file TextUtil.h
 * @brief Static utility functions for measuring ImGui text dimensions.
 *
 * This header provides helper functions for calculating text metrics
 * using ImGui fonts. It is primarily used for UI layout and alignment
 * logic within FuncDoodle.
 *
 * Functionality includes:
 * - Measuring text width and height
 * - Computing text bounding metrics
 * - Comparing multiple text blocks for maximum width calculations
 *
 * @note This class is purely static and must never be instantiated.
 * @warning Relies on ImGui font rendering data (ImFont).
 */

#pragma once

#include "Gui.h"

namespace FuncDoodle {
	/**
	 * @struct TextMetrics
	 * @brief Basic rect struct used for text measurement.
	 */
	struct TextMetrics {
		/**
		 * @var Width
		 * @brief Measured text width in pixels.
		 */
		float Width = 0.0f;
		/**
		 * @var Height
		 * @brief Measured text height in pixels.
		 */
		float Height = 0.0f;
	};

	/**
	 * @class TextUtil
	 * @brief Opaque,static text measurement utility functions
	 *
	 * @warning Should NEVER be constructed.
	 */
	class TextUtil {
		public:
		/**
		 * @fn Measure(ImFont* font, float size, const char* text)
		 * @brief Measures the width and height of a text string for a font and
		 * size.
		 */
		static TextMetrics Measure(ImFont* font, float size, const char* text);
		/**
		 * @fn TextWidth(ImFont* font, float size, const char* text)
		 * @brief Returns only the width of a text string.
		 */
		static float TextWidth(ImFont* font, float size, const char* text);
		/**
		 * @fn TextHeight(ImFont* font, float size, const char* text)
		 * @brief Returns only the height of a text string.
		 */
		static float TextHeight(ImFont* font, float size, const char* text);
		/**
		 * @fn MaxWidth(ImFont* titleFont, float titleSize, const char* titleA,
		 * const char* titleB, ImFont* descFont, float descSize, const char*
		 * descA, const char* descB)
		 * @brief Returns the widest of two title/description text pairings.
		 */
		static float MaxWidth(ImFont* titleFont, float titleSize,
			const char* titleA, const char* titleB, ImFont* descFont,
			float descSize, const char* descA, const char* descB);
	};
}  // namespace FuncDoodle
