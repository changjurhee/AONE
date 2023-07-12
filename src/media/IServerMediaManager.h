#pragma once
#include "../media/call_info.h"
#include <vector>
#include "../json/json.h"

namespace media {

struct IServerMediaManager {
public:
	virtual void updateClientVideoQuality(Json::Value info) = 0;
	virtual void updateRtpJitterBufferLatency(Json::Value info) = 0;
	virtual void startCall(Json::Value room_creat_info) = 0;
	virtual void endCall(Json::Value room_remove_info) = 0;
	virtual void addClient(Json::Value add_client_info) = 0;
	virtual void removeClient(Json::Value remove_client_info) = 0;
	virtual Json::Value getMediaProperty() = 0;
};

} // namespace media