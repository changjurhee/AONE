#ifndef MEDIA_MEDIA_CONFIG_H_
#define MEDIA_MEDIA_CONFIG_H_

#include <cstdint>

namespace media {

class MediaConfig {
public:
	static unsigned int GetRtpJitterBufferLatency();
	static uint64_t ThresholdOfNumLostForChangingVideoQuality();
};

}

#endif