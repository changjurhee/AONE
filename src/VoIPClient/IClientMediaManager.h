#pragma once
#include "../media/call_info.h"
#include "../json/json.h"
typedef unsigned long long handleptr;

namespace media {

struct IClientMediaManager {
	virtual void setVideoQuality(int video_quality_index) = 0;
	virtual void startCall(Json::Value client_info) = 0;
	virtual void setViewHandler(handleptr view_handler) = 0;
	virtual void endCall(Json::Value client_info) = 0;
};

} // namespace media