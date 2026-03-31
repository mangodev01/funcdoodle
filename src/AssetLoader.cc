#include "AssetLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Gui.h"

#include <filesystem>
#include <iostream>

#include "MacroUtils.h"

#include <glad/glad.h>

#include "AudioManager.h"

namespace FuncDoodle {
	AssetLoader::AssetLoader(const std::filesystem::path& assetsPath)
		: m_AssetsPath(assetsPath), m_AudioManager(new AudioManager) {
		LoadAssets();
	}
	AssetLoader::~AssetLoader() {
		UnloadAssets();
	}

	void AssetLoader::LoadAssets() {
		FUNC_DBG("Finding assets path...");
		if (std::filesystem::exists(m_AssetsPath)) {
			FUNC_DBG("Found assets path @" << m_AssetsPath.string());
		} else {
			FUNC_FATAL("Failed to find assets path -- " << m_AssetsPath
														<< " doesn't exist");
		}

		ImGuiIO& io = ImGui::GetIO();
		ImFontConfig fontConfig;
		fontConfig.OversampleH = 4;
		fontConfig.OversampleV = 4;
		fontConfig.PixelSnapH = true;

		m_FontRegular = io.Fonts->AddFontFromFileTTF(
			(m_AssetsPath / "Roboto" / "Roboto-Medium.ttf").string().c_str(),
			16.0f, &fontConfig);
		m_FontBold = io.Fonts->AddFontFromFileTTF(
			(m_AssetsPath / "Roboto" / "Roboto-Bold.ttf").string().c_str(),
			16.0f, &fontConfig);
		if (m_FontRegular) {
			io.FontDefault = m_FontRegular;
		}
	}
	void AssetLoader::UnloadAssets() {}

	// NOTE: this function doesn't support svg because stb_image doesn't support
	// it
	//
	// what was i even thinking with this comment? isn't this obvious?
	void AssetLoader::RenderImage(const char* name, ImDrawList* drawList,
		const ImVec2& pos, const ImVec2& size, const ImVec4& tint) {
		int width, height, channels;
		unsigned char* data = stbi_load((m_AssetsPath / name).string().c_str(),
			&width, &height, &channels, 0);
		if (!data) {
			FUNC_FATAL("Failed to load image\n"
					   << (m_AssetsPath / name)
					   << "\nTried to load from assets path -- " << m_AssetsPath
					   << ", error: " << stbi_failure_reason());
		}

		uint32_t textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);

		drawList->AddImage((ImTextureID)(intptr_t)textureID, pos,
			ImVec2(pos.x + size.x, pos.y + size.y), ImVec2(0, 0), ImVec2(1, 1),
			ImGui::ColorConvertFloat4ToU32(tint));
	}
	uint32_t AssetLoader::LoadImage(const char* name) {
		int width, height, channels;
		unsigned char* data = stbi_load((m_AssetsPath / name).string().c_str(),
			&width, &height, &channels, 0);
		if (!data) {
			FUNC_FATAL("Failed to load image\n"
					   << (m_AssetsPath / name)
					   << "\nTried to load from assets path -- " << m_AssetsPath
					   << ", error: " << stbi_failure_reason());
		}

		uint32_t textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);

		return textureID;
	}
	AudioData AssetLoader::LoadSound(std::filesystem::path soundName) {
		if (std::filesystem::exists(m_AssetsPath / soundName)) {
			return m_AudioManager->ParseWav(m_AssetsPath / soundName);
		} else {
			FUNC_FATAL("Sound " << soundName << " doesn't exist in assets/");
		}
	}
	void AssetLoader::PlaySound(AudioData data) {
		m_AudioManager->PlayWav(data);
	}
}  // namespace FuncDoodle
