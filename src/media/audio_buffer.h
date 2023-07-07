#ifndef MEDIA_AUDIO_BUFFER_H_
#define MEDIA_AUDIO_BUFFER_H_

#include <vector>

namespace media {

class AudioBuffer {

public:
	AudioBuffer(int sample_rate, int channels, int bits_per_sample, char* buf, int size);
	~AudioBuffer() {}

	int sample_rate() const { return sample_rate_; }

private:
	std::vector<char> buf_;
	int sample_rate_;
	int channels_;
	int bits_per_sample_;
};

}

#endif