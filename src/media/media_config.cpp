#include "media_config.h"

#include <string>

namespace media {

unsigned int MediaConfig::GetRtpJitterBufferLatency() {
	return 150;
}

uint64_t MediaConfig::ThresholdOfNumLostForChangingVideoQuality() {
	return 100;
}

}