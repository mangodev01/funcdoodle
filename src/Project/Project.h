/**
 * @file Project.h
 * @brief Defines the ProjectFile class, which manages animation project data.
 *
 * This file contains the core representation of a FuncDoodle project,
 * including animation frames, metadata (name, author, description),
 * rendering configuration (width, height, FPS, background color),
 * and full serialization/deserialization logic for the `.fdp` project format.
 *
 * It also implements undo/redo functionality through Action-based stacks,
 * allowing reversible editing operations such as drawing, filling, and frame
 * edits.
 */

#pragma once

/**
 * @file Project.h
 * @brief Defines the ProjectFile class, which manages animation project data.
 *
 * This file contains the core representation of a FuncDoodle project,
 * including frame storage (LongIndexArray), metadata, and undo/redo stacks.
 * Handles serialization to/from .fdp binary format.
 */

#include "Project/DynArr.h"
#include "Project/Frame.h"
#include "Util/Ptr.h"

#include <concepts>
#include <filesystem>
#include <stack>

#include "Action/Action.h"

#include "Util/MacroUtils.h"

#include <algorithm>
#include <memory>

#include "Platform/Window.h"

namespace FuncDoodle {
	/**
	 * @class ProjectFile
	 * @brief Class that is responsible for storing, reading & saving users'
	 * project files
	 *
	 * Represents a user's project, handling all aspects of its lifecycle -
	 * including in-memory storage of project data, deserializing from disk, and
	 * serializing back to the .fdp binary format that is documented in
	 * ../doc/FDP.md
	 *
	 * @invariant m_Window != nullptr
	 * @invariant m_Width  > 0
	 * @invariant m_Height > 0
	 */
	class ProjectFile {
		public:
		/**
		 * @fn ProjectFile
		 * @brief Creates a new in-memory project file.
		 *
		 * @param name Animation name buffer.
		 * @param width Initial canvas width.
		 * @param height Initial canvas height.
		 * @param author Animation author buffer.
		 * @param fps Initial playback FPS.
		 * @param desc Animation description buffer.
		 * @param win Window used for title updates.
		 * @param bgCol Initial background color.
		 */
		ProjectFile(char name[256], int width, int height, char author[100],
			int fps, char desc[512], Platform::Window* win, Col bgCol);
		~ProjectFile();
		/**
		 * @fn AnimName
		 * @brief Returns the animation name.
		 *
		 * @return Animation name string.
		 */
		[[nodiscard]] const char* AnimName() const;
		/**
		 * @fn SetAnimName
		 * @brief Sets the animation name.
		 *
		 * @param name New animation name buffer.
		 */
		void SetAnimName(char name[256]);
		/**
		 * @fn AnimWidth
		 * @brief Returns the animation width in pixels.
		 *
		 * @return Canvas width.
		 */
		[[nodiscard]] int AnimWidth() const;
		/**
		 * @fn SetAnimWidth
		 * @brief Sets the animation width.
		 *
		 * @param width New canvas width.
		 * @param clear Whether to clear existing frame data.
		 */
		void SetAnimWidth(int width, bool clear = false);
		/**
		 * @fn AnimHeight
		 * @brief Returns the animation height in pixels.
		 *
		 * @return Canvas height.
		 */
		[[nodiscard]] int AnimHeight() const;
		/**
		 * @fn SetAnimHeight
		 * @brief Sets the animation height.
		 *
		 * @param height New canvas height.
		 * @param clear Whether to clear existing frame data.
		 */
		void SetAnimHeight(int height, bool clear = false);
		/**
		 * @fn AnimAuthor
		 * @brief Returns the animation author.
		 *
		 * @return Author string.
		 */
		[[nodiscard]] const char* AnimAuthor() const;
		/**
		 * @fn SetAnimAuthor
		 * @brief Sets the animation author.
		 *
		 * @param author New author buffer.
		 */
		void SetAnimAuthor(char* author);
		/**
		 * @fn AnimFPS
		 * @brief Returns the animation frame rate.
		 *
		 * @return Frames per second.
		 */
		[[nodiscard]] int AnimFPS() const;
		/**
		 * @fn SetAnimFPS
		 * @brief Sets the animation frame rate.
		 *
		 * @param FPS New frames-per-second value.
		 */
		void SetAnimFPS(int FPS);
		/**
		 * @fn AnimDesc
		 * @brief Returns the animation description.
		 *
		 * @return Description string.
		 */
		[[nodiscard]] const char* AnimDesc() const;
		/**
		 * @fn SetAnimDesc
		 * @brief Sets the animation description.
		 *
		 * @param desc New description buffer.
		 */
		void SetAnimDesc(char* desc);
		/**
		 * @fn AnimFrameCount
		 * @brief Returns the number of frames in the animation.
		 *
		 * @return Frame count.
		 */
		[[nodiscard]] unsigned long AnimFrameCount() const;
		/**
		 * @fn SetAnimFrameCount
		 * @brief Resizes the animation frame count.
		 *
		 * @param count New frame count.
		 */
		void SetAnimFrameCount(unsigned long count);
		/**
		 * @fn SetBgCol
		 * @brief Sets the project background color and rebuilds backing frame
		 * storage.
		 *
		 * @param bgCol Pointer to three normalized RGB float values.
		 */
		void SetBgCol(const float* bgCol) {
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
		/**
		 * @fn BgCol
		 * @brief Returns the current background color.
		 *
		 * @return Background color.
		 */
		[[nodiscard]] Col BgCol() const { return m_BG; }
		/**
		 * @fn LastSavePath
		 * @brief Returns the last saved file path.
		 *
		 * @return Last save path string.
		 */
		[[nodiscard]] const char* LastSavePath() const {
			return m_LastSavePath;
		};
		/**
		 * @fn AnimFrames
		 * @brief Returns the animation frame storage.
		 *
		 * @return Shared frame array.
		 */
		SharedPtr<LongIndexArray> AnimFrames();
		/**
		 * @fn Write
		 * @brief Serializes the project to disk.
		 *
		 * @param filePath Output project path.
		 */
		void Write(const char* fileName);
		/**
		 * @fn ReadAndPopulate
		 * @brief Loads project data from disk into this instance.
		 *
		 * @param filePath Project path to read.
		 */
		void ReadAndPopulate(const char* filePath);
		/**
		 * @fn Export
		 * @brief Exports the animation using the chosen format.
		 *
		 * @param filePath Output path.
		 * @param format Export format identifier.
		 */
		void Export(const char* filePath, int format);
		/**
		 * @fn Window
		 * @brief Returns the owning application window.
		 *
		 * @return Window pointer.
		 */
		[[nodiscard]] constexpr Platform::Window* Window() const {
			return m_Window;
		}
		/**
		 * @fn Saved
		 * @brief Returns whether the project matches the last saved state.
		 *
		 * @return Save-state flag.
		 */
		bool Saved() const { return m_Saved; }
		/**
		 * @fn DisplayAltFPS
		 * @brief Shows a temporary FPS value in the window title.
		 *
		 * @param fps FPS value to display.
		 */
		void DisplayAltFPS(double fps);
		/**
		 * @fn UpdateTitle
		 * @brief Refreshes the window title using current project state.
		 */
		void UpdateTitle();

		// Undo management
		/**
		 * @fn PushUndoable
		 * @brief Pushes a new undoable action and clears redo history.
		 *
		 * @param action Action instance to store on the undo stack.
		 */
		template <typename T>
			requires std::derived_from<std::remove_cvref_t<T>, Action>
		void PushUndoable(T&& action) {
			using U = std::remove_cvref_t<T>;

			ClearRedoStack();
			m_UndoStack.push(std::make_unique<U>(std::forward<T>(action)));
			m_Saved = false;
		}

		/**
		 * @fn Undo
		 * @brief Undoes the most recent action on the undo stack.
		 */
		void Undo();
		/**
		 * @fn Redo
		 * @brief Reapplies the most recent action on the redo stack.
		 */
		void Redo();

		/**
		 * @fn ClearRedoStack
		 * @brief Removes all actions from the redo stack.
		 */
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
