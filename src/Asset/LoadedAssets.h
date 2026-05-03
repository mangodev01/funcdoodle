/**
 * @file LoadedAssets.h
 * @brief Global handles for textures and audio used across FuncDoodle.
 *
 * Contains externally defined asset IDs and audio data loaded at startup.
 * Initialized via GlobalLoadAssets().
 */

#pragma once

#include <iostream>

#include "Asset/AssetLoader.h"
#include "Audio/Audio.h"
#include "Audio/AudioManager.h"

namespace FuncDoodle {
	/**
	 * @var s_PlayTexId
	 * @brief Toolbar play icon texture.
	 */
	extern uint32_t s_PlayTexId;
	/**
	 * @var s_PauseTexId
	 * @brief Toolbar pause icon texture.
	 */
	extern uint32_t s_PauseTexId;
	/**
	 * @var s_RewindTexId
	 * @brief Toolbar rewind icon texture.
	 */
	extern uint32_t s_RewindTexId;
	/**
	 * @var s_EndTexId
	 * @brief Toolbar jump-to-end icon texture.
	 */
	extern uint32_t s_EndTexId;
	/**
	 * @var s_BucketTexId
	 * @brief Bucket tool icon texture.
	 */
	extern uint32_t s_BucketTexId;
	/**
	 * @var s_PencilTexId
	 * @brief Pencil tool icon texture.
	 */
	extern uint32_t s_PencilTexId;
	/**
	 * @var s_PickerTexId
	 * @brief Color picker tool icon texture.
	 */
	extern uint32_t s_PickerTexId;
	/**
	 * @var s_EraserTexId
	 * @brief Eraser tool icon texture.
	 */
	extern uint32_t s_EraserTexId;
	/**
	 * @var s_TextTexId
	 * @brief Reserved text tool icon texture.
	 */
	extern uint32_t s_TextTexId;
	/**
	 * @var s_AddTexId
	 * @brief Add/new action icon texture.
	 */
	extern uint32_t s_AddTexId;
	/**
	 * @var s_OpenTexId
	 * @brief Open action icon texture.
	 */
	extern uint32_t s_OpenTexId;
	/**
	 * @var s_SelectTexId
	 * @brief Selection tool icon texture.
	 */
	extern uint32_t s_SelectTexId;
	/**
	 * @var s_ProjCreateSound
	 * @brief Sound played when creating a project.
	 */
	extern AudioData s_ProjCreateSound;
	/**
	 * @var s_ProjSaveSound
	 * @brief Sound played when saving a project.
	 */
	extern AudioData s_ProjSaveSound;
	/**
	 * @var s_ExportSound
	 * @brief Sound played when exporting.
	 */
	extern AudioData s_ExportSound;
	/**
	 * @var s_ProjSaveEndSound
	 * @brief Sound played when save completes.
	 */
	extern AudioData s_ProjSaveEndSound;
	/**
	 * @fn GlobalLoadAssets(AssetLoader* assetLoader)
	 * @brief Loads global textures and sounds into the shared asset handles.
	 * @param assetLoader Loader used to fetch assets from disk.
	 */
	void GlobalLoadAssets(AssetLoader* assetLoader);
}  // namespace FuncDoodle
