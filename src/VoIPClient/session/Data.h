#pragma once
#include <string>
#include <list>

struct ContactData {
	std::string cid;
	std::string email;
	std::string name;
	bool enabled;
	bool login;
};

struct ConferenceData {
	std::string rid;
	long dataAndTime;
	long duration;
	std::list<std::string> participants;
}; 