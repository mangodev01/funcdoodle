/**
 * @file Frame.h
 * @brief Defines pixel color structures and frame/image data containers.
 *
 * This file provides the core low-level image representation used by
 * FuncDoodle, including:
 * - Col: an RGB8 color struct with utility operations
 * - ImageArray: a 2D pixel buffer backed by a flat vector
 * - Frame: a higher-level wrapper around ImageArray with editing operations
 *
 * It supports basic image manipulation such as rotation, selection-based edits,
 * copying to clipboard, and exporting to disk using stb_image_write.
 *
 * These structures form the foundation of all rendering and pixel editing
 * functionality in the application.
 */

#pragma once

/**
 * @file Frame.h
 * @brief Defines pixel color structures and frame/image data containers.
 *
 * This file provides the core low-level image representation used by
 * FuncDoodle, including the Col struct (RGB8) and ImageArray (2D pixel grid).
 * Also defines the Frame class, which wraps ImageArray for animation frames.
 */

#include <cmath>
#include <iostream>
#include <utility>
#include <vector>

#include "Action/Direction.h"

#include "Constants.h"
#include "Ptr.h"
#include "Selection.h"

#include "stb_image_write.h"

namespace FuncDoodle {
	/**
	 * @struct Col
	 * @brief A struct holding an RGB8 color
	 */
	struct Col {
		public:
		unsigned char r = 255;  ///< Red channel in 8-bit RGB space.
		unsigned char g = 255;  ///< Green channel in 8-bit RGB space.
		unsigned char b = 255;  ///< Blue channel in 8-bit RGB space.

		/**
		 * @fn FromFloat3
		 * @brief Converts a normalized RGB float triplet into an 8-bit color.
		 *
		 * @param f Pointer to three normalized RGB float values.
		 * @return Converted 8-bit RGB color.
		 */
		static Col FromFloat3(const float* f) {
			Col col;

			col.r = (unsigned char)((f[0] * 255.0f) + 0.5f);
			col.g = (unsigned char)((f[1] * 255.0f) + 0.5f);
			col.b = (unsigned char)((f[2] * 255.0f) + 0.5f);

			return col;
		}

		/**
		 * @brief Returns whether two colors are identical.
		 *
		 * @param other Color to compare against.
		 * @return Whether both colors match.
		 */
		bool operator==(const Col& other) const {
			return r == other.r && g == other.g && b == other.b;
		}
		/**
		 * @brief Returns whether two colors differ.
		 *
		 * @param other Color to compare against.
		 * @return Whether the colors differ.
		 */
		bool operator!=(const Col& other) const { return !(*this == other); }
		/**
		 * @brief Orders colors lexicographically by RGB channels.
		 *
		 * @param other Color to compare against.
		 * @return Whether this color sorts before @p other.
		 */
		bool operator<(const Col& other) const {
			if (r != other.r)
				return r < other.r;
			if (g != other.g)
				return g < other.g;
			return b < other.b;
		}

		/**
		 * @brief Streams a color in human-readable form.
		 *
		 * @param stream Output stream to write into.
		 * @param col Color value to serialize.
		 * @return Reference to @p stream.
		 */
		friend std::ostream& operator<<(std::ostream& stream, const Col& col) {
			stream << "Col{" << (unsigned int)col.r << ", "
				   << (unsigned int)col.g << ", " << (unsigned int)col.b << "}";
			return stream;
		}
	};
	/**
	 * @class ImageArray
	 * @brief 2D array of RGB8 color pixels.
	 *
	 * Stores a width x height grid of Col (RGB8) values in a flat vector.
	 * Used by Frame to store pixel data. Supports bounds-checked get/set.
	 *
	 * @invariant Width() > 0 && Height() > 0
	 * @invariant m_Data.size() == Width() * Height()
	 *
	 * @note Thread-unsafe: no locking.
	 * @warning Accessing out of bounds is undefined behavior.
	 */
	class ImageArray {
		public:
		/** @brief Copies pixel data from another image array. */
		ImageArray& operator=(const ImageArray&) = default;
		/** @brief Moves pixel data from another image array. */
		ImageArray& operator=(ImageArray&&) = default;
		/** @brief Copies an existing image array. */
		ImageArray(const ImageArray&) = default;
		/** @brief Moves an existing image array. */
		ImageArray(ImageArray&&) = default;

		/**
		 * @brief Construct array with dimensions.
		 * @param width Must be > 0.
		 * @param height Must be > 0.
		 * @param bgCol Background color for unfilled pixels.
		 * @pre width > 0 && height > 0
		 */
		ImageArray(int width, int height, Col bgCol);
		~ImageArray();

		/**
		 * @fn RedoColorAdjustment
		 * @brief Reapplies background-color dependent adjustments after a color change.
		 *
		 * @param bgCol New background color.
		 */
		void RedoColorAdjustment(Col bgCol);
		/**
		 * @fn Resize
		 * @brief Resizes the backing pixel buffer to match current dimensions.
		 */
		void Resize();
		/**
		 * @fn Set
		 * @brief Sets a pixel color.
		 *
		 * @param x Pixel X coordinate.
		 * @param y Pixel Y coordinate.
		 * @param color New pixel color.
		 */
		void Set(int x, int y, const Col& color);
		/**
		 * @fn Get
		 * @brief Returns a pixel color.
		 *
		 * @param x Pixel X coordinate.
		 * @param y Pixel Y coordinate.
		 * @return Pixel color at the requested coordinate.
		 */
		[[nodiscard]] Col Get(int x, int y) const;

		/**
		 * @fn Width
		 * @brief Returns the image width in pixels.
		 *
		 * @return Image width.
		 */
		[[nodiscard]] int Width() const { return m_Width; }
		/**
		 * @fn SetWidth
		 * @brief Sets the image width.
		 *
		 * @param width New image width.
		 * @param clear Whether pixel contents should be cleared.
		 */
		void SetWidth(int width, bool clear = false) { m_Width = width; }
		/**
		 * @fn Height
		 * @brief Returns the image height in pixels.
		 *
		 * @return Image height.
		 */
		[[nodiscard]] int Height() const { return m_Height; }
		/**
		 * @fn SetHeight
		 * @brief Sets the image height.
		 *
		 * @param height New image height.
		 * @param clear Whether pixel contents should be cleared.
		 */
		void SetHeight(int height, bool clear = false) {
			m_Height = height;
		}
		/**
		 * @fn SetData
		 * @brief Replaces the raw pixel buffer.
		 *
		 * @param data New pixel buffer.
		 */
		void SetData(const std::vector<Col>& data) {
			this->m_Data = data;
		}
		/**
		 * @fn Data
		 * @brief Returns the raw pixel buffer.
		 *
		 * @return Const reference to pixel data.
		 */
		[[nodiscard]] const std::vector<Col>& Data() const { return m_Data; }
		/**
		 * @fn BgCol
		 * @brief Returns the image background color.
		 *
		 * @return Background color.
		 */
		[[nodiscard]] Col BgCol() const { return m_BG; }
		/**
		 * @fn SetBG
		 * @brief Sets the background color.
		 *
		 * @param bgCol New background color.
		 */
		void SetBG(const Col bgCol) { m_BG = bgCol; }

		private:
		int m_Width = g_DefaultCanvasWidth;
		int m_Height = g_DefaultCanvasHeight;
		std::vector<Col> m_Data;
		Col m_BG;
	};
	/**
	 * @class Frame
	 * @brief Wraps an ImageArray with higher-level frame editing operations.
	 */
	class Frame {
		public:
		Frame() : m_Pixels(1, 1, Col()) {}
		/** @brief Copies an existing frame. @param other Frame to copy. */
		Frame(const Frame& other)  = default;
		/** @brief Creates a frame with explicit dimensions and background color. @param width Frame width. @param height Frame height. @param bgCol Background color. */
		Frame(int width, int height, Col bgCol)
			: m_Pixels(width, height, bgCol) {}
		/** @brief Creates a frame from an image array copy. @param arr Image array to copy. */
		Frame(ImageArray  arr) : m_Pixels(std::move(arr)) {}
		/** @brief Creates a frame from an optional image array pointer. @param arr Image array pointer to copy when non-null. */
		Frame(const ImageArray* arr)
			: m_Pixels(arr ? *arr : ImageArray(1, 1, Col())) {}

		~Frame() = default;

		/**
		 * @fn ReInit
		 * @brief Reinitializes the frame with new dimensions and background color.
		 *
		 * @param width New frame width.
		 * @param height New frame height.
		 * @param bgCol New background color.
		 */
		void ReInit(int width, int height, Col bgCol) {
			m_Pixels = ImageArray(width, height, bgCol);
		}
		/** @brief Copies another frame into this one. @param other Frame to copy. @return Reference to this frame. */
		Frame& operator=(const Frame& other);
		/** @brief Returns whether two frames contain identical pixel data. @param other Frame to compare against. @return Whether the frames match. */
		bool operator==(const Frame& other) const;
		/**
		 * @fn SetWidth
		 * @brief Sets the frame width.
		 *
		 * @param width New frame width.
		 * @param clear Whether to clear contents while resizing.
		 */
		void SetWidth(int width, bool clear = false);
		/**
		 * @fn SetHeight
		 * @brief Sets the frame height.
		 *
		 * @param height New frame height.
		 * @param clear Whether to clear contents while resizing.
		 */
		void SetHeight(int height, bool clear = false);

		/**
		 * @fn Rotate
		 * @brief Rotates the frame contents around the frame center.
		 *
		 * @param deg Rotation angle in degrees.
		 */
		void Rotate(int deg) {
			float rad = deg * M_PI / 180.0f;
			float cos_r = cos(rad);
			float sin_r = sin(rad);

			int w = m_Pixels.Width();
			int h = m_Pixels.Height();
			int newW = m_Pixels.Width();
			int newH = m_Pixels.Height();  // or calculate bounding box
			std::vector<Col> result(newW * newH);

			float cx = (w - 1) / 2.0f;
			float cy = (h - 1) / 2.0f;
			float ncx = (newW - 1) / 2.0f;
			float ncy = (newH - 1) / 2.0f;

			for (int y = 0; y < newH; y++) {
				for (int x = 0; x < newW; x++) {
					// Map output pixel to input coordinates
					float dx = x - ncx;
					float dy = y - ncy;
					float sx = (dx * cos_r) + (dy * sin_r) + cx;
					float sy = (-dx * sin_r) + (dy * cos_r) + cy;

					// Nearest neighbor
					auto isx = static_cast<int>(std::lround(sx));
					auto isy = static_cast<int>(std::lround(sy));
					if (isx >= 0 && isx < w && isy >= 0 && isy < h) {
						result[(y * newW) + x] = m_Pixels.Data()[(isy * w) + isx];
					}
				}
			}
			m_Pixels.SetData(result);
		}

		/**
		 * @fn RotateSelection
		 * @brief Rotates only the selected region.
		 *
		 * @param sel Selection to rotate.
		 * @param deg Rotation angle in degrees.
		 */
		void RotateSelection(WeakPtr<Selection> sel, int deg);
		/**
		 * @fn DeleteSelection
		 * @brief Replaces the selected region with background color.
		 *
		 * @param sel Selection to clear.
		 * @param bg Background color to write.
		 */
		void DeleteSelection(WeakPtr<Selection> sel, Col bg);
		/**
		 * @fn MoveSelection
		 * @brief Moves the selected region by one step.
		 *
		 * @param sel Selection to move.
		 * @param moveDir Direction to move in.
		 * @param bg Background color used to fill vacated pixels.
		 */
		void MoveSelection(WeakPtr<Selection> sel, Direction moveDir, Col bg);

		/**
		 * @fn CopyToClipboard
		 * @brief Copies the frame image to the system clipboard.
		 */
		void CopyToClipboard();
		/**
		 * @fn PastedFrame
		 * @brief Returns a frame created from clipboard image data.
		 *
		 * @return Newly allocated pasted frame, or nullptr on failure.
		 */
		static Frame* PastedFrame();
		/**
		 * @fn Export
		 * @brief Writes the frame to an image file.
		 *
		 * @param filePath Output image path.
		 */
		void Export(const char* filePath) const;

		/**
		 * @fn Pixels
		 * @brief Returns immutable access to the backing pixel array.
		 *
		 * @return Pointer to image data.
		 */
		[[nodiscard]] const ImageArray* Pixels() const { return &m_Pixels; }
		/**
		 * @fn PixelsMut
		 * @brief Returns mutable access to the backing pixel array.
		 *
		 * @return Pointer to mutable image data.
		 */
		ImageArray* PixelsMut() { return &m_Pixels; }
		/**
		 * @fn SetPixel
		 * @brief Sets a single frame pixel.
		 *
		 * @param x Pixel X coordinate.
		 * @param y Pixel Y coordinate.
		 * @param px New pixel color.
		 */
		void SetPixel(int x, int y, Col px) { m_Pixels.Set(x, y, px); }
		/**
		 * @fn Width
		 * @brief Returns the frame width in pixels.
		 *
		 * @return Frame width.
		 */
		[[nodiscard]] int Width() const { return m_Pixels.Width(); }
		/**
		 * @fn Height
		 * @brief Returns the frame height in pixels.
		 *
		 * @return Frame height.
		 */
		[[nodiscard]] int Height() const { return m_Pixels.Height(); }
		/**
		 * @fn Data
		 * @brief Returns a copy of the frame pixel buffer.
		 *
		 * @return Pixel buffer copy.
		 */
		[[nodiscard]] std::vector<Col> Data() const { return m_Pixels.Data(); }

		private:
		ImageArray m_Pixels;
	};
}  // namespace FuncDoodle
