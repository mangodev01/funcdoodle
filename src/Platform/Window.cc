#define GLFW_INCLUDE_NONE
#define TOML_EXCEPTIONS 0

#include "Window.h"

#include "AudioManager.h"
#include "MacroUtils.h"
#include "Test.h"
#include "Themes.h"
#include "imgui_impl_glfw.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <stb_image.h>

namespace FuncDoodle::Platform {
	bool g_GlfwInitted = false;

	Window::Window(WindowSpec spec) {
		m_CursorHidden = false;

		int monitorCount = 0;

		if (!g_GlfwInitted)
			InitGlfw();

		GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
		bool invalid = false;

		if (spec.Monitor < 0 || spec.Monitor >= monitorCount) {
			FUNC_WARN(
				"monitor " << spec.Monitor << " is invalid, using primary...");
			invalid = true;
		}

		GLFWmonitor* monitor = invalid ? nullptr : monitors[spec.Monitor];

		glfwWindowHint(GLFW_SCALE_TO_MONITOR, 1);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

		m_Handle =
			glfwCreateWindow(spec.Width, spec.Height, spec.Title, monitor, 0);

		if (!m_Handle) {
			const char* desc;
			int error = glfwGetError(&desc);

			// weird BUT i think its fine..? cos we std::exiting
			this->~Window();
			delete this;

			FUNC_FATAL("Failed to initialize GLFW window -- " << desc);
		}

		glfwMakeContextCurrent(m_Handle);

		glfwSwapInterval(0);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			FUNC_FATAL("Failed to initialize GLAD");
		}

		glfwSetWindowUserPointer(m_Handle, this);

		glfwSetDropCallback(m_Handle, GlfwDropTrampoline);
		glfwSetWindowCloseCallback(m_Handle, GlfwCloseTrampoline);

		PaError err = Pa_Initialize();
		if (err != paNoError) {
			FUNC_FATAL("Failed to initialize port audio: " +
					   (std::string)Pa_GetErrorText(err));
		}

		InitImGui();
	}

	void Window::GlfwDropTrampoline(
		GLFWwindow* glfwWin, int count, const char** paths) {
		Window* self = static_cast<Window*>(glfwGetWindowUserPointer(glfwWin));

		if (!self)
			return;

		if (self->mp_DropCallback)
			self->mp_DropCallback(self, count, paths);
	}

	void Window::GlfwCloseTrampoline(GLFWwindow* glfwWin) {
		Window* self = static_cast<Window*>(glfwGetWindowUserPointer(glfwWin));

		if (!self)
			return;

		if (self->mp_CloseCallback)
			self->mp_CloseCallback(self);
	}

	void Window::SetDropCallback(DropCallback cb) {
		mp_DropCallback = cb;
	}

	void Window::SetErrorCallback(ErrorCallback cb) {
		glfwSetErrorCallback(cb);
	}

	void Window::SetCloseCallback(CloseCallback cb) {
		mp_CloseCallback = cb;
	}

	void Window::SetTitle(const char* title) {
		glfwSetWindowTitle(m_Handle, title);
	}

	void Window::SetIcon(std::filesystem::path icon) {
		FUNC_DBG("Setting window icon to " << icon << "...");
		int width, height, chan;
		unsigned char* data =
			stbi_load(icon.string().c_str(), &width, &height, &chan, 0);

		if (data) {
			GLFWimage* glfwIcon = (GLFWimage*)malloc(sizeof(GLFWimage));
			glfwIcon->width = width;
			glfwIcon->height = height;
			glfwIcon->pixels = data;

			glfwSetWindowIcon(m_Handle, 1, glfwIcon);

			// TODO: somehow stbi_image_free(glfwIcon);
		} else {
			FUNC_WARN("Failed to read image data from window icon");
		}
	}

	void Window::SetShouldClose(bool shouldClose) {
		glfwSetWindowShouldClose(m_Handle, shouldClose);
	}

	bool Window::ShouldClose() {
		return glfwWindowShouldClose(m_Handle);
	}

	void Window::PollEvents() {
		glfwPollEvents();
	}

	void Window::GetFramebufferSize(int* width, int* height) {
		glfwGetFramebufferSize(m_Handle, width, height);
	}

	void Window::SwapBuffers() {
		glfwSwapBuffers(m_Handle);
	}

	void Window::InitGlfw() {
		// fix some leak regarding libdecor
		glfwInitHint(GLFW_WAYLAND_LIBDECOR, GLFW_WAYLAND_DISABLE_LIBDECOR);

		if (!glfwInit()) {
			const char* description;
			int error = glfwGetError(&description);
			FUNC_FATAL("Failed to initialize GLFW: " << error << " -- "
													 << description << "\n");
		}

		g_GlfwInitted = true;
	}

	void Window::InitImGui() {
		glfwMakeContextCurrent(m_Handle);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		FUNC_DBG("Window::InitImGui context created");

#ifdef FUNCDOODLE_BUILD_IMTESTS
		s_TestEngine = ImGuiTestEngine_CreateContext();

		ImGuiTestEngineIO& testIo = ImGuiTestEngine_GetIO(s_TestEngine);

		testIo.ConfigVerboseLevel = ImGuiTestVerboseLevel_Info;
		testIo.ConfigVerboseLevelOnError = ImGuiTestVerboseLevel_Debug;

		FuncDoodle_RegisterImTests();

		ImGuiTestEngine_Start(s_TestEngine, ImGui::GetCurrentContext());
#endif

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#ifndef TILING
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
#endif
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigErrorRecovery = true;
		io.ConfigWindowsMoveFromTitleBarOnly = true;
		float xScale, yScale;
		glfwGetWindowContentScale(m_Handle, &xScale, &yScale);
		float dpiScale = xScale;

		// Don't scale down on macOS - ImGui backends handle DPI scaling
		// properly
#ifndef __APPLE__
		ImGui::GetStyle().ScaleAllSizes(1.0f / dpiScale);
#endif

		// Setup default ImGui style (before LoadThemes so loaded themes
		// override this)
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(m_Handle, true);
		ImGui_ImplOpenGL3_Init("#version 140");
	}

	Window::~Window() {
		FuncDoodle::AudioManager::WaitForAllPlayback();
		Pa_Terminate();
		FuncDoodle::Themes::ClearThemes();

		// cleanup code
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();

#ifdef FUNCDOODLE_BUILD_IMTESTS
		ImGuiTestEngine_Stop(s_TestEngine);
#endif

		ImGui::DestroyContext();

#ifdef FUNCDOODLE_BUILD_IMTESTS
		ImGuiTestEngine_DestroyContext(s_TestEngine);
#endif

		glfwDestroyWindow(m_Handle);
		glfwTerminate();
	}

	void Window::SetCursorHidden(bool hidden) {
		// HACK: glfw on wayland doesn't seem to hide the os cursor properly,
		// so instead, set the cursor to a blank image which does work for some reason...
		// thanks wayland! very cool
		if (m_CursorHidden == hidden) return;
		m_CursorHidden = hidden;
		if (hidden) {
			unsigned char pixels[4] = { 0, 0, 0, 0 };
			GLFWimage img = { 1, 1, pixels };
			GLFWcursor* blank = glfwCreateCursor(&img, 0, 0);
			glfwSetCursor(m_Handle, blank);
			m_BlankCursor = blank;
		} else {
			glfwSetCursor(m_Handle, nullptr);
			if (m_BlankCursor) {
				glfwDestroyCursor(m_BlankCursor);
				m_BlankCursor = nullptr;
			}
		}
	}
}  // namespace FuncDoodle::Platform
