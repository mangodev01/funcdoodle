/**
 * @file Window.h
 * @brief GLFW-based window abstraction for FuncDoodle.
 *
 * Provides a RAII wrapper around GLFWwindow, handling:
 * - Window lifecycle (create/destroy)
 * - Input/event polling
 * - Buffer swapping
 * - Cursor control
 * - File drop callbacks
 * - ImGui initialization
 */

#pragma once

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include <filesystem>
#include <functional>

namespace FuncDoodle::Platform {

	/**
	 * @brief Global flag indicating whether GLFW has been initialized.
	 */
	extern bool g_GlfwInitted;

	/**
	 * @struct WindowSpec
	 * @brief Initial configuration used to create a window.
	 */
	struct WindowSpec {

		/**
		 * @brief Window width at startup.
		 */
		int Width;

		/**
		 * @brief Window height at startup.
		 */
		int Height;

		/**
		 * @brief Window title at startup.
		 */
		const char* Title;

		/**
		 * @brief Monitor index (0 = primary, 1 = secondary, etc.).
		 */
		int Monitor;
	};

	/**
	 * @class Window
	 * @brief Abstraction over a GLFW window with input, rendering, and lifecycle utilities.
	 */
	class Window {
		public:
		/**
		 * @fn Window
		 * @brief Creates a window from an initial specification.
		 *
		 * @param spec Window creation parameters.
		 */
		Window(WindowSpec spec);
		~Window();

		/**
		 * @brief Sets the window title.
		 */
		void SetTitle(const char* title);

		/**
		 * @brief Sets the window icon from a file path.
		 */
		void SetIcon(std::filesystem::path icon);

		/**
		 * @typedef DropCallback
		 * @brief Callback type invoked for file-drop events.
		 */
		using DropCallback = std::function<void(Window*, int, const char**)>;

		/**
		 * @brief Sets callback for file drop events.
		 */
		void SetDropCallback(DropCallback cb);

		/**
		 * @typedef ErrorCallback
		 * @brief Callback type invoked for GLFW errors.
		 */
		using ErrorCallback = void (*)(int err, const char* desc);

		/**
		 * @brief Sets GLFW error callback.
		 */
		void SetErrorCallback(ErrorCallback cb);

		/**
		 * @typedef CloseCallback
		 * @brief Callback type invoked when the window is asked to close.
		 */
		using CloseCallback = std::function<void(Window*)>;

		/**
		 * @brief Sets callback for window close events.
		 */
		void SetCloseCallback(CloseCallback cb);

		/**
		 * @brief Requests the window to close.
		 */
		void SetShouldClose(bool shouldClose);

		/**
		 * @brief Returns whether the window should close.
		 */
		bool ShouldClose();

		/**
		 * @brief Polls OS and input events.
		 */
		void PollEvents();

		/**
		 * @brief Retrieves framebuffer size in pixels.
		 */
		void GetFramebufferSize(int* width, int* height);

		/**
		 * @brief Swaps front and back buffers.
		 */
		void SwapBuffers();

		/**
		 * @brief Enables or disables cursor visibility.
		 */
		void SetCursorHidden(bool hidden);

		/**
		 * @brief Returns whether the cursor is currently hidden.
		 */
		inline bool GetCursorHidden() const { return m_CursorHidden; }

		/**
		 * @brief Initializes GLFW (must be called once before creating windows).
		 */
		static void InitGlfw();

		/**
		 * @brief Initializes ImGui for this window.
		 */
		void InitImGui();

		private:
		static void GlfwDropTrampoline(GLFWwindow* glfwWin, int count, const char** paths);
		static void GlfwCloseTrampoline(GLFWwindow* glfwWin);

		protected:
		DropCallback mp_DropCallback;   ///< Stored file-drop callback.
		CloseCallback mp_CloseCallback; ///< Stored close callback.

		private:
		GLFWwindow* m_Handle;
		bool m_CursorHidden;
		WindowSpec m_Spec;
		GLFWcursor* m_BlankCursor = nullptr;
	};

}  // namespace FuncDoodle::Platform
