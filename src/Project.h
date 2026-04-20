#pragma once

#include "DynArr.h"
#include "Frame.h"
#include "Ptr.h"

#include <concepts>
#include <filesystem>
#include <stack>

#include "Action/Action.h"

#include "MacroUtils.h"

#include <algorithm>
#include <memory>

#include "Platform/Window.h"

namespace FuncDoodle {
	class ProjectFile {
		public:
		ProjectFile(char name[256], int width, int height, char author[100],
			int fps, char desc[512], Platform::Window* win, Col bgCol);
		~ProjectFile();
		const char* AnimName() const;
		void SetAnimName(char name[256]);
		const int AnimWidth() const;
		void SetAnimWidth(int width, bool clear = false);
		const int AnimHeight() const;
		void SetAnimHeight(int height, bool clear = false);
		const char* AnimAuthor() const;
		void SetAnimAuthor(char* author);
		const int AnimFPS() const;
		void SetAnimFPS(int FPS);
		const char* AnimDesc() const;
		void SetAnimDesc(char* desc);
		const unsigned long AnimFrameCount() const;
		void SetAnimFrameCount(unsigned long count);
		inline void SetBgCol(const float* bgCol) {
			// Ensure bgCol is valid and has at least 3 elements
			if (bgCol) {
				m_BG = Col{.r = static_cast<unsigned char>(
							   std::clamp(bgCol[0] * 255, 0.0f, 255.0f)),
					.g = static_cast<unsigned char>(
						std::clamp(bgCol[1] * 255, 0.0f, 255.0f)),
					.b = static_cast<unsigned char>(
						std::clamp(bgCol[2] * 255, 0.0f, 255.0f))};
			}

			m_Frames =
				std::make_shared<LongIndexArray>(m_Width, m_Height, m_BG);

			m_Frames->PushBackEmpty();
		}
		inline const Col BgCol() const { return m_BG; }
		inline const char* LastSavePath() const { return m_LastSavePath; };
		SharedPtr<LongIndexArray> AnimFrames();
		void Write(const char* filePath);
		void ReadAndPopulate(const char* filePath);
		void Export(const char* filePath, int format);
		constexpr inline Platform::Window* Window() const { return m_Window; }
		inline bool Saved() { return m_Saved; }
		void DisplayAltFPS(double fps);
		void UpdateTitle();

		// Undo management
		template <typename T>
			requires std::derived_from<std::remove_cvref_t<T>, Action>
		void PushUndoable(T&& action) {
			using U = std::remove_cvref_t<T>;

			ClearRedoStack();
			m_UndoStack.push(std::make_unique<U>(std::forward<T>(action)));
			m_Saved = false;
		}

		void Undo();
		void Redo();

		void ClearRedoStack() {
			while (!m_RedoStack.empty()) {
				m_RedoStack.pop();
			}
		}

		private:
		char m_Name[256];  // 256
		int m_Width = 0;
		int m_Height = 0;
		char m_Author[100];	 // 100
		int m_FPS = 0;
		char m_Desc[512];  // 512
		SharedPtr<LongIndexArray> m_Frames;
		Platform::Window* m_Window;
		std::stack<UniquePtr<Action>> m_UndoStack;
		std::stack<UniquePtr<Action>> m_RedoStack;
		bool m_Saved = false;
		const char* m_LastSavePath = "";
		Col m_BG;
	};
}  // namespace FuncDoodle
