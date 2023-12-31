#pragma once

#include "IAccountManager.h"
#include "SessionControl.h"
#include <list>
#include "Data.h"
#include "../../json/json.h"
#include "IUiController.h"

class AccountManager : public IAccountManager {
private:
	static AccountManager* instance;

	SessionControl* sessionControl;
	IUiController* uiControl;

	AccountManager();

	// Util method
	std::string md5(std::string& data);
	bool isSubstring(const std::string& source, const std::string& target);

public:
	static AccountManager* getInstance();
	static void releaseInstance();
	
	std::list<std::string> myContactDataList;
	std::list<ConferenceData> myConferenceDataList;
	std::list<ContactData> allConatactDataList;	
	std::string myCid;
	
	// Communication with server (Send Msg)
	void registerAccount(std::string id, std::string email, std::string pw, std::string name, int pwdQuestion, std::string pwdAnswer);
	void login(std::string id, std::string pw);
	void logout(std::string cid);
	void updateMyContactList(std::string cid, std::list<std::string>& list);
	void resetPassword(std::string cid, std::string newPassword, int pwdQuestion, std::string pwdAnswer);
	void getAllContact();
	void createConference(long time, long duration, std::list<std::string>& list);
	void getAllConference(std::string cid);
	void deleteConference(std::string rid);

	// Communication with UI
	std::list<ContactData> getMyContactList();
	std::list<ContactData> searchContact(std::string key);
	void deleteContact(std::string cid);
	void addContact(std::string cid);
	void updateMyContact(std::string cid, std::string email, std::string name, std::string password);

	// Listener (Recieve Msg)
	void setSessionControl(SessionControl* control) override;
	void setUiControl(IUiController* control) override;

	void handleConnect(int result) override;
	void handleRegisterContact(Json::Value data) override;
	void handleLogin(Json::Value data) override;
	void handleResetPassword(Json::Value data) override;	
	void handleGetAllContact(Json::Value data) override;
	void handleGetAllMyConference(Json::Value data) override;
	void handleUpdateMyContact(Json::Value data) override;
	void handleCreateConference(Json::Value data) override;
	void handleDeleteConference(Json::Value data) override;
};
