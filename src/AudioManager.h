/**
 * @file AudioManager.h
 * @brief Audio loading and playback system for FuncDoodle.
 *
 * This file defines the AudioManager class, which is responsible for:
 * - Loading/parsing WAV audio files into AudioData
 * - Playing audio samples during runtime
 * - Tracking active playback instances
 * - Providing synchronization utilities for audio lifecycle management
 *
 * Playback tracking is implemented using mutex + condition variable
 * to safely coordinate concurrent audio operations.
 *
 * @note Designed for simple sound effect playback rather than full audio engine usage.
 * @warning Static playback state is shared across all instances.
 */

#pragma once

#include <condition_variable>
#include <filesystem>
#include <mutex>

#include "Audio.h"

namespace FuncDoodle {
	/**
	 * @class AudioManager
	 * @brief Handles loading and playback of audio data.
	 *
	 * Responsible for parsing WAV files and managing audio playback.
	 * Supports asynchronous playback tracking to allow the program to
	 * wait for or coordinate active sound instances.
	 *
	 * @note Uses a shared playback counter with synchronization primitives
	 * to track currently playing audio.
	 */
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
