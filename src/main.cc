#include "Themes.h"
#include "UUID.h"
#include "imgui_internal.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#define GLFW_INCLUDE_NONE
#define TOML_EXCEPTIONS 0
#include <GLFW/glfw3.h>
#include <filesystem>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <portaudio.h>
#include <stdio.h>

#include <chrono>
#include <cmath>

#include "App.h"
#include "AssetLoader.h"
#include "Audio.h"
#include "Test.h"

#include "LoadedAssets.h"

#include "MacroUtils.h"

#include <stb_image.h>

#include "exepath.h"

float SAMPLE_RATE = 44100.0;

#ifdef FUNCDOODLE_BUILD_TESTS
int FuncDoodle_RunTests();
#endif

#ifdef FUNCDOODLE_BUILD_IMTESTS
int FuncDoodle_RegisterImTests();
ImGuiTestEngine* s_TestEngine;
#endif

void GLFWErrorCallback(int error, const char* desc) {
	FUNC_ERR("GLFW ERROR (" << error << "): " << desc);
}

void GlobalAppTick(GLFWwindow* win,
	std::chrono::high_resolution_clock::time_point& lastFrameTime,
	FuncDoodle::Application* application, ImGuiIO& io) {
	auto currentFrameTime = std::chrono::high_resolution_clock::now();
	auto deltaTime =
		std::chrono::duration<double>(currentFrameTime - lastFrameTime).count();

	if (deltaTime >= application->FrameTime()) {
		lastFrameTime = currentFrameTime;
		glfwPollEvents();

		// Start the ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::DockSpaceOverViewport(0U, ImGui::GetMainViewport(),
			ImGuiDockNodeFlags_PassthruCentralNode);

		application->RenderImGui();

		// Rendering
		int display_w, display_h;
		glfwGetFramebufferSize(win, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0.0f, 0.0f, 0.0f, 1.00f);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow* backup = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup);
		}

		application->UpdateFPS(deltaTime);

		glfwSwapBuffers(win);

#ifdef FUNCDOODLE_BUILD_IMTESTS
		ImGuiTestEngine_PostSwap(s_TestEngine);
#endif
	}
}

GLFWimage* GlobalLoadWinImage(const std::filesystem::path& assetsPath) {
	FUNC_INF(assetsPath / "icon.png");
	std::filesystem::path icon = assetsPath / "icon.png";
	int width, height, chan;
	unsigned char* data =
		stbi_load(icon.string().c_str(), &width, &height, &chan, 0);
	if (data) {
		GLFWimage* icon = (GLFWimage*)malloc(sizeof(GLFWimage));
		icon->width = width;
		icon->height = height;
		icon->pixels = data;
		return icon;
	} else {
		FUNC_WARN("Failed to read image data from window icon");
		return nullptr;
	}
}

int main(int argc, char** argv) {
#ifdef FUNCDOODLE_BUILD_TESTS
	(void)argc;
	(void)argv;
	FuncDoodle_RunTests();
	FuncDoodle::TestRegistry::Instance().PrintSummary();

	return 0;
#endif

	const char* path = exepath::get();
	FUNC_DBG("Starting funcdoodle with exe path: " << path);
	const char* lastSlash = strrchr(path, '/');
	if (!lastSlash) {
		lastSlash = strrchr(path, '\\');
	}
	char dirPath[1024];
	if (lastSlash) {
		size_t len = lastSlash - path;
		strncpy(dirPath, path, len);
		dirPath[len] = '\0';
	} else {
		strncpy(dirPath, path, sizeof(dirPath) - 1);
		dirPath[sizeof(dirPath) - 1] = '\0';
	}

	std::filesystem::path rootPath(dirPath);

	std::filesystem::path assetsPath(rootPath);
	assetsPath /= "assets";

	std::filesystem::path themesPath(rootPath);
	themesPath /= "themes";

#ifdef DEBUG
	glfwSetErrorCallback(GLFWErrorCallback);
#endif

	// fix some leak regarding libdecor
	glfwInitHint(GLFW_WAYLAND_LIBDECOR, GLFW_WAYLAND_DISABLE_LIBDECOR);

	if (!glfwInit()) {
		const char* description;
		int error = glfwGetError(&description);
		fprintf(stderr, "Failed to initialize GLFW: %d -- %s\n", error,
			description);
		return -1;
	}

	glfwWindowHint(GLFW_SCALE_TO_MONITOR, 1);
#ifdef __APPLE__
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif

	GLFWwindow* win = glfwCreateWindow(900, 900, "FuncDoodle", NULL, NULL);
	if (!win) {
		const char* desc;
		int error = glfwGetError(&desc);
		fprintf(stderr, "Failed to initialize GLFW window -- %s\n", desc);
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(win);

	glfwSwapInterval(0);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		fprintf(stderr, "Failed to initialize GLAD\n");
		return -1;
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

#ifdef FUNCDOODLE_BUILD_IMTESTS
	s_TestEngine = ImGuiTestEngine_CreateContext();

	ImGuiTestEngineIO& test_io = ImGuiTestEngine_GetIO(s_TestEngine);

	test_io.ConfigVerboseLevel = ImGuiTestVerboseLevel_Info;
	test_io.ConfigVerboseLevelOnError = ImGuiTestVerboseLevel_Debug;

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
	glfwGetWindowContentScale(win, &xScale, &yScale);
	float dpiScale = xScale;
	ImFontConfig fontConfig;
	fontConfig.OversampleH = 4;
	fontConfig.OversampleV = 4;
	fontConfig.PixelSnapH = true;

	io.Fonts->AddFontFromFileTTF(
		(assetsPath / "Roboto" / "Roboto-Medium.ttf").string().c_str(), 16.0,
		&fontConfig);

	ImGui::GetStyle().ScaleAllSizes(1.0f / dpiScale);

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	glfwSetWindowCloseCallback(win, [](GLFWwindow* win) {});

	FuncDoodle::AssetLoader assetLoader(assetsPath);

	FuncDoodle::Themes::LoadThemes(themesPath);

	UniquePtr<FuncDoodle::Application> application =
		std::make_unique<FuncDoodle::Application>(
			win, &assetLoader, themesPath, rootPath);

	ImGuiSettingsHandler handler;
	handler.TypeName = "UserData";
	handler.TypeHash = ImHashStr(handler.TypeName);
	handler.UserData = application.get();
	handler.ReadOpenFn = [](ImGuiContext*, ImGuiSettingsHandler* handler,
							 const char* val) -> void* {
		if (std::strcmp(val, "Preferences") == 0) {
			return handler->UserData;
		}
		return nullptr;
	};
	handler.ReadLineFn = [](ImGuiContext*, ImGuiSettingsHandler*, void* entry,
							 const char* line) {
		char sel[37] = {0};
		if (std::sscanf(line, "Theme=\"%36s\"", sel) == 1) {
			static_cast<FuncDoodle::Application*>(entry)->SetTheme(
				FuncDoodle::UUID::FromString(sel));
		}
		int sfxEnabled;
		if (std::sscanf(line, "Sfx=%d", &sfxEnabled) == 1) {
			bool sfxEnabledBool = false;
			if (sfxEnabled >= 1) {
				sfxEnabledBool = true;
			}
			static_cast<FuncDoodle::Application*>(entry)->SetSFXEnabled(
				sfxEnabledBool);
		}
		int prevEnabled;
		if (std::sscanf(line, "Prev=%d", &prevEnabled) == 1) {
			bool prevEnabledBool = false;
			if (prevEnabled >= 1)
				prevEnabledBool = true;
			static_cast<FuncDoodle::Application*>(entry)->SetPrevEnabled(
				prevEnabledBool);
		}
		int undoByStroke;
		if (std::sscanf(line, "UndoByStroke=%d", &undoByStroke) == 1) {
			bool undoByStrokeBool = false;
			if (undoByStroke >= 1) {
				undoByStrokeBool = true;
			}
			static_cast<FuncDoodle::Application*>(entry)->SetUndoByStroke(
				undoByStrokeBool);
		}

		double frameLimit;
		if (std::sscanf(line, "FrameLimit=%lf", &frameLimit) == 1) {
			static_cast<FuncDoodle::Application*>(entry)->SetFrameLimit(
				frameLimit);
		}

		FuncDoodle::UUID uuid =
			static_cast<FuncDoodle::Application*>(entry)->Theme();
		if (!FuncDoodle::Themes::g_Themes.contains(uuid)) {
			ImGui::GetStyle() =
				FuncDoodle::Themes::g_Themes
					[FuncDoodle::UUID::FromString(
						 "d0c1a009-d09c-4fe6-84f8-eddcb2da38f9")]
						.Style;
			ImGui::GetStyle().Alpha = 1.0f;	 // Fully opaque
			ImGui::GetStyle().WindowRounding = 10.0f;
			ImGui::GetStyle().FrameRounding = 5.0f;
			ImGui::GetStyle().PopupRounding = 12.0f;
			ImGui::GetStyle().ScrollbarRounding = 10.0f;
			ImGui::GetStyle().GrabRounding = 6.0f;
			ImGui::GetStyle().TabRounding = 12.0f;
			ImGui::GetStyle().ChildRounding = 12.0f;
			ImGui::GetStyle().WindowPadding = ImVec2(10, 10);
			ImGui::GetStyle().FramePadding = ImVec2(8, 8);
			ImGui::GetStyle().ItemSpacing = ImVec2(10, 10);
			ImGui::GetStyle().IndentSpacing = 20.0f;
			ImGui::GetStyle().ScrollbarSize = 16.0f;
			if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
				ImGui::GetStyle().WindowRounding = 1.0f;
			}
			return;
		}
		ImGui::GetStyle() = FuncDoodle::Themes::g_Themes[uuid].Style;
		ImGui::GetStyle().Alpha = 1.0f;	 // Fully opaque
		ImGui::GetStyle().WindowRounding = 10.0f;
		ImGui::GetStyle().FrameRounding = 5.0f;
		ImGui::GetStyle().PopupRounding = 12.0f;
		ImGui::GetStyle().ScrollbarRounding = 10.0f;
		ImGui::GetStyle().GrabRounding = 6.0f;
		ImGui::GetStyle().TabRounding = 12.0f;
		ImGui::GetStyle().ChildRounding = 12.0f;
		ImGui::GetStyle().WindowPadding = ImVec2(10, 10);
		ImGui::GetStyle().FramePadding = ImVec2(8, 8);
		ImGui::GetStyle().ItemSpacing = ImVec2(10, 10);
		ImGui::GetStyle().IndentSpacing = 20.0f;
		ImGui::GetStyle().ScrollbarSize = 16.0f;
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::GetStyle().WindowRounding = 1.0f;
		}
	};
	handler.WriteAllFn = [](ImGuiContext*, ImGuiSettingsHandler* handler,
							 ImGuiTextBuffer* buf) {
		FuncDoodle::Application* application =
			static_cast<FuncDoodle::Application*>(handler->UserData);
		if (!application) {
			FUNC_INF("???");
			return;
		}
		FuncDoodle::UUID theme = application->Theme();
		buf->reserve(buf->size() + strlen(theme.ToString()));
		buf->append("[UserData][Preferences]\n");
		buf->appendf("Theme=\"%s\"", theme.ToString());
		buf->append("\n");
		buf->appendf("Sfx=%d", application->SFXEnabled() ? 1 : 0);
		buf->append("\n");
		buf->appendf("Prev=%d", application->PrevEnabled() ? 1 : 0);
		buf->append("\n");
		buf->appendf("UndoByStroke=%d", application->UndoByStroke() ? 1 : 0);
		buf->append("\n");
		buf->appendf("FrameLimit=%lf", application->FrameLimit());
		buf->append("\n");
	};
	ImGui::AddSettingsHandler(&handler);

	ImGui::LoadIniSettingsFromDisk(ImGui::GetIO().IniFilename);

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(win, true);
	ImGui_ImplOpenGL3_Init("#version 140");

	PaError err = Pa_Initialize();
	if (err != paNoError) {
		FUNC_WARN("Failed to initialize port audio: " +
				  (std::string)Pa_GetErrorText(err));
		exit(-1);
	}

	FuncDoodle::GlobalLoadAssets(&assetLoader);

	auto lastFrameTime = std::chrono::high_resolution_clock::now();

	GLFWimage* icon = GlobalLoadWinImage(assetsPath);

	glfwSetWindowUserPointer(win, application.get());
	// idk why it doesn't work if you dont set it twice
	io.UserData = application.get();
	if (icon != nullptr) {
		glfwSetWindowIcon(win, 1, icon);
	}
	glfwSetDropCallback(
		win, [](GLFWwindow* win, int count, const char** paths) {
			((FuncDoodle::Application*)(glfwGetWindowUserPointer(win)))
				->DropCallback(win, count, paths);
		});

	if (icon != nullptr) {
		stbi_image_free(icon->pixels);
		free(icon);
	}

	while (!glfwWindowShouldClose(win)) {
		GlobalAppTick(win, lastFrameTime, application.get(), io);
		if (application->ShouldClose()) {
			break;
		}
		if (glfwWindowShouldClose(win)) {
			if (application->CurProj() && !application->CurProj()->Saved()) {
				glfwSetWindowShouldClose(win, false);
				application->OpenSaveChangesDialog();
			}
		}
	}
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

	glfwDestroyWindow(win);
	glfwTerminate();

	return 0;
}
