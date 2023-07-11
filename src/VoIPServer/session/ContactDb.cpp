#include "ContactDb.h"
#include <iostream>
using namespace std;

ContactDb* ContactDb::instance = nullptr;

ContactDb* ContactDb::getInstance()
{
	if (instance == nullptr) {
		instance = new ContactDb(DB_CONTACT);
	}
	return instance;
}

void ContactDb::setEnable(string cid, bool enable)
{
	update(cid, "enable", enable);
}

bool ContactDb::getEnable(string cid)
{
	Json::Value enabled = get(cid, "enable");
	return enabled.asBool();
}
