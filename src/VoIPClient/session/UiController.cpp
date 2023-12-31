#include "UiController.h"
#include "SessionManager.h"
#include <thread>
#include "../CommandLineInterface.h"
#include <algorithm>
using namespace std;

UiController* UiController::instance = nullptr;
UiController::UiController() : result(0, 0)
{
	cout << "UiController()" << endl;	
	accountManager = AccountManager::getInstance();
	callsManager = CallsManager::getInstance();

	accountManager->setUiControl(this);
	callsManager->setUiControl(this);
}

UiController* UiController::getInstance() {
	if (instance == nullptr) {
		instance = new UiController();
	}
	return instance;
}

void UiController::releaseInstance()
{
	callsManager->releaseInstance();
	accountManager->releaseInstance();
}

void UiController::setCallbackWnd(CWnd* wnd)
{
	vector<CWnd*>::iterator iter;
	for (iter = callbackWnds.begin(); iter != callbackWnds.end(); iter++) {
		if (*iter == wnd) {
			return;
		}
	}
	callbackWnds.push_back(wnd);
}

void UiController::postMessage(WPARAM wPram, LPARAM lParam)
{
	callbackWnds.erase(remove_if(callbackWnds.begin(), callbackWnds.end(), [](CWnd* p) {return !IsWindow(p->m_hWnd);}), callbackWnds.end());
	for (int i = 0; i < callbackWnds.size(); i++) {
		if (IsWindow(callbackWnds[i]->m_hWnd)) {
			callbackWnds[i]->PostMessageW(UWM_UI_CONTROLLER, wPram, lParam);
			//cout << "UiController::postMessage() = " << lParam << endl;
		}
	}
}

void UiController::startConnection( string serverIp )
{
	std::string ip = serverIp;
	int serverPort = 0;

	// SessionManager init
	SessionManager::getInstance()->init(ip.c_str(), serverPort);
	// Start CLI
	CommandLineInterface* cli = CommandLineInterface::getInstance();
	cli->startCli(accountManager, callsManager);
}

void UiController::notify(int type, int result)
{
	postMessage(type, (LPARAM)result);
}

void UiController::notifyCallState(CallResult r)
{
	result = r;
	postMessage(MSG_RESPONSE_CALLSTATE, (LPARAM)(&result));
}

list<ContactData> UiController::get_MyContacts()
{
	return accountManager->getMyContactList();
}

list<ConferenceData> UiController::get_MyConferences()
{
	return accountManager->myConferenceDataList;
}

list<ContactData> UiController::get_SearchResult(string key)
{
	return accountManager->searchContact(key);
}

void UiController::req_Connect(CWnd* wnd, string serverIp)
{
	setCallbackWnd(wnd);
    std::thread commandline(&UiController::startConnection, this, serverIp);
    commandline.detach();
}

void UiController::req_GetAllContacts(CWnd* wnd)
{
	setCallbackWnd(wnd);
	accountManager->getAllContact();
}

void UiController::req_GetAllConferences(CWnd* wnd)
{
	setCallbackWnd(wnd);
	accountManager->getAllConference(accountManager->myCid);
}

void UiController::req_ResetPassword(CWnd* wnd, string id, string newPw, int pwdQ, string pwdA)
{
	setCallbackWnd(wnd);
	accountManager->resetPassword(id, newPw, pwdQ, pwdA);
}

void UiController::req_Login(CWnd* wnd, string email, string password)
{
	setCallbackWnd(wnd);
	accountManager->login(email, password);
}

void UiController::req_Logout(CWnd* wnd)
{
	setCallbackWnd(wnd);
	accountManager->logout(accountManager->myCid);
}

void UiController::req_Register(CWnd* wnd, string id, string email, string pw, string name, int pwdQ, string pwdA)
{
	setCallbackWnd(wnd);
	accountManager->registerAccount(id, email, pw, name, pwdQ, pwdA);
}

void UiController::req_AddMyContact(CWnd* wnd, string id)
{
	setCallbackWnd(wnd);
	accountManager->addContact(id);
}

void UiController::req_DeleteMyContact(CWnd* wnd, string id)
{
	setCallbackWnd(wnd);
	accountManager->deleteContact(id);
}

void UiController::req_updateContact(CWnd* wnd, string email, string name, string password)
{
	setCallbackWnd(wnd);
	accountManager->updateMyContact(accountManager->myCid, email, name, password);
}

void UiController::request_OutgoingCall(CWnd* wnd, string id)
{
	setCallbackWnd(wnd);
	callsManager->startOutgoingCall(id);
}

void UiController::request_AnswerCall(CWnd* wnd)
{
	setCallbackWnd(wnd);
	callsManager->answerCall();
}

void UiController::request_RejectCall(CWnd* wnd)
{
	setCallbackWnd(wnd);
	callsManager->rejectCall();
}

void UiController::request_EndCall(CWnd* wnd)
{
	setCallbackWnd(wnd);
	callsManager->disconnectCall();
}

void UiController::request_CreateConference(CWnd* wnd, long time, long duration, list<string>& list)
{
	setCallbackWnd(wnd);
	accountManager->createConference(time, duration, list);
}

void UiController::request_JoinConference(CWnd* wnd, string id)
{
	setCallbackWnd(wnd);
	callsManager->joinConference(id);
}

void UiController::request_ExitConference(CWnd* wnd)
{
	setCallbackWnd(wnd);
	callsManager->exitConference();
}

void UiController::request_deleteConference(CWnd* wnd, string rid)
{
	setCallbackWnd(wnd);
	accountManager->deleteConference(rid);
}

void UiController::request_EndOrExitCall(CWnd* wnd)
{
	Call* call = callsManager->getCall();
	if (call == nullptr) {
		return;
	}

	if (call->isConference()) {
		request_ExitConference(wnd);
	}
	else {
		request_EndCall(wnd);
	}
}

void UiController::request_VAD(CWnd* wnd, bool on)
{
	setCallbackWnd(wnd);
	// TODO : VAD Feature on off
}

void UiController::request_AEC(CWnd* wnd, bool on)
{
	setCallbackWnd(wnd);
	// TODO : AEC Feature on off
}

