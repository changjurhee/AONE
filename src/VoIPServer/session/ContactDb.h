
#include "DatabaseManager.h"

#pragma once

class ContactDb : public DatabaseManager {
private:	
	static ContactDb* instance;
	ContactDb( string dbName ) : DatabaseManager(dbName) {}; // Constructor	
public:
	static ContactDb* getInstance();
	void setEnable(string cid, bool enable);
	bool getEnable(string cid);
};