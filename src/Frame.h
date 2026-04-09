#pragma once

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "Action/Direction.h"
#include "Gui.h"

#include "Ptr.h"
#include "Selection.h"

#include "stb_image_write.h"

namespace FuncDoodle {
	class MoveSelectionAction;

	struct Col {
		public:
			unsigned char r = 255, g = 255, b = 255;

			static Col FromFloat3(const float* f) {
				Col col;

				col.r = (unsigned char)(f[0] * 255.0f + 0.5f);
				col.g = (unsigned char)(f[1] * 255.0f + 0.5f);
				col.b = (unsigned char)(f[2] * 255.0f + 0.5f);

				return col;
			}

			bool operator==(const Col& other) const {
				return r == other.r && g == other.g && b == other.b;
			}
			bool operator!=(const Col& other) const {
				return !(*this == other);
			}
			bool operator<(const Col& other) const {
				if (r != other.r)
					return r < other.r;
				if (g != other.g)
					return g < other.g;
				return b < other.b;
			}

			friend std::ostream& operator<<(
				std::ostream& stream, const Col& col) {
				stream << "Col{" << (unsigned int)col.r << ", "
					   << (unsigned int)col.g << ", " << (unsigned int)col.b
					   << "}";
				return stream;
			}
	};
	class ImageArray {
		public:
			ImageArray& operator=(const ImageArray&) = default;
			ImageArray& operator=(ImageArray&&) = default;
			ImageArray(const ImageArray&) = default;
			ImageArray(ImageArray&&) = default;
			ImageArray(int width, int height, Col bgCol);
			~ImageArray();

			void RedoColorAdjustment(Col bgCol);
			void Resize();
			void Set(int x, int y, const Col& color);
			Col Get(int x, int y) const;

			inline int Width() const { return m_Width; }
			inline void SetWidth(int width, bool clear = false) {
				m_Width = width;
			}
			inline int Height() const { return m_Height; }
			inline void SetHeight(int height, bool clear = false) {
				m_Height = height;
			}
			inline void SetData(const std::vector<Col>& data) {
				this->m_Data = data;
			}
			inline const std::vector<Col>& Data() const { return m_Data; }
			inline const Col BgCol() const { return m_BG; }
			inline void SetBG(const Col bgCol) { m_BG = bgCol; }

		private:
			int m_Width = 32;
			int m_Height = 32;
			std::vector<Col> m_Data;
			Col m_BG;
	};
	class Frame {
		public:
			Frame() : m_Pixels(1, 1, Col()) {}
			Frame(const Frame& other) : m_Pixels(other.m_Pixels) {}
			Frame(int width, int height, Col bgCol)
				: m_Pixels(width, height, bgCol) {}
			Frame(const ImageArray& arr) : m_Pixels(arr) {}
			Frame(const ImageArray* arr)
				: m_Pixels(arr ? *arr : ImageArray(1, 1, Col())) {}

			~Frame() = default;

			void ReInit(int width, int height, Col bgCol) {
				m_Pixels = ImageArray(width, height, bgCol);
			}
			Frame& operator=(const Frame& other);
			bool operator==(const Frame& other) const;
			void SetWidth(int width, bool clear = false);
			void SetHeight(int height, bool clear = false);

			void Rotate(int deg) {
				float rad = deg * M_PI / 180.0f;
				float cos_r = cos(rad);
				float sin_r = sin(rad);

				int w = m_Pixels.Width(), h = m_Pixels.Height();
				int newW = m_Pixels.Width(),
					newH = m_Pixels.Height();  // or calculate bounding box
				std::vector<Col> result(newW * newH);

				float cx = (w - 1) / 2.0f, cy = (h - 1) / 2.0f;
				float ncx = (newW - 1) / 2.0f, ncy = (newH - 1) / 2.0f;

				for (int y = 0; y < newH; y++) {
					for (int x = 0; x < newW; x++) {
						// Map output pixel to input coordinates
						float dx = x - ncx, dy = y - ncy;
						float sx = dx * cos_r + dy * sin_r + cx;
						float sy = -dx * sin_r + dy * cos_r + cy;

						// Nearest neighbor
						int isx = static_cast<int>(std::lround(sx));
						int isy = static_cast<int>(std::lround(sy));
						if (isx >= 0 && isx < w && isy >= 0 && isy < h) {
							result[y * newW + x] =
								m_Pixels.Data()[isy * w + isx];
						}
					}
				}
				m_Pixels.SetData(result);
			}

			void RotateSelection(WeakPtr<Selection> sel, int deg);
			void DeleteSelection(WeakPtr<Selection> sel, Col bg);
			void MoveSelection(
				WeakPtr<Selection> sel, Direction moveDir, Col bg);

			void CopyToClipboard();
			static Frame* PastedFrame();
			void Export(const char* filePath);

			inline const ImageArray* Pixels() const { return &m_Pixels; }
			inline ImageArray* PixelsMut() { return &m_Pixels; }
			inline void SetPixel(int x, int y, Col px) {
				m_Pixels.Set(x, y, px);
			}
			inline const int Width() const { return m_Pixels.Width(); }
			inline const int Height() const { return m_Pixels.Height(); }
			inline std::vector<Col> Data() const { return m_Pixels.Data(); }

		private:
			ImageArray m_Pixels;
	};
}  // namespace FuncDoodle
