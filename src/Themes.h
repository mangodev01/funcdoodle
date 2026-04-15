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
#include <type_traits>
#include <unordered_map>
#include <vector>

#define TOML_EXCEPTIONS 0

#include <toml++.h>

#include "MacroUtils.h"

#include "UUID.h"

namespace FuncDoodle {
	namespace Themes {
		constexpr const char* s_DefaultTheme =
			"d0c1a009-d09c-4fe6-84f8-eddcb2da38f9";

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

			if (table.contains("other"sv) && table.get("other"sv)->is_table()) {
				toml::table* other = table.get("other"sv)->as_table();

				auto read_number = [](const toml::node* node, double& out) {
					if (!node) {
						return false;
					}
					if (auto fp = node->as_floating_point()) {
						out = fp->get();
						return true;
					}
					if (auto i = node->as_integer()) {
						out = static_cast<double>(i->get());
						return true;
					}
					return false;
				};

				auto read_float = [&](std::string_view key, float& out) {
					double tmp = 0.0;
					if (read_number(other->get(key), tmp)) {
						out = static_cast<float>(tmp);
					}
				};

				auto read_vec2 = [&](std::string_view key, ImVec2& out) {
					const toml::node* node = other->get(key);
					if (!node || !node->is_array()) {
						return;
					}
					const toml::array& arr = *node->as_array();
					if (arr.size() != 2) {
						return;
					}
					double x = 0.0;
					double y = 0.0;
					if (!read_number(arr.get(0), x) ||
						!read_number(arr.get(1), y)) {
						return;
					}
					out = ImVec2(static_cast<float>(x), static_cast<float>(y));
				};

				auto read_bool = [&](std::string_view key, bool& out) {
					const toml::node* node = other->get(key);
					if (!node) {
						return;
					}
					if (auto b = node->as_boolean()) {
						out = b->get();
						return;
					}
					double tmp = 0.0;
					if (read_number(node, tmp)) {
						out = (tmp != 0.0);
					}
				};

				auto read_enum = [&](std::string_view key, auto& out) {
					double tmp = 0.0;
					if (read_number(other->get(key), tmp)) {
						using T = std::remove_reference_t<decltype(out)>;
						out = static_cast<T>(static_cast<int>(tmp));
					}
				};

				read_float("Alpha"sv, style.Alpha);
				read_float("DisabledAlpha"sv, style.DisabledAlpha);
				read_vec2("WindowPadding"sv, style.WindowPadding);
				read_float("WindowRounding"sv, style.WindowRounding);
				read_float("WindowBorderSize"sv, style.WindowBorderSize);
				read_float("WindowBorderHoverPadding"sv,
					style.WindowBorderHoverPadding);
				read_vec2("WindowMinSize"sv, style.WindowMinSize);
				read_vec2("WindowTitleAlign"sv, style.WindowTitleAlign);
				read_enum("WindowMenuButtonPosition"sv,
					style.WindowMenuButtonPosition);
				read_float("ChildRounding"sv, style.ChildRounding);
				read_float("ChildBorderSize"sv, style.ChildBorderSize);
				read_float("PopupRounding"sv, style.PopupRounding);
				read_float("PopupBorderSize"sv, style.PopupBorderSize);
				read_vec2("FramePadding"sv, style.FramePadding);
				read_float("FrameRounding"sv, style.FrameRounding);
				read_float("FrameBorderSize"sv, style.FrameBorderSize);
				read_vec2("ItemSpacing"sv, style.ItemSpacing);
				read_vec2("ItemInnerSpacing"sv, style.ItemInnerSpacing);
				read_vec2("CellPadding"sv, style.CellPadding);
				read_vec2("TouchExtraPadding"sv, style.TouchExtraPadding);
				read_float("IndentSpacing"sv, style.IndentSpacing);
				read_float("ColumnsMinSpacing"sv, style.ColumnsMinSpacing);
				read_float("ScrollbarSize"sv, style.ScrollbarSize);
				read_float("ScrollbarRounding"sv, style.ScrollbarRounding);
				read_float("ScrollbarPadding"sv, style.ScrollbarPadding);
				read_float("GrabMinSize"sv, style.GrabMinSize);
				read_float("GrabRounding"sv, style.GrabRounding);
				read_float("LogSliderDeadzone"sv, style.LogSliderDeadzone);
				read_float("ImageRounding"sv, style.ImageRounding);
				read_float("ImageBorderSize"sv, style.ImageBorderSize);
				read_float("TabRounding"sv, style.TabRounding);
				read_float("TabBorderSize"sv, style.TabBorderSize);
				read_float("TabMinWidthBase"sv, style.TabMinWidthBase);
				read_float("TabMinWidthShrink"sv, style.TabMinWidthShrink);
				read_float("TabCloseButtonMinWidthSelected"sv,
					style.TabCloseButtonMinWidthSelected);
				read_float("TabCloseButtonMinWidthUnselected"sv,
					style.TabCloseButtonMinWidthUnselected);
				read_float("TabBarBorderSize"sv, style.TabBarBorderSize);
				read_float("TabBarOverlineSize"sv, style.TabBarOverlineSize);
				read_float(
					"TableAngledHeadersAngle"sv, style.TableAngledHeadersAngle);
				read_vec2("TableAngledHeadersTextAlign"sv,
					style.TableAngledHeadersTextAlign);
				read_enum("TreeLinesFlags"sv, style.TreeLinesFlags);
				read_float("TreeLinesSize"sv, style.TreeLinesSize);
				read_float("TreeLinesRounding"sv, style.TreeLinesRounding);
				read_float(
					"DragDropTargetRounding"sv, style.DragDropTargetRounding);
				read_float("DragDropTargetBorderSize"sv,
					style.DragDropTargetBorderSize);
				read_float(
					"DragDropTargetPadding"sv, style.DragDropTargetPadding);
				read_float("ColorMarkerSize"sv, style.ColorMarkerSize);
				read_enum("ColorButtonPosition"sv, style.ColorButtonPosition);
				read_vec2("ButtonTextAlign"sv, style.ButtonTextAlign);
				read_vec2("SelectableTextAlign"sv, style.SelectableTextAlign);
				read_float("SeparatorSize"sv, style.SeparatorSize);
				read_float(
					"SeparatorTextBorderSize"sv, style.SeparatorTextBorderSize);
				read_vec2("SeparatorTextAlign"sv, style.SeparatorTextAlign);
				read_vec2("SeparatorTextPadding"sv, style.SeparatorTextPadding);
				read_vec2("DisplayWindowPadding"sv, style.DisplayWindowPadding);
				read_vec2(
					"DisplaySafeAreaPadding"sv, style.DisplaySafeAreaPadding);
				read_bool("DockingNodeHasCloseButton"sv,
					style.DockingNodeHasCloseButton);
				read_float(
					"DockingSeparatorSize"sv, style.DockingSeparatorSize);
				read_float("MouseCursorScale"sv, style.MouseCursorScale);
				read_bool("AntiAliasedLines"sv, style.AntiAliasedLines);
				read_bool(
					"AntiAliasedLinesUseTex"sv, style.AntiAliasedLinesUseTex);
				read_bool("AntiAliasedFill"sv, style.AntiAliasedFill);
				read_float(
					"CurveTessellationTol"sv, style.CurveTessellationTol);
				read_float("CircleTessellationMaxError"sv,
					style.CircleTessellationMaxError);
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
#define SAVE_VEC2(field) \
	other.insert(#field, toml::array{style.field.x, style.field.y})
#define SAVE_BOOL(field) other.insert(#field, style.field)
#define SAVE_ENUM(field) other.insert(#field, static_cast<int>(style.field))

			if (g_SaveThemeOpen) {
				ImGui::Begin("Save current theme", &g_SaveThemeOpen);
				static char themeName[100] = "";
				ImGui::InputText("Name", themeName, 100);
				static char themeAuthor[50] = "";
				ImGui::InputText("Author", themeAuthor, 50);

				if (ImGui::Button("Save")) {
					// should've made the constructor explicit cos assigning a
					// string to a filedialog doesn't make any sense
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
