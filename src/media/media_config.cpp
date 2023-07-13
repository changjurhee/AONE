#include "media_config.h"

#include <string>

namespace media {

unsigned int MediaConfig::GetRtpJitterBufferLatency() {
	return 150;
}

unsigned int MediaConfig::GetNumOfContinousLost() {
	return 3;
}

uint64_t MediaConfig::ThresholdOfNumLostForChangingVideoQuality() {
	return 10;
}

}