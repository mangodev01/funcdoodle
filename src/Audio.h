/**
 * @file Audio.h
 * @brief Audio data structures and PortAudio management.
 */

#pragma once

#include <cstring>
#include <iostream>
#include <portaudio.h>
#include <vector>

namespace FuncDoodle {
	/**
	 * @struct AudioData
	 * @brief Represents raw PCM audio data.
	 */
	struct AudioData {
		std::vector<float> samples;
		int sampleRate;
		int numChannels;
		size_t cur = 0;
		size_t total = 0;
	};
};	// namespace FuncDoodle

/**
 * @brief PortAudio callback function.
 *
 * @param inBuf Input audio buffer provided by the stream (may be nullptr depending on configuration).
 * @param outBuf Output audio buffer that must be filled with audio samples.
 * @param framesPerBuf Number of audio frames to process in this callback invocation.
 * @param timeInfo Timing information for the current callback (stream time, input/output timestamps).
 * @param statusFlags Flags indicating stream status (e.g., underflow, overflow, clipping).
 * @param userData Pointer to user-defined data passed when the stream was opened.
 *
 * @return PortAudio callback status code (e.g., continue streaming, complete, or abort).
 */
static int paCB(const void* inBuf, void* outBuf, unsigned long framesPerBuf, 
	const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
	void* userData) {
	if (!userData) {
		return paAbort;
	}

	FuncDoodle::AudioData* data = static_cast<FuncDoodle::AudioData*>(userData);
	float* out = static_cast<float*>(outBuf);
	unsigned long framesLeft = data->total - data->cur;
	unsigned long framesToProcess =
		(framesLeft < framesPerBuf) ? framesLeft : framesPerBuf;

	std::memcpy(out, data->samples.data() + data->cur * data->numChannels,
		framesToProcess * data->numChannels * sizeof(float));

	data->cur += framesToProcess;

	if (framesToProcess < framesPerBuf) {
		size_t offset = framesToProcess * data->numChannels;

		memset(out + offset, 0,
			(framesPerBuf - framesToProcess) * data->numChannels *
				sizeof(float));

		return paComplete;
	}

	return paContinue;
}
