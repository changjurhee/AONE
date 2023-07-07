#include "audio_buffer.h"

namespace media {

AudioBuffer::AudioBuffer(int sample_rate, int channels, int bits_per_sample, char* buf, int size) :
	sample_rate_(sample_rate),
	channels_(channels),
	bits_per_sample_(bits_per_sample),
	buf_(buf, buf + size)
{
}

}
