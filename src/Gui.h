#pragma once

#include <cstdlib>
#include <filesystem>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <nfd.h>
#include <ostream>
#include <vector>

inline std::ostream& operator<<(std::ostream& os, const ImGuiStyle& style) {
	os << "ImGuiStyle { "
	   << "Alpha: " << style.Alpha << ", "
	   << "DisabledAlpha: " << style.DisabledAlpha << ", "
	   << "WindowPadding: (" << style.WindowPadding.x << ", "
	   << style.WindowPadding.y << "), "
	   << "WindowRounding: " << style.WindowRounding << ", "
	   << "WindowBorderSize: " << style.WindowBorderSize << ", "
	   << "FramePadding: (" << style.FramePadding.x << ", "
	   << style.FramePadding.y << "), "
	   << "FrameRounding: " << style.FrameRounding << ", "
	   << "ItemSpacing: (" << style.ItemSpacing.x << ", " << style.ItemSpacing.y
	   << "), "
	   << "ScrollbarSize: " << style.ScrollbarSize << ", "
	   << "ScrollbarRounding: " << style.ScrollbarRounding << ", "
	   << "GrabMinSize: " << style.GrabMinSize << ", "
	   << "GrabRounding: " << style.GrabRounding << ", "
	   << "TabRounding: " << style.TabRounding << " }\n";

	// Print colors
	os << "Colors:\n";
	for (int i = 0; i < ImGuiCol_COUNT; ++i) {
		const ImVec4& col = style.Colors[i];
		os << "  [" << ImGuiCol_(i) << "] (" << col.x << ", " << col.y << ", "
		   << col.z << ", " << col.w << ")\n";
	}

	return os;
}

class FileDialog {
	public:
		FileDialog(
			const char* filterList = nullptr, const char* defaultPath = nullptr)
			: m_FilterList(filterList), m_DefaultPath(defaultPath) {}

		[[nodiscard]] std::filesystem::path Open() const {
			nfdchar_t* outPath = nullptr;
			nfdresult_t result =
				NFD_OpenDialog(m_FilterList, m_DefaultPath, &outPath);

			if (result == NFD_OKAY) {
				std::filesystem::path path(outPath);
				std::free(outPath);
				return path;
			}
			return {};
		}

		[[nodiscard]] std::filesystem::path Save() const {
			nfdchar_t* outPath = nullptr;
			nfdresult_t result =
				NFD_SaveDialog(m_FilterList, m_DefaultPath, &outPath);

			if (result == NFD_OKAY) {
				std::filesystem::path path(outPath);
				std::free(outPath);
				return path;
			}
			return {};
		}

		[[nodiscard]] std::vector<std::filesystem::path> OpenMultiple() const {
			nfdpathset_t pathSet;
			nfdresult_t result =
				NFD_OpenDialogMultiple(m_FilterList, m_DefaultPath, &pathSet);

			if (result == NFD_OKAY) {
				std::vector<std::filesystem::path> paths;
				size_t count = NFD_PathSet_GetCount(&pathSet);
				paths.reserve(count);
				for (size_t i = 0; i < count; ++i) {
					nfdchar_t* path = NFD_PathSet_GetPath(&pathSet, i);
					paths.emplace_back(path);
				}
				NFD_PathSet_Free(&pathSet);
				return paths;
			}
			return {};
		}

		[[nodiscard]] std::filesystem::path Dir() const {
			nfdchar_t* outPath = nullptr;
			nfdresult_t result = NFD_PickFolder(m_DefaultPath, &outPath);

			if (result == NFD_OKAY) {
				std::filesystem::path path(outPath);
				std::free(outPath);
				return path;
			}
			return {};
		}

	private:
		const char* m_FilterList;
		const char* m_DefaultPath;
};
