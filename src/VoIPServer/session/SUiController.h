#pragma once

#ifndef _AFXDLL
#define _AFXDLL
#endif //_AFXDLL

#include <afxwin.h>
#include "AccountManager.h"
#include "TelephonyManager.h"
#include "SessionManager.h"
#include "ISUiController.h"
#include <VoIPClient/session/Data.h>

using namespace std;

class SUiController : public ISUiController
{
private:
	SUiController();
	static SUiController* instance;

	AccountManager* accountManager;
	TelephonyManager* telephonyManager;
	ConferenceDb* conferenceDb;
	ContactDb* contactDb;

	// MainFrame window to notify message
	vector<CWnd*> callbackWnds;

public:	
	static SUiController* getInstance();
	void releaseInstance();
	void setCallbackWnd(CWnd* wnd);
	void postMessage(WPARAM wPram, LPARAM lParam);
	void startCommandLineInterface();
	void startCliThread();
	//-----------------------------------------
	// Post Message back to UI
	void notify(int type, int result) override;

	//-----------------------------------------
	// Functions to get data from Managers
	list<ContactData> get_AllContacts();
	list<ConferenceData> get_AllConferences();

	//-----------------------------------------
	// Functions to request server
	void req_enableContact(CWnd *wnd, string cid, bool enable );
	void req_deleteContact(CWnd* wnd, string cid);
};