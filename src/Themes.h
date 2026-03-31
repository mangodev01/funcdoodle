#pragma once

#include "Gui.h"
#include "imgui.h"
#include "nfd.h"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <string.h>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#define TOML_EXCEPTIONS 0

#include <toml++.h>

#include "MacroUtils.h"

#include "UUID.h"

namespace FuncDoodle {
	namespace Themes {
		struct CustomTheme {
				const char* Name;
				const char* Author;
				ImGuiStyle Style;
				UUID Uuid;
				bool OwnsMeta = false;
				CustomTheme()
					: Uuid(UUID()), Name(""), Author(""), Style(ImGuiStyle()),
					  OwnsMeta(false) {}
				CustomTheme(const char* name, const char* author,
					ImGuiStyle style, UUID uuid, bool ownsMeta = false)
					: Uuid(uuid), Name(name), Author(author), Style(style),
					  OwnsMeta(ownsMeta) {};
		};

		inline std::map<UUID, CustomTheme> g_Themes;
		inline bool g_ThemeEditorOpen = false;
		inline bool g_SaveThemeOpen = false;
		inline void ClearThemes() {
			for (auto& [uuid, theme] : g_Themes) {
				(void)uuid;
				if (theme.OwnsMeta) {
					std::free(const_cast<char*>(theme.Name));
					std::free(const_cast<char*>(theme.Author));
					theme.Name = "";
					theme.Author = "";
					theme.OwnsMeta = false;
				}
			}
			g_Themes.clear();
		}
		inline void ThemeEditor() {
			if (!g_ThemeEditorOpen) {
				return;
			}
			ImGui::ShowStyleEditor(&ImGui::GetStyle());
			// if (g_ThemeEditorOpen) {
				// ImGui::Begin("Theme editor", &g_ThemeEditorOpen,
						// ImGuiWindowFlags_NoCollapse);
//
				// ImGuiStyle& style = ImGui::GetStyle();
//
				// ImVec2 windowSize = ImGui::GetWindowSize();
				// float windowWidth = windowSize.x;
//
				// Calculate the number of col based on the window width
				// int numCol = static_cast<int>(
					// windowWidth /
					// 200);  // Each column has a minimum width of 100px
				// numCol = ImMax(1, numCol);
//
				// ImGui::Columns(numCol, nullptr, false);
				// for (unsigned char col = 0; col < ImGuiCol_COUNT; ++col) {
					// ImGui::PushID(col);
					// ImVec4& color = style.Colors[col];
//
					// if (ImGui::ColorEdit4(ImGui::GetStyleColorName(col),
							// (float*)&color)) {
						// style.Colors[col] = color;
					// }
//
					// if ((col + 1) % numCol == 0) {
						// ImGui::NextColumn();
					// }
//
					// ImGui::PopID();
				// }
//
				// End the cols
				// ImGui::Columns(1);
//
				// if (ImGui::Button("Close") ||
					// ImGui::IsKeyPressed(ImGuiKey_Escape) ||
					// ImGui::IsKeyPressed(ImGuiKey_Enter) ||
					// ImGui::IsKeyPressed(ImGuiKey_KeypadEnter)) {
					// g_ThemeEditorOpen = false;
				// }
//
				// ImGui::End();
			// }
		}
		inline CustomTheme g_LastLoadedTheme;
		inline CustomTheme* LoadThemeFromFile(const char* path) {
			using namespace std::string_view_literals;

			toml::parse_result result =
				toml::parse_file(std::string_view(path));
			if (!result) {
				FUNC_WARN("Failed to parse theme file... it's probably not "
						  "valid TOML");
				FUNC_WARN("error: " << result.error());
				return nullptr;
			}

			toml::table table = result.table();
			if (!table.contains("meta"sv) || !table.contains("colors"sv)) {
				FUNC_WARN(
					"Theme file is missing required fields (meta/colors)");
				return nullptr;
			}

			toml::table* meta = table.get("meta"sv)->as_table();
			const char* name = meta->get("name")->as_string()->get().c_str();
			const char* author =
				meta->get("author")->as_string()->get().c_str();
			const char* uuid = meta->get("uuid")->as_string()->get().c_str();

			// Allocate memory for name and author (because TOML parser returns
			// temporary strings)
			char* name_copy = (char*)malloc(strlen(name) + 1);
			char* author_copy = (char*)malloc(strlen(author) + 1);
			char* uuid_copy = (char*)malloc(strlen(uuid) + 1);

			if (!name_copy || !author_copy || !uuid_copy) {
				if (name_copy)
					free(name_copy);
				if (author_copy)
					free(author_copy);
				if (uuid_copy)
					free(uuid_copy);
				FUNC_ERR("Memory allocation failed!");
				return nullptr;
			}

			strcpy(name_copy, name);
			strcpy(author_copy, author);
			strcpy(uuid_copy, uuid);

			toml::table* colors = table.get("colors"sv)->as_table();
			ImGuiStyle style = ImGui::GetStyle();

			for (const auto& [k, v] : *colors) {
				if (!v.is_array())
					continue;

				// Convert key to an integer safely
				int parsed;
				try {
					parsed = std::stoi(std::string(k.str()));
				} catch (...) {
					FUNC_WARN("Invalid color key: " << k.str());
					continue;
				}

				if (parsed < 0 || parsed >= ImGuiCol_COUNT) {
					FUNC_WARN("Invalid ImGui color index: " << parsed);
					continue;
				}

				toml::array arr = *v.as_array();
				if (arr.size() != 4)
					continue;

				style.Colors[parsed] =
					ImVec4(arr.get(0)->as_floating_point()->get(),
						arr.get(1)->as_floating_point()->get(),
						arr.get(2)->as_floating_point()->get(),
						arr.get(3)->as_floating_point()->get());
			}

			g_LastLoadedTheme = CustomTheme{name_copy, author_copy, style,
				UUID::FromString(uuid_copy), true};
			std::free(uuid_copy);

			return &g_LastLoadedTheme;
		}
		inline void LoadThemes(std::filesystem::path path) {
			ClearThemes();
			if (std::filesystem::exists(path) &&
				std::filesystem::is_directory(path)) {
				for (std::filesystem::directory_entry e :
					std::filesystem::directory_iterator(path)) {
					if (!e.is_regular_file()) {
						FUNC_GRAY("Skipping "
								  << e.path()
								  << "because it isn't a regular file");
						continue;
					}
					if (e.path().filename().string().starts_with(".")) {
						continue;
					}
					std::string pathStr = e.path().string();
					const char* path = pathStr.c_str();
					CustomTheme* theme = LoadThemeFromFile(path);
					if (theme) {
						auto [it, inserted] =
							g_Themes.emplace(theme->Uuid, *theme);
						if (!inserted && theme->OwnsMeta) {
							std::free(const_cast<char*>(theme->Name));
							std::free(const_cast<char*>(theme->Author));
							theme->Name = "";
							theme->Author = "";
							theme->OwnsMeta = false;
						}
					}
				}
			} else {
				FUNC_FATAL("Failed to load themes -- either the themes/ "
						   "directory doesn't exist, or it isn't a directory");
			}
			// NOTE: std::map is keyed/sorted by UUID, so name-based sorting is
			// intentionally omitted here.
		}

		inline void SaveCurrentTheme() {
			// Save macros for ImGuiStyle fields
			#define SAVE_FLOAT(field) other.insert(#field, style.field)
			#define SAVE_VEC2(field) other.insert(#field, toml::array{style.field.x, style.field.y})
			#define SAVE_BOOL(field) other.insert(#field, style.field)
			#define SAVE_ENUM(field) other.insert(#field, static_cast<int>(style.field))

			if (g_SaveThemeOpen) {
				ImGui::Begin("Save current theme", &g_SaveThemeOpen);
				static char themeName[100] = "";
				ImGui::InputText("Name", themeName, 100);
				static char themeAuthor[50] = "";
				ImGui::InputText("Author", themeAuthor, 50);


				if (ImGui::Button("Save")) {
					// should've made the constructor explicit cos assigning a string to a filedialog doesn't make any sense
					FileDialog dialog = "toml";
					std::filesystem::path res = dialog.Save();

					{
						using namespace std::string_view_literals;
						toml::table theme = toml::table();
						toml::table meta = toml::table();

						meta.insert("name"sv, themeName);
						meta.insert("author"sv, themeAuthor);
						meta.insert("uuid"sv, UUID::Gen().ToString());
						theme.insert("meta"sv, meta);

						toml::table colors = toml::table();
						ImGuiStyle& style = ImGui::GetStyle();

						for (unsigned char i = 0; i < ImGuiCol_COUNT; ++i) {
							std::string is = std::to_string(i);
							std::string_view iv = std::string_view(is);
							colors.insert(
								iv, toml::array{style.Colors[i].x,
										style.Colors[i].y, style.Colors[i].z,
										style.Colors[i].w});
						}

						theme.insert("colors"sv, colors);

						toml::table other = toml::table();
						
						SAVE_FLOAT(Alpha);
						SAVE_FLOAT(DisabledAlpha);
						SAVE_VEC2(WindowPadding);
						SAVE_FLOAT(WindowRounding);
						SAVE_FLOAT(WindowBorderSize);
						SAVE_FLOAT(WindowBorderHoverPadding);
						SAVE_VEC2(WindowMinSize);
						SAVE_VEC2(WindowTitleAlign);
						SAVE_ENUM(WindowMenuButtonPosition);
						SAVE_FLOAT(ChildRounding);
						SAVE_FLOAT(ChildBorderSize);
						SAVE_FLOAT(PopupRounding);
						SAVE_FLOAT(PopupBorderSize);
						SAVE_VEC2(FramePadding);
						SAVE_FLOAT(FrameRounding);
						SAVE_FLOAT(FrameBorderSize);
						SAVE_VEC2(ItemSpacing);
						SAVE_VEC2(ItemInnerSpacing);
						SAVE_VEC2(CellPadding);
						SAVE_VEC2(TouchExtraPadding);
						SAVE_FLOAT(IndentSpacing);
						SAVE_FLOAT(ColumnsMinSpacing);
						SAVE_FLOAT(ScrollbarSize);
						SAVE_FLOAT(ScrollbarRounding);
						SAVE_FLOAT(ScrollbarPadding);
						SAVE_FLOAT(GrabMinSize);
						SAVE_FLOAT(GrabRounding);
						SAVE_FLOAT(LogSliderDeadzone);
						SAVE_FLOAT(ImageRounding);
						SAVE_FLOAT(ImageBorderSize);
						SAVE_FLOAT(TabRounding);
						SAVE_FLOAT(TabBorderSize);
						SAVE_FLOAT(TabMinWidthBase);
						SAVE_FLOAT(TabMinWidthShrink);
						SAVE_FLOAT(TabCloseButtonMinWidthSelected);
						SAVE_FLOAT(TabCloseButtonMinWidthUnselected);
						SAVE_FLOAT(TabBarBorderSize);
						SAVE_FLOAT(TabBarOverlineSize);
						SAVE_FLOAT(TableAngledHeadersAngle);
						SAVE_VEC2(TableAngledHeadersTextAlign);
						SAVE_ENUM(TreeLinesFlags);
						SAVE_FLOAT(TreeLinesSize);
						SAVE_FLOAT(TreeLinesRounding);
						SAVE_FLOAT(DragDropTargetRounding);
						SAVE_FLOAT(DragDropTargetBorderSize);
						SAVE_FLOAT(DragDropTargetPadding);
						SAVE_FLOAT(ColorMarkerSize);
						SAVE_ENUM(ColorButtonPosition);
						SAVE_VEC2(ButtonTextAlign);
						SAVE_VEC2(SelectableTextAlign);
						SAVE_FLOAT(SeparatorSize);
						SAVE_FLOAT(SeparatorTextBorderSize);
						SAVE_VEC2(SeparatorTextAlign);
						SAVE_VEC2(SeparatorTextPadding);
						SAVE_VEC2(DisplayWindowPadding);
						SAVE_VEC2(DisplaySafeAreaPadding);
						SAVE_BOOL(DockingNodeHasCloseButton);
						SAVE_FLOAT(DockingSeparatorSize);
						SAVE_FLOAT(MouseCursorScale);
						SAVE_BOOL(AntiAliasedLines);
						SAVE_BOOL(AntiAliasedLinesUseTex);
						SAVE_BOOL(AntiAliasedFill);
						SAVE_FLOAT(CurveTessellationTol);
						SAVE_FLOAT(CircleTessellationMaxError);

						std::ofstream f(res);
						if (!f) {
							FUNC_ERR("Failed to open file...");
							g_SaveThemeOpen = false;
							ImGui::End();
							return;
						}
						FUNC_INF("Saving current theme to " << res << "!");
						f << theme;
						f.close();
						g_SaveThemeOpen = false;
					}
				}
				ImGui::End();
			}
		}
	}  // namespace Themes
}  // namespace FuncDoodle
