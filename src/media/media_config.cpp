#include "media_config.h"

#include <string>

namespace media {

unsigned int MediaConfig::GetRtpJitterBufferLatency() {
	return 150;
}

}