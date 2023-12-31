#pragma once

#ifndef _AFXDLL
#define _AFXDLL
#endif //_AFXDLL

#include <afxwin.h>
#include "AccountManager.h"
#include "CallsManager.h"
#include "SessionManager.h"
#include "Data.h"
#include "IUiController.h"

using namespace std;

class UiController : public IUiController
{
private:
	UiController();
	static UiController* instance;

	AccountManager* accountManager;
	CallsManager* callsManager;

	// MainFrame window to notify message
	vector<CWnd*> callbackWnds;

	CallResult result;

public:	
	static UiController* getInstance();
	void releaseInstance();
	void setCallbackWnd(CWnd* wnd);
	void postMessage(WPARAM wPram, LPARAM lParam);
	void startConnection( string serverIp );

	//-----------------------------------------
	// Post Message back to UI
	void notify(int type, int result) override;
	void notifyCallState(CallResult result) override;

	//-----------------------------------------
	// Functions to get data from Managers
	list<ContactData> get_MyContacts();
	list<ContactData> get_SearchResult(string key);
	list<ConferenceData> get_MyConferences();

	//-----------------------------------------
	// Functions to request server
	void req_Connect(CWnd* wnd, string serverIp);
	void req_GetAllContacts(CWnd* wnd);
	void req_GetAllConferences(CWnd* wnd);
	void req_ResetPassword(CWnd* wnd, string id, string newPw, int pwdQ, string pwdA);
	void req_Login(CWnd* wnd, string email, string password);
	void req_Logout(CWnd* wnd);
	void req_Register(CWnd* wnd, string id, string email, string pw, string name, int pwdQ, string pwdA);
	void req_AddMyContact(CWnd* wnd, string id);
	void req_DeleteMyContact(CWnd* wnd, string id);
	void req_updateContact(CWnd* wnd, string email, string name, string password);

	void request_OutgoingCall(CWnd* wnd, string id);
	void request_AnswerCall(CWnd* wnd );
	void request_RejectCall(CWnd* wnd );
	void request_EndCall(CWnd* wnd );

	void request_CreateConference(CWnd* wnd, long time, long duration, list<string>& list);
	void request_JoinConference(CWnd* wnd, string id);
	void request_ExitConference(CWnd* wnd);
	void request_deleteConference(CWnd* wnd, string rid);

	void request_EndOrExitCall(CWnd* wnd);
	void request_VAD(CWnd* wnd, bool on);
	void request_AEC(CWnd* wnd, bool on);
};