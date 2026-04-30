/**
 * @file LoadedAssets.h
 * @brief Global handles for textures and audio used across FuncDoodle.
 *
 * Contains externally defined asset IDs and audio data loaded at startup.
 * Initialized via GlobalLoadAssets().
 */

#pragma once

#include <iostream>

#include "AssetLoader.h"
#include "Audio.h"
#include "AudioManager.h"

namespace FuncDoodle {
	extern uint32_t s_PlayTexId;
	extern uint32_t s_PauseTexId;
	extern uint32_t s_RewindTexId;
	extern uint32_t s_EndTexId;
	extern uint32_t s_BucketTexId;
	extern uint32_t s_PencilTexId;
	extern uint32_t s_PickerTexId;
	extern uint32_t s_EraserTexId;
	extern uint32_t s_TextTexId;
	extern uint32_t s_AddTexId;
	extern uint32_t s_OpenTexId;
	extern uint32_t s_SelectTexId;
	extern AudioData s_ProjCreateSound;
	extern AudioData s_ProjSaveSound;
	extern AudioData s_ExportSound;
	extern AudioData s_ProjSaveEndSound;
	void GlobalLoadAssets(AssetLoader* assetLoader);
}  // namespace FuncDoodle
