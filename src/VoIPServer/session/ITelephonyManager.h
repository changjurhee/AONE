#pragma once

#include "SessionControl.h"
#include "../../json/json.h"
#include "ISUiController.h"

class ITelephonyManager {
public:
	virtual void setSessionControl(SessionControl* control) = 0;
	virtual void setUiControl(ISUiController* control) = 0;
	virtual void handleOutgoingCall(Json::Value data) = 0;
	virtual void handleOutgoingCallNoUser(Json::Value data) = 0;
	virtual void handleIncomingCallResponse(Json::Value data) = 0;
	virtual void handleDisconnect(Json::Value data) = 0;
	virtual void releaseConnection(std::string cid) = 0;
	// conference
	virtual void initializeConference(std::string myIp) = 0;
	virtual void handleCreateConference(Json::Value data) = 0;
	virtual void handleJoinConference(Json::Value data) = 0;
	virtual void handleExitConference(Json::Value data) = 0;
	virtual void removeConferenceSilent(Json::Value data) = 0;

	virtual void handleRequestVideoQualityChange(Json::Value data) = 0;
	virtual void handleRequestRtpJitterBufferLatencyChange(Json::Value data) = 0;
};