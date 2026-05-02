#include "AudioManager.h"
#include "Audio.h"

#include <af/AudioFile.h>
#include <chrono>
#include <filesystem>
#include <thread>

namespace FuncDoodle {
	std::mutex AudioManager::s_PlaybackMutex;
	std::condition_variable AudioManager::s_PlaybackCv;
	int AudioManager::s_ActivePlaybacks = 0;

	// Plays the provided AudioData asynchronously.
	void AudioManager::PlayWav(AudioData wav) {
		// Allocate a heap copy so that the callback has a valid pointer
		auto* audioData = new AudioData(std::move(wav));
		{
			std::scoped_lock lk(s_PlaybackMutex);
			++s_ActivePlaybacks;
		}

		PaStream* stream = nullptr;
		PaError err = Pa_OpenDefaultStream(&stream,
			0,						 // no input channels
			audioData->numChannels,	 // output channels
			paFloat32,				 // sample format
			audioData->sampleRate, paFramesPerBufferUnspecified,
			paCB,		 // callback function
			audioData);	 // user data
		if (err != paNoError) {
			std::cerr << "Error opening default stream: "
					  << Pa_GetErrorText(err) << '\n';
			delete audioData;
			{
				std::scoped_lock lk(s_PlaybackMutex);
				--s_ActivePlaybacks;
			}
			s_PlaybackCv.notify_all();
			return;
		}

		err = Pa_StartStream(stream);
		if (err != paNoError) {
			std::cerr << "Error starting stream: " << Pa_GetErrorText(err)
					  << '\n';
			Pa_CloseStream(stream);
			delete audioData;
			{
				std::scoped_lock lk(s_PlaybackMutex);
				--s_ActivePlaybacks;
			}
			s_PlaybackCv.notify_all();
			return;
		}

		// Launch a detached thread to monitor playback completion
		std::thread([stream, audioData]() {
			// Wait until playback is finished
			while (Pa_IsStreamActive(stream) == 1) {
				Pa_Sleep(100);
			}
			// Clean up PortAudio
			Pa_StopStream(stream);
			Pa_CloseStream(stream);

			// Free the audio data allocated in PlayWav
			delete audioData;
			{
				std::scoped_lock lk(s_PlaybackMutex);
				--s_ActivePlaybacks;
			}
			s_PlaybackCv.notify_all();
		}).detach();
	}

	void AudioManager::WaitForAllPlayback() {
		std::unique_lock<std::mutex> lk(s_PlaybackMutex);
		s_PlaybackCv.wait(lk, [] { return s_ActivePlaybacks == 0; });
	}

	// Parses a WAV file from the given path, then plays it.
	AudioData AudioManager::ParseWav(std::filesystem::path wavPath) {
		AudioFile<float> audioFile;
		if (!audioFile.load(wavPath.string())) {
			std::cerr << "Failed to load wav file: " << wavPath << '\n';
			std::exit(-1);
		}

		AudioData audioData;
		audioData.sampleRate = audioFile.getSampleRate();
		audioData.numChannels = audioFile.getNumChannels();
		audioData.total = audioFile.getNumSamplesPerChannel();
		audioData.cur = 0;	// Start at beginning

		// Resize to hold all interleaved samples:
		audioData.samples.resize(audioData.total * audioData.numChannels);

		// AudioFile stores samples per channel (samples[channel][sampleIndex])
		// Interleave the channels into a single vector.
		for (int ch = 0; ch < audioData.numChannels; ++ch) {
			for (size_t i = 0; i < audioData.total; ++i) {
				audioData.samples[(i * audioData.numChannels) + ch] =
					audioFile.samples[ch][i];
			}
		}
		return audioData;
	}

}  // namespace FuncDoodle
