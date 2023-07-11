#pragma once

#include "session/SessionManager.h"
#include "session/AccountManager.h"
#include "session/TelephonyManager.h"

class CommandLineInterface {
private:
	static CommandLineInterface* instance;
public:
	static CommandLineInterface* getInstance();
	void startCli();
};