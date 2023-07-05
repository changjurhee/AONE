#pragma once
#include "../media/call_info.h"

struct IClientMediaManager {
	virtual void registerNotifyVideoQualityCallback(void (*notifyVideoQuality)(int)) = 0;
	virtual void setVideoQuality(int video_quality_index) = 0;
	virtual void startCall(ContactInfo& contact_info, OperatingInfo& operate_info) = 0;
	virtual void endCall() = 0;
};