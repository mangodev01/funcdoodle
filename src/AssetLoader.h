/**
 * @file AssetLoader.h
 * @brief Centralized asset management system for images, audio, and fonts.
 *
 * This file defines the AssetLoader class, which is responsible for loading,
 * storing, and providing access to all runtime assets used by FuncDoodle.
 *
 * Responsibilities include:
 * - Loading and unloading assets from a configured assets directory
 * - Managing image textures for rendering (via ImGui draw lists)
 * - Loading and playing sound effects through AudioManager
 * - Providing access to application fonts
 *
 * @note Designed to be instantiated once per application lifecycle.
 * @note Acts as a central abstraction layer over asset I/O and runtime resources.
 */

#pragma once

#include "Gui.h"
#include <filesystem>

#include "AudioManager.h"

#include "Ptr.h"

namespace FuncDoodle {
	/**
	 * @class AssetLoader
	 * @brief Convenient abstraction that's responsible for loading assets at a given assetsPath.
	 *
	 * @note Responsible for images, sounds and fonts.
	 * @note Should be instantiated once per program execution.
	 */
	class AssetLoader {
		public:
		explicit AssetLoader(const std::filesystem::path& assetsPath);
		~AssetLoader();
		[[nodiscard]] inline const std::filesystem::path& GetPath() const {
			return m_AssetsPath;
		};
		inline void SetPath(const std::filesystem::path& assetsPath) {
			m_AssetsPath = assetsPath;
		};
		void InitAssets();
		void UnloadAssets();
		void LoadAssets();
		void RenderImage(const char* name, ImDrawList* drawList,
			const ImVec2& pos, const ImVec2& size,
			const ImVec4& tint = ImVec4(1, 1, 1, 1));
		uint32_t LoadImage(const char* name);
		AudioData LoadSound(std::filesystem::path soundName);
		void PlaySound(AudioData data);
		[[nodiscard]] inline ImFont* GetFontRegular() const { return m_FontRegular; }
		[[nodiscard]] inline ImFont* GetFontBold() const { return m_FontBold; }

		private:
		std::filesystem::path m_AssetsPath;
		UniquePtr<AudioManager> m_AudioManager;
		ImFont* m_FontRegular = nullptr;
		ImFont* m_FontBold = nullptr;
	};
}  // namespace FuncDoodle
