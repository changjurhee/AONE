#pragma once

#define UWM_UI_CONTROLLER			WM_USER+201

// Response message from Manager -> UI Controller -> Mainframe
#define MSG_RESPONSE_CONNECT		1001 // Server connection
#define MSG_RESPONSE_LOGIN			1002 // Login 
#define MSG_RESPONSE_RESET_PW		1003 // Reset PW
#define MSG_RESPONSE_REGISTER		1004 // Register 
#define MSG_RESPONSE_UPDATE_CONTACT	1005 // Update contact, reset password
#define MSG_RESPONSE_DATA			1006 // Update contacts & conferences
#define MSG_RESPONSE_CALLSTATE		1007 // Call and conference status

#define MSG_RESPONSE_DELETE_CONF	1008 // Delete conference
#define MSG_RESPONSE_JOIN_CONF		1009 // Join conference

#define MSG_RESPONSE_AUDIO_VAD		2001
#define MSG_RESPONSE_AUDIO_AEC		2002

// warning C4091: 'typedef ': 변수를 선언하지 않으면 'CallResult' 왼쪽은 무시됩니다.
/*typedef */struct CallResult {
	std::string callerId;
	int result = 0;
	int cause = 0;

	//Result() {}
	CallResult(int r, int c) : result(r), cause(c) {}
};

class IUiController {
public:
	virtual void notify(int type, int result) = 0;
	virtual void notifyCallState(CallResult result) = 0;
};