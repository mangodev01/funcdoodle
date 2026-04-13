#pragma once

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include <filesystem>
#include <functional>

namespace FuncDoodle::Platform {
	extern bool g_GlfwInitted;

	struct WindowSpec {
			/// window width at startup
			int Width;

			/// window height at startup
			int Height;

			/// window title at startup
			const char* Title;

			/// monitor - 0 for primary, 1 for secondary, etc.
			int Monitor;
	};

	class Window {
		public:
			Window(WindowSpec spec);
			~Window();

			void SetTitle(const char* title);
			void SetIcon(std::filesystem::path icon);

			using DropCallback =
				std::function<void(Window*, int, const char**)>;
			void SetDropCallback(DropCallback cb);

			using ErrorCallback = void (*)(int err, const char* desc);
			void SetErrorCallback(ErrorCallback cb);

			void SetShouldClose(bool shouldClose);
			bool ShouldClose();

			void PollEvents();
			void GetFramebufferSize(int* width, int* height);
			void SwapBuffers();

			static void InitGlfw();
			void InitImGui();

		private:
			static void GlfwDropTrampoline(
				GLFWwindow* glfwWin, int count, const char** paths);

		protected:
			DropCallback mp_DropCallback;

		private:
			GLFWwindow* m_Handle;
			WindowSpec m_Spec;
	};
}  // namespace FuncDoodle::Platform
