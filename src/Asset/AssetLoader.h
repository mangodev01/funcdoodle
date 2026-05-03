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
 * @note Acts as a central abstraction layer over asset I/O and runtime
 * resources.
 */

#pragma once

#include "UI/Gui.h"
#include <filesystem>

#include "Audio/AudioManager.h"

#include "Util/Ptr.h"

namespace FuncDoodle {
	/**
	 * @class AssetLoader
	 * @brief Convenient abstraction that's responsible for loading assets at a
	 * given assetsPath.
	 *
	 * @note Responsible for images, sounds and fonts.
	 * @note Should be instantiated once per program execution.
	 */
	class AssetLoader {
		public:
		/**
		 * @fn AssetLoader
		 * @brief Creates an asset loader rooted at the given assets directory.
		 *
		 * @param assetsPath Filesystem path containing runtime assets.
		 */
		explicit AssetLoader(std::filesystem::path assetsPath);
		~AssetLoader();
		/**
		 * @fn GetPath
		 * @brief Returns the current assets directory.
		 *
		 * @return Path used for asset loading.
		 */
		[[nodiscard]] const std::filesystem::path& GetPath() const {
			return m_AssetsPath;
		};
		/**
		 * @fn SetPath
		 * @brief Changes the assets directory used for future loads.
		 *
		 * @param assetsPath New assets directory.
		 */
		void SetPath(const std::filesystem::path& assetsPath) {
			m_AssetsPath = assetsPath;
		};
		/**
		 * @fn InitAssets
		 * @brief Initializes fonts and other asset-backed runtime state.
		 */
		void InitAssets();
		/**
		 * @fn UnloadAssets
		 * @brief Releases loaded asset resources.
		 */
		void UnloadAssets();
		/**
		 * @fn LoadAssets
		 * @brief Loads shared textures and sounds from disk.
		 */
		void LoadAssets();
		/**
		 * @fn RenderImage
		 * @brief Draws a loaded image into an ImGui draw list.
		 *
		 * @param name Asset name to render.
		 * @param drawList Draw list to submit image commands to.
		 * @param pos Top-left position in screen space.
		 * @param size Render size in pixels.
		 * @param tint Tint color applied while rendering.
		 */
		void RenderImage(const char* name, ImDrawList* drawList,
			const ImVec2& pos, const ImVec2& size,
			const ImVec4& tint = ImVec4(1, 1, 1, 1));
		/**
		 * @fn LoadImage
		 * @brief Loads an image asset and returns its texture handle.
		 *
		 * @param name Asset name to load.
		 * @return OpenGL texture ID for the loaded image.
		 */
		uint32_t LoadImage(const char* name);
		/**
		 * @fn LoadSound
		 * @brief Loads a sound asset into memory.
		 *
		 * @param soundName Relative sound asset path.
		 * @return Decoded audio data.
		 */
		AudioData LoadSound(std::filesystem::path soundPath);
		/**
		 * @fn PlaySound
		 * @brief Plays decoded audio data through the audio manager.
		 *
		 * @param data Audio sample buffer to play.
		 */
		void PlaySound(AudioData data);
		/**
		 * @fn GetFontRegular
		 * @brief Returns the regular UI font.
		 *
		 * @return Pointer to the regular font.
		 */
		[[nodiscard]] ImFont* GetFontRegular() const { return m_FontRegular; }
		/**
		 * @fn GetFontBold
		 * @brief Returns the bold UI font.
		 *
		 * @return Pointer to the bold font.
		 */
		[[nodiscard]] ImFont* GetFontBold() const { return m_FontBold; }

		private:
		std::filesystem::path m_AssetsPath;
		UniquePtr<AudioManager> m_AudioManager;
		ImFont* m_FontRegular = nullptr;
		ImFont* m_FontBold = nullptr;
	};
}  // namespace FuncDoodle
