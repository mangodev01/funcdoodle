#pragma once

#include <condition_variable>
#include <filesystem>
#include <mutex>

#include "Audio.h"

namespace FuncDoodle {
	class AudioManager {
		public:
		AudioManager() {}
		~AudioManager() {}
		void PlayWav(AudioData wavPath);
		AudioData ParseWav(std::filesystem::path wav);
		static void WaitForAllPlayback();

		private:
		static std::mutex s_PlaybackMutex;
		static std::condition_variable s_PlaybackCv;
		static int s_ActivePlaybacks;
	};
};	// namespace FuncDoodle
