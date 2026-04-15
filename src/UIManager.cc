#include "UIManager.h"
#include "App.h"
#include "Constants.h"
#include "ImUtil.h"
#include "LoadedAssets.h"
#include "Themes.h"

namespace FuncDoodle {
	UIManager::UIManager() {}

	void UIManager::SaveChanges() {
		Application* app = Application::Get();

		if (app->GetPopups()->IsOpen("save_changes")) {
			ImGui::OpenPopup("Save Changes");
		}

		if (ImGui::BeginPopupModal(
				"Save Changes", app->GetPopups()->Get("save_changes"))) {
			ImGui::Text("Save changes?");
			ImUtil::ButtonRowResult choice = ImUtil::YesNoCancelButtons();
			if (choice == ImUtil::ButtonRowResult::Primary) {
				app->Save(true);
			} else if (choice == ImUtil::ButtonRowResult::Secondary) {
				app->SetShouldClose(true);
				ImGui::CloseCurrentPopup();
			} else if (choice == ImUtil::ButtonRowResult::Tertiary) {
				app->SetShouldClose(false);
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	void UIManager::MainMenuBar() {
		Application* app = Application::Get();

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File", true)) {
				if (ImGui::MenuItem("New",
						m_WaitingForKey ? nullptr
										: app->GetKeybinds().Get("new"))) {
					if (app->GetSettings().Sfx)
						app->GetAssetLoader()->PlaySound(s_ProjCreateSound);
					app->GetPopups()->Open("new");
				}

				if (ImGui::MenuItem("Open",
						m_WaitingForKey ? nullptr
										: app->GetKeybinds().Get("open"))) {
#ifndef MACOS
					std::thread([&]() {
#endif
						app->OpenFileDialog([&]() { app->ReadProjectFile(); });
#ifndef MACOS
					}).detach();
#endif
				}
				if (app->GetCurProj()) {
					const char* path = app->GetCurProj()->LastSavePath();

					if (ImGui::MenuItem("Save",
							m_WaitingForKey ? nullptr
											: app->GetKeybinds().Get("save"))) {
						if (*path) {
							app->SaveAt(path);
						} else {
							app->Save();
						}
					}

					if (*path) {
						if (ImGui::MenuItem("Save as...",
								m_WaitingForKey
									? nullptr
									: app->GetKeybinds().Get("save_as"))) {
							// force dialog if user presses save as
							app->Save();
						}
					}

					if (ImGui::MenuItem("Close")) {
						app->SetCurProj(nullptr);
						app->GetManager()->SetProj(nullptr);
					}
					if (ImGui::MenuItem("Edit project")) {
						app->GetPopups()->Open("edit_proj");
					}
					if (ImGui::MenuItem(
							"Export", m_WaitingForKey
										  ? nullptr
										  : app->GetKeybinds().Get("export"))) {
						app->GetPopups()->Open("export");
					}
				}
				if (ImGui::MenuItem("Exit",
						m_WaitingForKey ? nullptr
										: app->GetKeybinds().Get("quit"))) {
					app->GetPopups()->CloseAllExcept("save_changes");
					app->GetWindow()->SetShouldClose(true);
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit", true)) {
				if (app->GetCurProj()) {
					if (ImGui::BeginMenu("Transform")) {
						if (ImGui::MenuItem("Rotate 90°")) {
							app->Rotate(90);
						}

						if (ImGui::MenuItem("Rotate -90°")) {
							app->Rotate(-90);
						}

						if (ImGui::MenuItem("Rotate...")) {
							app->GetPopups()->Open("rotate");
						}

						ImGui::EndMenu();
					}

					if (ImGui::MenuItem("Delete",
							m_WaitingForKey ? nullptr
											: app->GetKeybinds().Get("del"))) {
						if (app->GetController()->Sel()) {
							app->DeleteCurrentSelection();
						}
					}

					if (ImGui::MenuItem("Move left",
							m_WaitingForKey ? nullptr
											: app->GetKeybinds().Get(
												  "move_selection_left"))) {
						if (app->GetController()->Sel()) {
							app->MoveCurrentSelection(Direction::Left);
						}
					}

					if (ImGui::MenuItem("Move right",
							m_WaitingForKey ? nullptr
											: app->GetKeybinds().Get(
												  "move_selection_right"))) {
						if (app->GetController()->Sel()) {
							app->MoveCurrentSelection(Direction::Right);
						}
					}

					if (ImGui::MenuItem("Move up",
							m_WaitingForKey ? nullptr
											: app->GetKeybinds().Get(
												  "move_selection_up"))) {
						if (app->GetController()->Sel()) {
							app->MoveCurrentSelection(Direction::Up);
						}
					}

					if (ImGui::MenuItem("Move down",
							m_WaitingForKey ? nullptr
											: app->GetKeybinds().Get(
												  "move_selection_down"))) {
						if (app->GetController()->Sel()) {
							app->MoveCurrentSelection(Direction::Down);
						}
					}
				}

				if (ImGui::MenuItem("Preferences",
						m_WaitingForKey ? nullptr
										: app->GetKeybinds().Get("pref"))) {
					app->GetPopups()->Open("pref");
				}
				if (ImGui::MenuItem("Theme editor",
						m_WaitingForKey ? nullptr
										: app->GetKeybinds().Get("theme"))) {
					Themes::g_ThemeEditorOpen = true;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help", true)) {
				if (ImGui::MenuItem("Show keybinds",
						m_WaitingForKey ? nullptr
										: app->GetKeybinds().Get("keybinds"))) {
					app->GetPopups()->Open("keybinds");
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

	void UIManager::Keybinds() {
		Application* app = Application::Get();

		if (app->GetPopups()->IsOpen("keybinds")) {
			ImGui::OpenPopup("Keybinds");
		}

		if (ImGui::BeginPopupModal("Keybinds",
				app->GetPopups()->Get("keybinds"),
				ImGuiWindowFlags_AlwaysAutoResize)) {
			if (ImGui::BeginTable(
					"keybinds", 3, ImGuiTableFlags_BordersInnerH)) {
				ImGui::TableSetupColumn("Action");
				ImGui::TableSetupColumn("Keybind");
				ImGui::TableSetupColumn("Reset");
				ImGui::TableHeadersRow();

				for (auto& [k, v] : app->GetKeybinds().GetAll()) {
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
						app->GetKeybinds().Write();
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

			if (ImUtil::CloseButton() || ImUtil::EscPressed()) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		} else {
			if (m_WaitingForKey != nullptr) {
				m_WaitingForKey = nullptr;
			}
		}
	}

	void UIManager::EditProj() {
		Application* app = Application::Get();

		if (app->GetPopups()->IsOpen("edit_proj")) {
			ImGui::OpenPopup("EditProj");
		}

		if (ImGui::IsPopupOpen("EditProj")) {
			ImGui::SetNextWindowFocus();
			ImGui::SetNextWindowPos(ImVec2(485, 384), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(309, 312), ImGuiCond_FirstUseEver);
		}
		if (ImGui::BeginPopupModal("EditProj",
				app->GetPopups()->Get("edit_proj"),
				ImGuiWindowFlags_AlwaysAutoResize) &&
			app->GetCurProj()) {
			char name[256];
			strcpy(name, app->GetCurProj()->AnimName());
			int width = app->GetCurProj()->AnimWidth();
			int height = app->GetCurProj()->AnimHeight();
			char author[100];
			strcpy(author, app->GetCurProj()->AnimAuthor());
			int fps = app->GetCurProj()->AnimFPS();
			char desc[512];
			strcpy(desc, app->GetCurProj()->AnimDesc());
			if (app->GetCacheProj()) {
				strcpy(name, app->GetCacheProj()->AnimName());
				width = app->GetCacheProj()->AnimWidth();
				height = app->GetCacheProj()->AnimHeight();
				strcpy(author, app->GetCacheProj()->AnimAuthor());
				fps = app->GetCacheProj()->AnimFPS();
				strcpy(desc, app->GetCacheProj()->AnimDesc());
			} else {
				strcpy(name, (char*)"Untitled Animation");
				width = 32;
				height = 32;
				const char* username = ImUtil::GetUsername();
				strncpy(author, username, sizeof(author) - 1);
				author[sizeof(author) - 1] = '\0';
				fps = 10;
				strcpy(desc, "Simple test project");
			}
			if (ImGui::InputText("Name", name, sizeof(name))) {
				app->GetCacheProj()->SetAnimName(name);
			}
			if (ImGui::InputInt("Width", &width)) {
				app->GetCacheProj()->SetAnimWidth(width);
			}
			if (ImGui::InputInt("Height", &height)) {
				app->GetCacheProj()->SetAnimHeight(height);
			}
			if (ImGui::InputText("Author", author, sizeof(author))) {
				app->GetCacheProj()->SetAnimAuthor(author);
			}
			if (ImGui::InputInt("FPS", &fps)) {
				app->GetCacheProj()->SetAnimFPS(fps);
			}
			if (ImGui::InputText("Description", desc, sizeof(desc))) {
				app->GetCacheProj()->SetAnimDesc(desc);
			}

			ImUtil::ButtonRowResult choice = ImUtil::CloseOkButtons();
			if (choice == ImUtil::ButtonRowResult::Secondary ||
				ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
				app->GetPopups()->Close("edit_proj");
				ImGui::CloseCurrentPopup();
			}
			if (choice == ImUtil::ButtonRowResult::Primary ||
				ImGui::IsKeyPressed(ImGuiKey_Enter, false) ||
				ImGui::IsKeyPressed(ImGuiKey_KeypadEnter, false)) {
				app->GetCurProj() = app->GetCacheProj();
				app->GetPopups()->Close("edit_proj");
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	void UIManager::NewProj() {
		Application* app = Application::Get();

		if (app->GetPopups()->IsOpen("new")) {
			ImGui::OpenPopup("NewProj");
		}

		if (ImGui::IsPopupOpen("NewProj")) {
			ImGui::SetNextWindowFocus();
			ImGui::SetNextWindowPos(ImVec2(376, 436), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(350, 336), ImGuiCond_FirstUseEver);
		}

		if (ImGui::BeginPopupModal("NewProj", app->GetPopups()->Get("new"),
				ImGuiWindowFlags_AlwaysAutoResize)) {
			bool justOpened = ImGui::IsWindowAppearing();
			char name[256] = "";
			int width = 32;
			int height = 32;
			char author[100] = "";
			int fps = 0;
			char desc[512] = "";

			if (!app->GetCacheProj()) {
				strcpy(name, (char*)"testproj");
				width = 32;
				height = 32;
				const char* username = ImUtil::GetUsername();
				strncpy(author, username, sizeof(author) - 1);
				author[sizeof(author) - 1] = '\0';
				fps = 10;
				strcpy(desc, "Simple test project");
				app->SetCacheProj(std::make_shared<ProjectFile>(name, width,
					height, author, fps, desc, app->GetWindow(), Col()));
			} else {
				strcpy(name, app->GetCacheProj()->AnimName());
				width = app->GetCacheProj()->AnimWidth();
				height = app->GetCacheProj()->AnimHeight();
				strcpy(author, app->GetCacheProj()->AnimAuthor());
				fps = app->GetCacheProj()->AnimFPS();
				strcpy(desc, app->GetCacheProj()->AnimDesc());

				float r =
					(float)(app->GetCacheProj()->BgCol().r) / MAX_COLOR_VALUE;
				float g =
					(float)(app->GetCacheProj()->BgCol().g) / MAX_COLOR_VALUE;
				float b =
					(float)(app->GetCacheProj()->BgCol().b) / MAX_COLOR_VALUE;
				m_CacheBGCol[0] = r;
				m_CacheBGCol[1] = g;
				m_CacheBGCol[2] = b;
			}

			// GUI inputs for project properties
			if (ImGui::InputText("Name", name, sizeof(name))) {
				app->GetCacheProj()->SetAnimName(name);
			}
			if (ImGui::InputInt("Width", &width)) {
				if (app->GetCurProj())
					app->GetCacheProj()->SetAnimWidth(width, false);
				else {
					if (app->GetCacheProj()) {
						app->GetCacheProj()->SetAnimWidth(width, true);
					}
				}
			}
			if (ImGui::InputInt("Height", &height)) {
				if (app->GetCurProj())
					app->GetCacheProj()->SetAnimHeight(height, false);
				else
					app->GetCacheProj()->SetAnimHeight(height, true);
			}
			if (ImGui::InputText("Author", author, sizeof(author))) {
				app->GetCacheProj()->SetAnimAuthor(author);
			}
			if (ImGui::InputInt("FPS", &fps)) {
				app->GetCacheProj()->SetAnimFPS(fps);
			}
			if (ImGui::InputText("Description", desc, sizeof(desc))) {
				app->GetCacheProj()->SetAnimDesc(desc);
			}
			if (ImGui::ColorPicker3("BG", m_CacheBGCol.data())) {
				if (app->GetCacheProj())
					app->GetCacheProj()->SetBgCol(m_CacheBGCol.data());
			}

			ImUtil::ButtonRowResult choice = ImUtil::CloseOkButtons();
			if (choice == ImUtil::ButtonRowResult::Secondary ||
				ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
				app->GetPopups()->Close("new");
				ImGui::CloseCurrentPopup();
			}
			bool acceptByKey =
				!justOpened &&
				(ImGui::IsKeyPressed(ImGuiKey_Enter, false) ||
					ImGui::IsKeyPressed(ImGuiKey_KeypadEnter, false));
			if (choice == ImUtil::ButtonRowResult::Primary || acceptByKey) {
				app->SetCurProj(app->GetCacheProj());
				app->GetManager()->SetProj(app->GetCurProj());
				app->GetManager()->SetSettings(app->GetSettings());

				app->GetPopups()->Close("new");
			}

			ImGui::EndPopup();
		}
	}

	void UIManager::ExportProj() {
		Application* app = Application::Get();

		if (app->GetPopups()->IsOpen("export")) {
			ImGui::OpenPopup("Export##export");
			app->GetPopups()->Close("export");
		}

		if (ImGui::BeginPopup("Export##export")) {
			const char* formats[] = {"PNGs", "MP4"};
			ImGui::Combo("Export Format", app->GetExportFormatPtr(), formats,
				IM_ARRAYSIZE(formats));
			ImUtil::ButtonRowResult choice = ImUtil::ExportCloseButtons();
			if (choice == ImUtil::ButtonRowResult::Primary ||
				ImUtil::EnterPressed()) {
				FileDialog dialog;
				std::filesystem::path path = dialog.Dir();

				if (app->GetSettings().Sfx)
					app->GetAssetLoader()->PlaySound(s_ExportSound);
				FUNC_INF("Exporting to " << path);
				app->GetCurProj()->Export(
					path.string().c_str(), app->GetExportFormat());

				ImGui::CloseCurrentPopup();
			}
			if (choice == ImUtil::ButtonRowResult::Secondary ||
				ImUtil::EscPressed()) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	void UIManager::CheckKeybinds() {
		Application* app = Application::Get();
		if (m_WaitingForKey != nullptr)
			return;

		ImGuiIO& io = ImGui::GetIO();

		// Check if each shortcut is pressed and perform the appropriate action
		if (app->GetKeybinds().Get("new").IsPressed()) {
			if (app->GetSettings().Sfx)
				app->GetAssetLoader()->PlaySound(s_ProjCreateSound);
			app->GetPopups()->Open("new");
		}
		if (app->GetKeybinds().Get("open").IsPressed()) {
#ifndef MACOS
			std::thread([&]() {
#endif
				app->OpenFileDialog([&]() { app->ReadProjectFile(); });
#ifndef MACOS
			}).detach();
#endif
		}
		if (app->GetKeybinds().Get("quit").IsPressed()) {
			app->GetWindow()->SetShouldClose(true);
		}
		if (app->GetKeybinds().Get("pref").IsPressed()) {
			app->GetPopups()->Open("pref");
		}
		if (app->GetKeybinds().Get("theme").IsPressed()) {
			Themes::g_ThemeEditorOpen = true;
		}
		if (app->GetKeybinds().Get("del").IsPressed()) {
			if (app->GetController()->Sel() && app->GetCurProj()) {
				app->DeleteCurrentSelection();
			}
		}
		if (app->GetController()->Sel() && app->GetCurProj()) {
			if (app->GetKeybinds().Get("move_selection_left").IsPressed())
				app->MoveCurrentSelection(Direction::Left);
			if (app->GetKeybinds().Get("move_selection_right").IsPressed())
				app->MoveCurrentSelection(Direction::Right);
			if (app->GetKeybinds().Get("move_selection_up").IsPressed())
				app->MoveCurrentSelection(Direction::Up);
			if (app->GetKeybinds().Get("move_selection_down").IsPressed())
				app->MoveCurrentSelection(Direction::Down);
		}
		if (app->GetKeybinds().Get("keybinds").IsPressed()) {
			app->GetPopups()->Open("keybinds");
		}
		if (app->GetCurProj()) {
			if (app->GetKeybinds().Get("save").IsPressed()) {
				const char* path = app->GetCurProj()->LastSavePath();

				if (*path) {
					app->SaveAt(path);
				} else {
					app->Save();
				}
			}

			if (app->GetKeybinds().Get("save_as").IsPressed()) {
				app->Save();
			}

			if (app->GetKeybinds().Get("export").IsPressed()) {
				app->GetPopups()->Open("export");
			}
		}
	}

}  // namespace FuncDoodle
