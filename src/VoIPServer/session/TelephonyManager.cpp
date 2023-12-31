#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>

#include "TelephonyManager.h"
#include "media/ServerMediaManager.h"

TelephonyManager* TelephonyManager::instance = nullptr;
Json::FastWriter fastWriter;
int connNum = 0;

TelephonyManager::TelephonyManager() {
	sessionControl = nullptr;
	uiControl = nullptr;
	conferenceDb = ConferenceDb::getInstance();
	contactDb = ContactDb::getInstance();

#if MEDIA_ENABLED
	mediaManager = media::ServerMediaManager::getInstance();
#endif // Media
}

TelephonyManager* TelephonyManager::getInstance() {
	if (instance == nullptr) {
		instance = new TelephonyManager();
	}
	return instance;
}

void TelephonyManager::releaseInstance() {
	if (instance != nullptr) {
		instance->release();
		delete instance;
		instance = nullptr;
		std::cout << "TelephonyManager::releaseInstance" << std::endl;
	}
}

void TelephonyManager::release() {
	sessionControl = nullptr;
	conferenceDb = nullptr;
}

string TelephonyManager::generateConnectionId()
{
	while (true) {
		std::string connId("CONN_" + std::to_string(connNum++));
		if (conferenceDb->search("rid", connId).empty()) {
			return connId;
		}
	}
}

void TelephonyManager::onAnswer(Json::Value data) {
	if (sessionControl == nullptr) {
		std::cerr << "onAnswer()/Not register sessionControl" << std::endl;
		return;
	}

	std::string connId = data["rid"].asString();
	std::string from = data["from"].asString();

	std::cout << "onAnswer()/from[" << from << "/connId[" << connId << "]" << std::endl;
	logConnections();

	Json::Value payload;
	if (mediaManager != nullptr) {
		payload = mediaManager->getMediaProperty();
	}
	payload["rid"] = connId;
	payload["result"] = 1;
	payload["result_detail"] = data["result_detail"].asString();

	if (mediaManager != nullptr) {
		Json::Value media;
		media["rid"] = connId;
		media["conferenceSize"] = 2;
		media["myIp"] = data["myIp"].asString();
		mediaManager->startCall(media);
	}

	Json::Value clientMedia;
	clientMedia["rid"] = connId;
	Connection conn = connectionMap[connId];
	std::list<std::string> participants = conn.getParticipants();
	for (const auto& participant : participants) {
		if (participant == from) {
			sessionControl->sendData(304, payload, from);
			if (mediaManager != nullptr) {
				clientMedia["cid"] = from;
				clientMedia["clientIp"] = contactDb->get(participant, "ipAddress");
				clientMedia["name"] = contactDb->get(participant, "name");
				mediaManager->addClient(clientMedia);
			}
			continue;
		}

		sessionControl->sendData(302, payload, participant);
		if (mediaManager != nullptr) {
			clientMedia["cid"] = participant;
			clientMedia["clientIp"] = contactDb->get(participant, "ipAddress");
			clientMedia["name"] = contactDb->get(participant, "name");
			mediaManager->addClient(clientMedia);
		}
	}
}

void TelephonyManager::onReject(Json::Value data) {
	if (sessionControl == nullptr) {
		std::cerr << "onReject()/Not register sessionControl" << std::endl;
		return;
	}

	std::string connId = data["rid"].asString();
	std::string from = data["from"].asString();
	int cause = data["cause"].asInt();

	Json::Value payload;
	payload["rid"] = connId;
	payload["result"] = 2;
	payload["result_detail"] = data["result_detail"].asString();
	payload["cause"] = cause;
	payload["cause_detail"] = data["cause_detail"].asString();

	Connection conn = connectionMap[connId];
	std::list<std::string> participants = conn.getParticipants();
	for (const auto& participant : participants) {
		if (participant == from) {
			if (cause == 1) { /* reject */
				sessionControl->sendData(304, payload, from);
			}
			continue;
		}
		sessionControl->sendData(302, payload, participant);
	}
	connectionMap.erase(connId);

	std::cout << "onReject()/from[" << from << "]connId[" << connId << "]" << endl;
	logConnections();
}

int TelephonyManager::joinableConference(Json::Value data) {
	std::string connId(data["rid"].asString());
	std::string from(data["from"].asString());
	Json::Value payload;

	if (connectionMap.count(connId) == 0) {
		return 1;
	}

	Connection conn = connectionMap[connId];
	if (!conn.isOnTime()) {
		return 2;
	}

	std::list<std::string> conferenceList = conn.getConferenceList();
	std::list<std::string>::iterator it = std::find(conferenceList.begin(), conferenceList.end(), from);
	if (it == conferenceList.end()) {
		return 3;
	}

	return 0;
}

std::string timePrint(std::chrono::system_clock::time_point tp) {
	std::time_t target = std::chrono::system_clock::to_time_t(tp);
	std::time(&target);
	char time[26];
	ctime_s(time, 26, &target);
	return time;
}

void TelephonyManager::manageConferenceLifetime(std::string connId) {
	std::string displayConn = "[" + connId + "]";
	Connection conn = connectionMap[connId];
	std::chrono::system_clock::time_point startTime = std::chrono::system_clock::time_point(std::chrono::seconds(conn.getConferenceStartTime()));
	std::chrono::seconds duration(conn.getDuration());

	std::cout << displayConn << "Created!!! will startTime: " << timePrint(startTime) << std::endl;

	bool silentDestroyed = false;

	while (true) {
		if (connectionMap.count(connId) == 0) {
			silentDestroyed = true;
			break;
		}
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		std::cout << displayConn << "Alive(" << timePrint(now) << ")" << std::endl;

		// wait 1 minute
		std::this_thread::sleep_for(std::chrono::minutes(1));
		std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();

		// timeover
		if (currentTime >= startTime + duration) {
			break;
		}
	}
	std::cout << displayConn << "connection Closed" << endl;
	if (!silentDestroyed) {
		removeConference(connId);
	}
}

void TelephonyManager::logConnections()
{
	std::cout << "--------------------------------------------------------" << std::endl;
	for (const auto& iter : connectionMap) {		
		Connection conn = iter.second;
		std::string connId = conn.getId();
		bool isConference = conn.isConference();
		cout << "[" << connId << "]/";
		std::list<std::string> conferences = conn.getConferenceList();
		std::list<std::string> participants = conn.getParticipants();
		if (conferences.size() > 0) {
			std::cout << "[Conferences]:";
			for (std::string conference : conferences) {
				std::cout << conference << " ";
			}
		}
		if (participants.size() > 0) {
			std::cout << "[Participants]:";
			for (std::string participant : participants) {
				std::cout << participant << " ";
			}
		}
		std::cout << endl;
	}
	std::cout << "--------------------------------------------------------" << std::endl;
}

// Implement interface
void TelephonyManager::setSessionControl(SessionControl* control) {
	sessionControl = control;
}

void TelephonyManager::setUiControl(ISUiController* control)
{
	uiControl = control;
}

void TelephonyManager::handleOutgoingCall(Json::Value data) {
	if (sessionControl == nullptr) {
		std::cerr << "Not register sessionControl" << std::endl;
		return;
	}
	std::string from = data["from"].asString();
	std::string to = data["to"].asString();

	std::string connId = generateConnectionId();
	Connection conn(connId);
	std::cout << "setParticipant: " << from << ", " << to << std::endl;
	conn.setParticipant(from);
	conn.setParticipant(to);
	connectionMap.insert({ connId, conn });

	Json::Value payload;
	payload["rid"] = connId;
	payload["cid"] = from;

	sessionControl->sendData(301, payload, from);
	sessionControl->sendData(303, payload, to);

	std::cout << "handleOutgoingCall()/from[" << from << "]/to[" << to << "]/connId[" << connId << "]" << endl;
	logConnections();
}

void TelephonyManager::handleOutgoingCallNoUser(Json::Value data) {
	std::string from = data["from"].asString();
	std::string to = data["to"].asString();
	Json::Value payload;
	payload["rid"] = "UNKNOWN";
	payload["result"] = 2;
	payload["result_detail"] = "FAIL";
	payload["cause"] = 3;
	payload["cause_detail"] = "UNREACHABLE";

	sessionControl->sendData(302, payload, from);

	std::cout << "handleOutgoingCallNoUser()/from[" << from << "]/to[" << to << "]" << endl;
	logConnections();
}

void TelephonyManager::handleIncomingCallResponse(Json::Value data) {
	int result = data["result"].asInt();
	if (result == 1) { // 1:ANSWER
		onAnswer(data);
	}
	else if (result == 2) { // 2:REJECT
		onReject(data);
	}
}

void TelephonyManager::handleDisconnect(Json::Value data) {
	if (sessionControl == nullptr) {
		std::cerr << "Not register sessionControl" << std::endl;
		return;
	}
	std::string connId(data["rid"].asString());

	Json::Value clientMedia;
	clientMedia["rid"] = connId;

	Json::Value payload;
	payload["rid"] = connId;
	Connection conn = connectionMap[connId];
	int msgId;
	std::list<std::string> participants = conn.getParticipants();
	for (const auto& participant : participants) {
		connectionMap[connId].removeParticipant(participant);
		msgId = conn.isConference() ? 209 : 305;
		sessionControl->sendData(msgId, payload, participant);
		if (mediaManager != nullptr) {
			clientMedia["cid"] = participant;
			mediaManager->removeClient(clientMedia);
		}
	}
	if (!conn.isConference()) {
		connectionMap.erase(connId);
	}
	std::cout << "handleDisconnect()/connId[" << connId << "]" << endl;
	logConnections();

	if (mediaManager != nullptr) {
		Json::Value media;
		media["rid"] = connId;
		mediaManager->endCall(media);
	}
}

void TelephonyManager::releaseConnection(std::string cid) {
	Connection conn;
	std::string connId;
	// find connection
	for (const auto& iter : connectionMap) {
		conn = iter.second;
		std::list<std::string> participants = conn.getParticipants();
		if (std::count(participants.begin(), participants.end(), cid) > 0) {
			connId = iter.first;
			break;
		}
	}
	if (!connId.empty() && !conn.isConference()) {
		Json::Value payload;
		payload["rid"] = connId;
		handleDisconnect(payload);
	}
}

void TelephonyManager::initializeConference(std::string myIp)
{
	// Create room from database
	Json::Value dbConferences = conferenceDb->get();
	for (Json::Value dbConference : dbConferences) {
		std::string connId = dbConference["rid"].asString();
		Connection conn(connId, dbConference);
		connectionMap.insert({ connId, conn });

		postConferenceCreated(connId, myIp);
	}
	std::cout << "Initial Conference Room Created" << std::endl;
	logConnections();
}

void TelephonyManager::handleCreateConference(Json::Value data) {
	Json::Value payload;
	if (data["duration"].asInt64() > 36000) {
		std::cout << "CreateConference Failed! Invaild parameters" << std::endl;
		payload["result"] = 1;
		payload["cause"] = 1;
		sessionControl->sendData(206, payload, data["host"].asString());
		return;
	}
	std::string connId = generateConnectionId();
	Connection conn(connId, data);
	connectionMap.insert({ connId, conn });
	conferenceDb->update(connId, data); // Add data

	postConferenceCreated(connId, data["myIp"].asString());

	logConnections();

	payload["result"] = 0;
	payload["rid"] = connId;
	sessionControl->sendData(206, payload, data["host"].asString()); // Send feedback
}

void TelephonyManager::postConferenceCreated(std::string connId, std::string myIp) {
	std::cout << "handleCreateConference()/connId[" << connId << "]" << std::endl;
	Connection conn = connectionMap[connId];
	std::thread room(&TelephonyManager::manageConferenceLifetime, instance, connId);
	room.detach();

	if (mediaManager != nullptr) {
		Json::Value media;
		media["rid"] = connId;
		media["conferenceSize"] = conn.getConferenceList().size();
		media["myIp"] = myIp;
		mediaManager->startCall(media);
	}
}

void TelephonyManager::removeConference(std::string connId) {
	Json::Value data;
	data["rid"] = connId;
	handleDisconnect(data);

	Connection conn = connectionMap[connId];
	Json::Value payload;
	payload["result"] = 0;
	payload["rid"] = connId;
	for (const auto& cid : conn.getConferenceList()) {
		sessionControl->sendData(207, payload, cid);
	}

	conferenceDb->remove(connId);
	connectionMap.erase(connId);
	std::cout << "removeConference()/connId[" << connId << "]" << std::endl;
	logConnections();
}

void TelephonyManager::removeConferenceSilent(Json::Value data) {
	std::string connId = data["rid"].asString();
	connectionMap.erase(connId);
}

void TelephonyManager::handleJoinConference(Json::Value data) {
	if (sessionControl == nullptr) {
		std::cerr << "Not register sessionControl" << std::endl;
		return;
	}

	std::string connId(data["rid"].asString());
	std::string from(data["from"].asString());

	int joinable = joinableConference(data);
	Json::Value payloadFail;
	payloadFail["result"] = 2;
	payloadFail["result_detail"] = "FAIL";

	if (joinable == 1) {
		std::cout << "handleJoinConference()/NO_ROOM/from[" << from << "]/connId[" << connId << endl;
		logConnections();

		payloadFail["cause"] = 1;
		payloadFail["cause_detail"] = "NO ROOM";
		sessionControl->sendData(208, payloadFail, from);
		return;
	}
	else if (joinable == 2) {
		std::cout << "handleJoinConference()/NO_TIME/from[" << from << "]/connId[" << connId << endl;
		logConnections();

		payloadFail["cause"] = 2;
		payloadFail["cause_detail"] = "NOT STARTED";
		sessionControl->sendData(208, payloadFail, from);
		return;
	}
	else if (joinable == 3) {
		std::cout << "handleJoinConference()/UNINVITED/from[" << from << "]/connId[" << connId << endl;
		logConnections();

		payloadFail["cause"] = 3;
		payloadFail["cause_detail"] = "UNINVITED";
		sessionControl->sendData(208, payloadFail, from);
		return;
	}

	connectionMap[connId].setParticipant(from);

	Json::Value payload;
	if (mediaManager != nullptr) {
		payload = mediaManager->getMediaProperty();
	}
	payload["result"] = 1;
	payload["result_detail"] = "SUCCESS";
	payload["rid"] = connId;
	sessionControl->sendData(208, payload, from);

	if (mediaManager != nullptr) {
		Json::Value media;
		media["rid"] = connId;
		media["cid"] = from;
		media["clientIp"] = contactDb->get(from, "ipAddress");
		media["name"] = contactDb->get(from, "name");
		mediaManager->addClient(media);
	}

	std::cout << "handleJoinConference()/OK/from[" << from << "]/connId[" << connId << endl;
	logConnections();
}

void TelephonyManager::handleExitConference(Json::Value data) {
	if (sessionControl == nullptr) {
		std::cerr << "handleExitConference()/Not register sessionControl" << std::endl;
		return;
	}

	std::string connId(data["rid"].asString());
	std::string from(data["from"].asString());

	if (connectionMap.find(connId) == connectionMap.end()) {
		std::cout << "handleExitConference()/Not exists[" << connId << "]" << std::endl;
		logConnections();
		return;
	}
	connectionMap[connId].removeParticipant(from);

	Json::Value payload;
	payload["result"] = 1;
	payload["result_detail"] = "SUCCESS";
	sessionControl->sendData(209, payload, from);

	if (mediaManager != nullptr) {
		Json::Value media;
		media["rid"] = connId;
		media["cid"] = from;
		mediaManager->removeClient(media);
	}

	std::cout << "handleExitConference()/from[" << from << "]/connId[" << connId << "]" << std::endl;
	logConnections();
}

void TelephonyManager::handleRequestVideoQualityChange(Json::Value data) {
	if (mediaManager != nullptr) {
		mediaManager->updateClientVideoQuality(data);
	}
}

void TelephonyManager::handleRequestRtpJitterBufferLatencyChange(Json::Value data) {
	if (mediaManager != nullptr) {
		mediaManager->updateRtpJitterBufferLatency(data);
	}
}

void TelephonyManager::notifyVideoQualityChanged(std::string rid, int quality) {
	// send to client (request quality change)
	Connection conn = connectionMap[rid];

	Json::Value payload;
	payload["quality"] = quality;
	std::list<std::string> participants = conn.getParticipants();
	for (const auto& participant : participants) {
		sessionControl->sendData(402, payload, participant);
	}
}