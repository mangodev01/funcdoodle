#pragma once

#include <cstring>
#include <iostream>
#include <portaudio.h>
#include <vector>

namespace FuncDoodle {
	struct AudioData {
		std::vector<float> samples;
		int sampleRate;
		int numChannels;
		size_t cur = 0;
		size_t total = 0;
	};
};	// namespace FuncDoodle

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
