#include "LoadedAssets.h"

#include <cstdint>

namespace FuncDoodle {
	uint32_t s_PlayTexId = 0;
	uint32_t s_PauseTexId = 0;
	uint32_t s_RewindTexId = 0;
	uint32_t s_EndTexId = 0;
	uint32_t s_PencilTexId = 0;
	uint32_t s_PickerTexId = 0;
	uint32_t s_EraserTexId = 0;
	uint32_t s_BucketTexId = 0;
	uint32_t s_SelectTexId = 0;
	uint32_t s_TextTexId = 0;
	uint32_t s_AddTexId = 0;
	uint32_t s_OpenTexId = 0;

	AudioData s_ProjCreateSound;
	AudioData s_ProjSaveSound;
	AudioData s_ExportSound;
	AudioData s_ProjSaveEndSound;

	void AssetLoader::LoadAssets() {
		// images
		s_PlayTexId = LoadImage("play.png");
		s_PauseTexId = LoadImage("pause.png");
		s_RewindTexId = LoadImage("rewind.png");
		s_EndTexId = LoadImage("end.png");
		s_PencilTexId = LoadImage("pencil.png");
		s_PickerTexId = LoadImage("picker.png");
		s_EraserTexId = LoadImage("eraser.png");
		s_BucketTexId = LoadImage("bucket.png");
		s_TextTexId = LoadImage("text.png");
		s_AddTexId = LoadImage("add.png");
		s_OpenTexId = LoadImage("folder.png");
		s_SelectTexId = LoadImage("select.png");

		// audio
		s_ProjCreateSound = LoadSound("create.wav");
		s_ProjSaveSound = LoadSound("save.wav");
		s_ExportSound = LoadSound("export.wav");
		s_ProjSaveEndSound = LoadSound("saveend.wav");
	}
}  // namespace FuncDoodle
