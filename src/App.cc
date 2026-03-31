#include "Gui.h"
#include "KeyHandler.h"
#include "Manager.h"

#include "Action.h"

#include "App.h"

#include <array>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <imgui.h>
#include <iostream>
#include <stdint.h>
#include <string.h>
#include <string>

#include "LoadedAssets.h"
#include "TextUtil.h"

#include "MacroUtils.h"

#include "ImUtil.h"
#include "Themes.h"
#include "Tool.h"

#include "imgui_te_ui.h"
#include "nfd.h"

#include "Keybinds.h"

#include "Test.h"

namespace FuncDoodle {
	Application::Application(GLFWwindow* win, AssetLoader* assetLoader,
		std::filesystem::path themesPath, std::filesystem::path rootPath)
		: m_FilePath(""), m_CurrentProj(nullptr), m_CacheProj(nullptr),
		  m_EditorController(std::make_shared<EditorController>()),
		  m_Window(win), m_AssetLoader(assetLoader),
		  m_CacheBGCol({255, 255, 255}), m_ThemesPath(themesPath),
		  m_Theme(UUID::FromString("d0c1a009-d09c-4fe6-84f8-eddcb2da38f9")),
		  m_Keybinds(rootPath) {
		m_Manager = std::make_unique<AnimationManager>(
			nullptr, assetLoader, m_EditorController, m_Keybinds, m_PrevEnabled),
		m_Manager->SetUndoByStroke(m_UndoByStroke);

		RegisterKeybinds();
	}

	void Application::RegisterKeybinds() {
		// nose
		m_Keybinds.Register("new", {true, false, false, ImGuiKey_N});
		m_Keybinds.Register("open", {true, false, false, ImGuiKey_O});
		m_Keybinds.Register("save", {true, false, false, ImGuiKey_S});
		m_Keybinds.Register("export", {true, false, false, ImGuiKey_E});

		m_Keybinds.Register("quit", {true, false, false, ImGuiKey_Q});
		m_Keybinds.Register("pref", {true, false, false, ImGuiKey_Comma});
		m_Keybinds.Register("theme", {true, false, false, ImGuiKey_T});

		m_Keybinds.Register("keybinds", {true, false, false, ImGuiKey_H});

		m_Keybinds.Register("del", {false, false, false, ImGuiKey_Delete});

		ToolKeybindsRegister(m_Keybinds);
		KeyHandler::RegisterKeybinds(m_Keybinds);
		m_Manager->RegisterKeybinds();

		m_Keybinds.End();
	}

	void Application::RegisterPopups() {
		m_Popups.Register("edit_proj");
		m_Popups.Register("export");
		m_Popups.Register("keybinds");
		m_Popups.Register("new");
		m_Popups.Register("pref");
		m_Popups.Register("rotate");
		m_Popups.Register("save_changes");
	}

	Application::~Application() {
		for (char* log : s_Logs) {
			delete[] log;
		}
		s_Logs.clear();
	}

	void Application::UpdateFPS(double deltaTime) {
		constexpr double minDelta = 1.0 / 1000.0;
		constexpr double maxDelta = 1.0;
		if (deltaTime >= minDelta && deltaTime <= maxDelta) {
			m_FPS = 1.0 / deltaTime;
		} else if (deltaTime > maxDelta) {
			m_FPS = 1.0 / maxDelta;
		}
		m_LastFrameTime = deltaTime;
	}

	char* GlobalGetShortcut(const char* key, bool shift, bool super) {
		int maxLen = 11 + strlen(key);

		char* shortcut = (char*)malloc(maxLen);

#if defined(_WIN32) || defined(__linux__)
		strcpy(shortcut, "Ctrl");
#else
		strcpy(shortcut, "Cmd");
#endif

		if (shift) {
			strcat(shortcut, "+Shift");
		}

		if (super) {
			strcat(shortcut, "+Super");
		}

		strcat(shortcut, "+");
		strcat(shortcut, key);

		return shortcut;
	}

	void Application::CheckKeybinds() {
		if (m_WaitingForKey != nullptr)
			return;

		ImGuiIO& io = ImGui::GetIO();

		// Check if each shortcut is pressed and perform the appropriate action
		if (m_Keybinds.Get("new").IsPressed()) {
			if (m_SFXEnabled)
				m_AssetLoader->PlaySound(s_ProjCreateSound);
			m_Popups.Open("new");
		}
		if (m_Keybinds.Get("open").IsPressed()) {
			OpenFileDialog([&]() { this->ReadProjectFile(); });
		}
		if (m_Keybinds.Get("quit").IsPressed()) {
			glfwSetWindowShouldClose(m_Window, true);
		}
		if (m_Keybinds.Get("pref").IsPressed()) {
			m_Popups.Open("pref");
		}
		if (m_Keybinds.Get("theme").IsPressed()) {
			Themes::g_ThemeEditorOpen = true;
		}
		if (m_Keybinds.Get("del").IsPressed()) {
			if (m_EditorController->Sel() && m_CurrentProj) {
				auto selPixels = m_EditorController->Sel()->All();
				std::vector<Col> prevPixels;
				prevPixels.reserve(selPixels.size());
				for (auto& p : selPixels) {
					prevPixels.push_back(
						m_Manager->SelectedFrame()->Pixels()->Get(p.x, p.y));
				}
				m_CurrentProj->PushUndoable(
					DeleteSelectionAction(m_Manager->SelectedFrameI(),
						m_EditorController->Sel(), prevPixels, m_CurrentProj));
				m_Manager->SelectedFrame()->DeleteSelection(
					m_EditorController->Sel().get(), m_CurrentProj->BgCol());
			}
		}
		if (m_Keybinds.Get("keybinds").IsPressed()) {
			m_Popups.Open("keybinds");
		}
		if (m_CurrentProj) {
			if (m_CurrentProj && m_Keybinds.Get("save").IsPressed()) {
				if (m_SFXEnabled)
					m_AssetLoader->PlaySound(s_ProjSaveSound);
				SaveFileDialog([&]() { this->SaveProjectFile(); });
				if (m_SFXEnabled)
					m_AssetLoader->PlaySound(s_ProjSaveEndSound);
			}

			if (m_Keybinds.Get("export").IsPressed()) {
				m_Popups.Open("export");
			}
		}
	}

	void Application::RenderImGui() {
		#ifdef FUNCDOODLE_BUILD_IMTESTS
			ImGuiTestEngine_ShowTestEngineWindows(s_TestEngine, &m_ShowTests);
		#endif

		if (!m_CurrentProj)
			RenderOptions();

		CheckKeybinds();
		RenderMainMenuBar();
		RenderEditPrefs();
		RenderRotate();
		SaveChangesDialog();
		RenderExport();
		RenderEditProj();
		RenderKeybinds();
		RenderNewProj();

		Themes::ThemeEditor();
		Themes::SaveCurrentTheme();

		if (m_CurrentProj) {
			m_Manager->SetUndoByStroke(m_UndoByStroke);
			m_Manager->RenderTimeline(m_PrevEnabled);
			m_Manager->RenderControls();
			m_Manager->RenderLogs();
			m_Manager->Player()->Play();
			m_CurrentProj->DisplayFPS(m_FPS);
		} else {
			char* title = (char*)malloc(35);
			if (title != 0) {
				snprintf(
					title, 35, "FuncDoodle %s, %d FPS", FUNCVER, (int)m_FPS);
				glfwSetWindowTitle(m_Window, title);
				free(title);
			} else {
				FUNC_WARN(
					"Failed to allocate title -- perhaps you ran out of RAM?");
			}
		}
	}
	void Application::OpenFileDialog(std::function<void()> done) {
		nfdchar_t* outPath = 0;
		nfdresult_t result = NFD_OpenDialog("fdp", 0, &outPath);

		if (result == NFD_OKAY) {
			m_FilePath = outPath;
			done();
			free(outPath);
		} else if (result == NFD_CANCEL) {
			FUNC_DBG("Cancelled");
		} else {
			FUNC_WARN("Failed to open open file dialog -- " +
					  (std::string)NFD_GetError());
		}
		// free(outPath);
	}
	void Application::SaveFileDialog(std::function<void()> done) {
		if (m_CurrentProj == nullptr) {
			FUNC_INF("No project to save");
			return;
		}
		nfdchar_t* outPath = 0;
		nfdresult_t result = NFD_SaveDialog("fdp", 0, &outPath);

		if (result == NFD_OKAY) {
			m_FilePath = outPath;
			done();
			free(outPath);
		} else if (result == NFD_CANCEL) {
			FUNC_DBG("Cancelled");
		} else {
			FUNC_WARN("Failed to open save file dialog -- " +
					  (std::string)NFD_GetError());
		}
	}
	void Application::ReadProjectFile() {
		// m_FilePath is the actual file that we're going to read
		if (m_FilePath.empty()) {
			FUNC_DBG("@Application::ReadProjectFile -- m_FilePath is nullptr");
			return;
		}

		if (m_CurrentProj == nullptr) {
			m_CurrentProj.reset(new ProjectFile((char*)"", 1, 1, (char*)"", 0,
				(char*)"", m_Window, Col{.r = 0, .g = 0, .b = 0}));
		}

		m_CurrentProj->ReadAndPopulate(m_FilePath.c_str());

		m_Manager->SetProj(m_CurrentProj);
	}
	void Application::SaveProjectFile() {
		if (m_FilePath.empty()) {
			FUNC_DBG("@Application::SaveProjectFile -- m_FilePath is nullptr");
			return;
		}
		m_CurrentProj->Write(m_FilePath.c_str());
	}

	void Application::RenderOptions() {
		ImGuiViewport* vp = ImGui::GetMainViewport();
		ImVec2 size = vp->Size;
		ImVec2 safe = ImGui::GetStyle().DisplaySafeAreaPadding;

		float menuBarHeight = ImGui::GetFrameHeight();
		ImGui::SetNextWindowPos(ImVec2(safe.x - 2, menuBarHeight + safe.y - 3));
		size.y -= menuBarHeight;
		ImGui::SetNextWindowSize(size, ImGuiCond_Always);
		ImGui::SetNextWindowViewport(vp->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		ImGui::Begin("Options", 0, ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoDecoration);

		ImGui::BeginGroup();

		const ImVec2 btnSize(50, 50);
		const float titleFontSize = 25.0f;
		const float descFontSize = 18.0f;
		const float descYOffset = 12.0f;
		const float openTextYOffset = 4.0f;
		const float rowGapExtra = 14.0f;

		ImFont* titleFont = m_AssetLoader && m_AssetLoader->GetFontBold()
			? m_AssetLoader->GetFontBold()
			: ImGui::GetFont();

		const float spacingY = ImGui::GetStyle().ItemSpacing.y;
		const float textGap = ImGui::GetStyle().ItemSpacing.x;
		const float rowGap = spacingY + rowGapExtra;

		const char* newProjTitle = "New project";
		const char* newProjDesc = "Create a new FuncDoodle project";
		const char* openProjTitle = "Open project";
		const char* openProjDesc = "Open an existing FuncDoodle project";

		float maxWidth = TextUtil::MaxWidth(titleFont, titleFontSize,
			newProjTitle, openProjTitle, nullptr, descFontSize, newProjDesc,
			openProjDesc);

		float groupWidth = btnSize.x;
		float groupHeight = btnSize.y * 2 + rowGap;

		float windowWidth = ImGui::GetWindowSize().x;
		ImGui::SetCursorPosX((windowWidth - groupWidth) * 0.5f - maxWidth);
		float rowStartX = ImGui::GetCursorPosX();

		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 avail = ImGui::GetContentRegionAvail();
		float rowTopY = (avail.y - groupHeight) * 0.5f;
		ImGui::SetCursorPosY(rowTopY);

		auto renderOptionRow = [&](const char* title, const char* desc,
								   uint32_t texId, float textYOffset,
								   const std::function<void()>& onClick) {
			if (ImGui::ImageButton(title, (ImTextureID)(intptr_t)texId, btnSize)) {
				onClick();
			}

			ImVec2 btnMin = ImGui::GetItemRectMin();
			ImVec2 btnMax = ImGui::GetItemRectMax();
			float btnTopY = btnMin.y - windowPos.y;
			float btnBottomY = btnMax.y - windowPos.y;
			float textX = btnMax.x - windowPos.x + textGap;

			ImGui::SameLine();

			ImGui::PushFont(titleFont, titleFontSize);
			float titleHeight =
				TextUtil::TextHeight(titleFont, titleFontSize, title);
			ImGui::SetCursorPosX(textX);
			ImGui::SetCursorPosY(btnTopY + (btnSize.y - titleHeight) * 0.5f -
				textYOffset);
			ImGui::Text("%s", title);
			ImGui::PopFont();

			ImGui::PushFont(nullptr, descFontSize);
			float descLineHeight = ImGui::GetTextLineHeight();
			ImGui::SetCursorPosX(textX);
			ImGui::SetCursorPosY(btnBottomY - descLineHeight - descYOffset -
				textYOffset);
			ImGui::Text("%s", desc);
			ImGui::PopFont();
		};

		renderOptionRow(newProjTitle, newProjDesc, s_AddTexId, 0.0f, [&]() {
			if (m_SFXEnabled)
				m_AssetLoader->PlaySound(s_ProjCreateSound);
			m_Popups.Open("new");
		});

		ImGui::SetCursorPosX(rowStartX);
		ImGui::SetCursorPosY(rowTopY + btnSize.y + rowGap);

		renderOptionRow(openProjTitle, openProjDesc, s_OpenTexId, openTextYOffset,
			[&]() { OpenFileDialog([&]() { this->ReadProjectFile(); }); });

		ImGui::EndGroup();
		ImGui::End();
		ImGui::PopStyleVar(2);
	}


	void Application::SaveChangesDialog() {
		if (m_Popups.IsOpen("save_changes")) {
			ImGui::OpenPopup("Save Changes");
		}

		if (ImGui::BeginPopupModal(
				"Save Changes", m_Popups.Get("save_changes"))) {
			ImGui::Text("Save changes?");
			if (ImGui::Button("Yes")) {
				SaveFileDialog([&] { SaveProjectFile(); });
				// glfwSetWindowShouldClose(m_Window, true);
				m_ShouldClose = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("No")) {
				// glfwSetWindowShouldClose(m_Window, true);
				m_ShouldClose = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				m_ShouldClose = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
	void Application::OpenSaveChangesDialog() {
		FUNC_DBG("Saved?: " << m_CurrentProj->Saved());
		FUNC_DBG("m_Popups.Open(save_changes) being called");
		m_Popups.Open("save_changes");
		FUNC_DBG("called m_Popups.Open(save_changes)");
	}
	void Application::DropCallback(
		GLFWwindow* win, int count, const char** paths) {
		if (count == 0)
			return;
		if (count > 1) {
			FUNC_WARN("Attempted to drag and drop multiple items when 1 is "
					  "expected: Attempting to use first item");
		}
		m_FilePath = paths[0] ? paths[0] : "";
		this->ReadProjectFile();
	}
	void Application::RenderEditProj() {
		if (m_Popups.IsOpen("edit_proj")) {
			ImGui::OpenPopup("EditProj");
		}

		if (ImGui::IsPopupOpen("EditProj")) {
			ImGui::SetNextWindowFocus();
			ImGui::SetNextWindowPos(ImVec2(485, 384), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(309, 312), ImGuiCond_FirstUseEver);
		}
		if (ImGui::BeginPopupModal("EditProj", m_Popups.Get("edit_proj"),
				ImGuiWindowFlags_AlwaysAutoResize) &&
			m_CurrentProj) {
			char name[256];
			strcpy(name, m_CurrentProj->AnimName());
			int width = m_CurrentProj->AnimWidth();
			int height = m_CurrentProj->AnimHeight();
			char author[100];
			strcpy(author, m_CurrentProj->AnimAuthor());
			int fps = m_CurrentProj->AnimFPS();
			char desc[512];
			strcpy(desc, m_CurrentProj->AnimDesc());
			if (m_CacheProj) {
				strcpy(name, m_CacheProj->AnimName());
				width = m_CacheProj->AnimWidth();
				height = m_CacheProj->AnimHeight();
				strcpy(author, m_CacheProj->AnimAuthor());
				fps = m_CacheProj->AnimFPS();
				strcpy(desc, m_CacheProj->AnimDesc());
			} else {
				strcpy(name, (char*)"Untitled Animation");
				width = 32;
				height = 32;
				const char* username =
					std::getenv("USER");  // Common on Linux and macOS
				if (!username) {
					username =
						std::getenv("LOGNAME");	 // Fallback for Linux and macOS
				}
				if (!username) {
					username = std::getenv("USERNAME");	 // Common on Windows
				}
				if (!username) {
					username = "unknown";
				}
				strncpy(author, username, sizeof(author) - 1);
				author[sizeof(author) - 1] = '\0';
				fps = 10;
				strcpy(desc, "Simple test project");
			}
			if (ImGui::InputText("Name", name, sizeof(name))) {
				m_CacheProj->SetAnimName(name);
			}
			if (ImGui::InputInt("Width", &width)) {
				m_CacheProj->SetAnimWidth(width);
			}
			if (ImGui::InputInt("Height", &height)) {
				m_CacheProj->SetAnimHeight(height);
			}
			if (ImGui::InputText("Author", author, sizeof(author))) {
				m_CacheProj->SetAnimAuthor(author);
			}
			if (ImGui::InputInt("FPS", &fps)) {
				m_CacheProj->SetAnimFPS(fps);
			}
			if (ImGui::InputText("Description", desc, sizeof(desc))) {
				m_CacheProj->SetAnimDesc(desc);
			}

			if (ImGui::Button("Close") ||
				ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
				m_Popups.Close("edit_proj");
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("OK") ||
				ImGui::IsKeyPressed(ImGuiKey_Enter, false) ||
				ImGui::IsKeyPressed(ImGuiKey_KeypadEnter, false)) {
				m_CurrentProj = m_CacheProj;
				m_Popups.Close("edit_proj");
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
	void Application::RenderNewProj() {
		if (m_Popups.IsOpen("new")) {
			ImGui::OpenPopup("NewProj");
		}

		if (ImGui::IsPopupOpen("NewProj")) {
			ImGui::SetNextWindowFocus();
			ImGui::SetNextWindowPos(ImVec2(376, 436), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(350, 336), ImGuiCond_FirstUseEver);
		}

		if (ImGui::BeginPopupModal("NewProj", m_Popups.Get("new"),
				ImGuiWindowFlags_AlwaysAutoResize)) {
			bool justOpened = ImGui::IsWindowAppearing();
			char name[256] = "";
			int width = 32;
			int height = 32;
			char author[100] = "";
			int fps = 0;
			char desc[512] = "";

			if (!m_CacheProj) {
				strcpy(name, (char*)"testproj");
				width = 32;
				height = 32;
				const char* username = std::getenv("USER");
				if (!username) {
					username = std::getenv("LOGNAME");
				}
				if (!username) {
					username = std::getenv("USERNAME");
				}
				if (!username) {
					username = "unknown";
				}
				strncpy(author, username, sizeof(author) - 1);
				author[sizeof(author) - 1] = '\0';
				fps = 10;
				strcpy(desc, "Simple test project");
				m_CacheProj.reset(new ProjectFile(
					name, width, height, author, fps, desc, m_Window, Col()));
			} else {
				strcpy(name, m_CacheProj->AnimName());
				width = m_CacheProj->AnimWidth();
				height = m_CacheProj->AnimHeight();
				strcpy(author, m_CacheProj->AnimAuthor());
				fps = m_CacheProj->AnimFPS();
				strcpy(desc, m_CacheProj->AnimDesc());

				float r = (float)(m_CacheProj->BgCol().r) / 255;
				float g = (float)(m_CacheProj->BgCol().g) / 255;
				float b = (float)(m_CacheProj->BgCol().b) / 255;
				m_CacheBGCol[0] = r;
				m_CacheBGCol[1] = g;
				m_CacheBGCol[2] = b;
			}

			// GUI inputs for project properties
			if (ImGui::InputText("Name", name, sizeof(name))) {
				m_CacheProj->SetAnimName(name);
			}
			if (ImGui::InputInt("Width", &width)) {
				if (m_CurrentProj)
					m_CacheProj->SetAnimWidth(width, false);
				else {
					if (m_CacheProj) {
						m_CacheProj->SetAnimWidth(width, true);
					}
				}
			}
			if (ImGui::InputInt("Height", &height)) {
				if (m_CurrentProj)
					m_CacheProj->SetAnimHeight(height, false);
				else
					m_CacheProj->SetAnimHeight(height, true);
			}
			if (ImGui::InputText("Author", author, sizeof(author))) {
				m_CacheProj->SetAnimAuthor(author);
			}
			if (ImGui::InputInt("FPS", &fps)) {
				m_CacheProj->SetAnimFPS(fps);
			}
			if (ImGui::InputText("Description", desc, sizeof(desc))) {
				m_CacheProj->SetAnimDesc(desc);
			}
			if (ImGui::ColorPicker3("BG", m_CacheBGCol.data())) {
				if (m_CacheProj)
					m_CacheProj->SetBgCol(m_CacheBGCol.data());
			}

			if (ImGui::Button("Close") ||
				ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
				m_Popups.Close("new");
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			bool acceptByKey = !justOpened &&
				(ImGui::IsKeyPressed(ImGuiKey_Enter, false) ||
					ImGui::IsKeyPressed(ImGuiKey_KeypadEnter, false));
			if (ImGui::Button("OK") || acceptByKey) {
				m_CurrentProj = m_CacheProj;
				m_Manager = std::make_unique<AnimationManager>(m_CurrentProj,
					m_AssetLoader, m_EditorController, m_Keybinds, m_PrevEnabled);
				m_Manager->SetUndoByStroke(m_UndoByStroke);
				m_Popups.Close("new");
			}

			ImGui::EndPopup();
		}
	}
	void Application::Rotate(int32_t deg) {
		if (!m_EditorController->Sel()) {
			m_CurrentProj->PushUndoable(RotateFrameAction(
				m_Manager->SelectedFrameI(), deg, m_CurrentProj));
			m_Manager->SelectedFrame()->Rotate(deg);
		} else {
			m_CurrentProj->PushUndoable(
				RotateSelectionAction(m_Manager->SelectedFrameI(),
					m_EditorController->Sel(), deg, m_CurrentProj));

			m_Manager->SelectedFrame()->RotateSelection(
				m_EditorController->Sel().get(), deg);
		}
	}

	void Application::RenderMainMenuBar() {
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File", true)) {
				if (ImGui::MenuItem("New project",
						m_WaitingForKey ? nullptr : m_Keybinds.Get("new"))) {
					if (m_SFXEnabled)
						m_AssetLoader->PlaySound(s_ProjCreateSound);
					m_Popups.Open("new");
				}

				if (ImGui::MenuItem("Open",
						m_WaitingForKey ? nullptr : m_Keybinds.Get("open"))) {
					this->OpenFileDialog([&]() { this->ReadProjectFile(); });
				}
				if (m_CurrentProj) {
					if (ImGui::MenuItem("Save", m_WaitingForKey
													? nullptr
													: m_Keybinds.Get("save"))) {
						if (m_SFXEnabled)
							m_AssetLoader->PlaySound(s_ProjSaveSound);
						this->SaveFileDialog(
							[&]() { this->SaveProjectFile(); });
					}

					if (ImGui::MenuItem("Close")) {
						m_CurrentProj = nullptr;
						m_Manager->SetProj(nullptr);
					}
					if (ImGui::MenuItem("Edit project")) {
						m_Popups.Open("edit_proj");
					}
					if (ImGui::MenuItem("Export",
							m_WaitingForKey ? nullptr
											: m_Keybinds.Get("export"))) {
						m_Popups.Open("export");
					}
				}
				if (ImGui::MenuItem("Exit",
						m_WaitingForKey ? nullptr : m_Keybinds.Get("quit"))) {
					m_Popups.CloseAllExcept("save_changes");
					glfwSetWindowShouldClose(m_Window, true);
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit", true)) {
				if (m_CurrentProj) {
					if (ImGui::BeginMenu("Transform")) {
						if (ImGui::MenuItem("Rotate 90°")) {
							Rotate(90);
						}

						if (ImGui::MenuItem("Rotate -90°")) {
							Rotate(-90);
						}

						if (ImGui::MenuItem("Rotate...")) {
							m_Popups.Open("rotate");
						}

						ImGui::EndMenu();
					}

					if (ImGui::MenuItem("Delete",
							m_WaitingForKey ? nullptr
											: m_Keybinds.Get("del"))) {
						if (m_EditorController->Sel()) {
							auto selPixels = m_EditorController->Sel()->All();
							std::vector<Col> prevPixels;
							prevPixels.reserve(selPixels.size());
							for (auto& p : selPixels) {
								prevPixels.push_back(
									m_Manager->SelectedFrame()->Pixels()->Get(
										p.x, p.y));
							}
							m_CurrentProj->PushUndoable(DeleteSelectionAction(
								m_Manager->SelectedFrameI(),
								m_EditorController->Sel(), prevPixels,
								m_CurrentProj));
							m_Manager->SelectedFrame()->DeleteSelection(
								m_EditorController->Sel().get(),
								m_CurrentProj->BgCol());
						}
					}
				}

				if (ImGui::MenuItem("Preferences",
						m_WaitingForKey ? nullptr : m_Keybinds.Get("pref"))) {
					m_Popups.Open("pref");
				}
				if (ImGui::MenuItem("Theme editor",
						m_WaitingForKey ? nullptr : m_Keybinds.Get("theme"))) {
					Themes::g_ThemeEditorOpen = true;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help", true)) {
				if (ImGui::MenuItem("Show keybinds",
						m_WaitingForKey ? nullptr
										: m_Keybinds.Get("keybinds"))) {
					m_Popups.Open("keybinds");
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}
	void Application::RenderEditPrefs() {
		if (m_Popups.IsOpen("pref")) {
			ImGui::OpenPopup("EditPrefs");
			m_Popups.Close("pref");
		}
		if (ImGui::BeginPopup("EditPrefs")) {
			if (ImGui::BeginCombo("Theme", Themes::g_Themes[m_Theme].Name)) {
				for (auto& [uuid, theme] : Themes::g_Themes) {
					bool is_selected = (m_Theme == uuid);
					if (ImGui::Selectable(theme.Name, is_selected)) {
						m_Theme = uuid;
						ImGui::GetStyle() = theme.Style;
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
						if (ImGui::GetIO().ConfigFlags &
							ImGuiConfigFlags_ViewportsEnable) {
							ImGui::GetStyle().WindowRounding = 1.0f;
						}
					}
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text("by %s", theme.Author);
						ImGui::EndTooltip();
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			if (ImGui::Button("Save current")) {
				Themes::g_SaveThemeOpen = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("Add temporary from file")) {
				static Themes::CustomTheme* style;
				nfdpathset_t pathset;
				nfdresult_t res = NFD_OpenDialogMultiple("toml", "", &pathset);
				if (res == NFD_OKAY) {
					for (size_t i = 0; i < NFD_PathSet_GetCount(&pathset);
						i++) {
						nfdchar_t* path = NFD_PathSet_GetPath(&pathset, i);
						style = Themes::LoadThemeFromFile(path);
						if (style) {
							auto [it, inserted] =
								Themes::g_Themes.emplace(style->Uuid, *style);
							if (!inserted && style->OwnsMeta) {
								std::free(const_cast<char*>(style->Name));
								std::free(const_cast<char*>(style->Author));
								style->Name = "";
								style->Author = "";
								style->OwnsMeta = false;
							}
						}
					}
					NFD_PathSet_Free(&pathset);
				} else if (res == NFD_ERROR) {
					FUNC_ERR(
						"Failed to open save theme dialog: " << NFD_GetError());
				}
			}
			if (ImGui::Button("Open themes directory")) {
				OPEN_FILE_EXPLORER(m_ThemesPath);
			}
			ImGui::SameLine();
			if (ImGui::Button("Refresh")) {
				Themes::ClearThemes();
				Themes::LoadThemes(m_ThemesPath);
			}

			ImGui::Checkbox("SFX", &m_SFXEnabled);
			ImGui::SameLine();
			ImGui::Checkbox("Preview", &m_PrevEnabled);
			ImGui::SameLine();
			ImGui::Checkbox("Undo by stroke", &m_UndoByStroke);

			ImGui::InputDouble("FPS limit", &m_FrameLimit);

			if (ImGui::IsKeyPressed(ImGuiKey_Escape) ||
				ImGui::IsKeyPressed(ImGuiKey_Enter) ||
				ImGui::IsKeyPressed(ImGuiKey_KeypadEnter) ||
				ImGui::Button("OK")) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
	void Application::RenderRotate() {
		if (m_Popups.IsOpen("rotate")) {
			ImGui::OpenPopup("Rotate");
			m_Popups.Close("rotate");
		}

		if (ImGui::BeginPopup("Rotate")) {
			ImGui::DragInt("##Deg", &m_Deg, 1.0f, -360, 360, "%d°");

			if (ImGui::Button("OK")) {
				Rotate(m_Deg);
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::Button("Cancel")) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
	void Application::RenderExport() {
		if (m_Popups.IsOpen("export")) {
			ImGui::OpenPopup("Export##export");
			m_Popups.Close("export");
		}

		if (ImGui::BeginPopup("Export##export")) {
			const char* formats[] = {"PNGs", "MP4"};
			ImGui::Combo("Export Format", &m_ExportFormat, formats,
				IM_ARRAYSIZE(formats));
			if (ImGui::Button("Export") ||
				ImGui::IsKeyPressed(ImGuiKey_Enter, false) ||
				ImGui::IsKeyPressed(ImGuiKey_KeypadEnter, false)) {
				nfdchar_t* outPath = 0;
				nfdresult_t result = NFD_PickFolder(0, &outPath);

				if (result == NFD_OKAY) {
					if (m_SFXEnabled)
						m_AssetLoader->PlaySound(s_ExportSound);
					FUNC_INF("Exporting to " << outPath);
					m_CurrentProj->Export(outPath, m_ExportFormat);
					free(outPath);
				} else if (result == NFD_CANCEL) {
					FUNC_DBG("Cancelled");
					free(outPath);
				} else {
					FUNC_DBG("Failed to open file dialog" +
							 (std::string)NFD_GetError());
					free(outPath);
				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close") ||
				ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
	void Application::RenderKeybinds() {
		if (m_Popups.IsOpen("keybinds")) {
			ImGui::OpenPopup("Keybinds");
		}

		if (ImGui::BeginPopupModal("Keybinds", m_Popups.Get("keybinds"),
				ImGuiWindowFlags_AlwaysAutoResize)) {
			if (ImGui::BeginTable(
					"keybinds", 3, ImGuiTableFlags_BordersInnerH)) {
				ImGui::TableSetupColumn("Action");
				ImGui::TableSetupColumn("Keybind");
				ImGui::TableSetupColumn("Reset");
				ImGui::TableHeadersRow();

				for (auto& [k, v] : m_Keybinds.GetAll()) {
					ImGui::TableNextRow();
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg,
						ImGui::GetColorU32(ImGuiCol_FrameBg));

					ImGui::TableSetColumnIndex(0);
					ImGui::TextUnformatted(k);

					ImGui::TableSetColumnIndex(1);

					ImGui::PushID(k);
					const char* label = "...";
					if (!m_WaitingForKey || strcmp(m_WaitingForKey, k) != 0) {
						label = v.User.value_or(v.Default);
					}
					if (ImGui::Button(label)) {
						m_WaitingForKey = k;
					}

					ImGuiKey key = ImUtil::GetAnyReleasedKey();

					if (m_WaitingForKey != nullptr &&
						strcmp(m_WaitingForKey, k) == 0 &&
						key != ImGuiKey_None) {
						ImGuiIO& io = ImGui::GetIO();
						v.User =
							Shortcut(io.KeyCtrl, io.KeyShift, io.KeySuper, key);
						m_WaitingForKey = nullptr;
						io.KeysData[key].Down = false;
						io.KeyCtrl = false;
						io.KeyShift = false;
						io.KeySuper = false;
						m_Keybinds.Write();
					}

					ImGui::PopID();

					ImGui::TableSetColumnIndex(2);
					if (!v.User.has_value() || v.User.value() == v.Default) {
						ImGui::BeginDisabled(true);
					} else {
						ImGui::BeginDisabled(false);
					}

					ImGui::PushID(k);
					if (ImGui::Button("Reset")) {
						v.User = std::nullopt;
					}
					ImGui::PopID();

					ImGui::EndDisabled();
				}
				ImGui::EndTable();
			}

			ImGui::EndPopup();
		} else {
			if (m_WaitingForKey != nullptr) {
				m_WaitingForKey = nullptr;
			}
		}
	}
}  // namespace FuncDoodle
