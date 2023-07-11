#pragma once

#define UWM_SUI_CONTROLLER			WM_USER+201

// Response message from Manager -> UI Controller -> Mainframe
#define MSG_RESPONSE_UPDATE_DATA			1006 // Update contacts & conferences
#define MSG_RESPONSE_UPDATE_CALL			1007

class ISUiController {
public:
	virtual void notify(int type, int result) = 0;
};