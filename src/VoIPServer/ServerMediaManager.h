#pragma once
#include "IServerMediaManager.h"
#include "../media/MediaManager.h"
#include "../media/VideoMediaPipeline.h"
#include "../media/AudioMediaPipeline.h"

class ServerMediaManager : public MediaManager, public IServerMediaManager {
private:
	static ServerMediaManager* instance;
	vector<PipeMode> video_pipe_mode_list_;
	vector<PipeMode> audio_pipe_mode_list_;
	void (*notifyTargetVideoQuality)(int, int);
	OperatingInfo* ServerMediaManager::get_operate_info(void);
	ServerMediaManager(int max_pipeline);
	ContactInfo* get_contact_info(Json::Value add_client_info);
	string server_ip;
public:
	void updateClientVideoQuality(string rid, string cid, int level) override;
	void registerNotifyTargetVideoQualityCallback(void (*notifyTargetVideoQuality)(int, int)) override;
	void startCall(Json::Value room_creat_info);

	void endCall(Json::Value room_remove_info) override;
	void addClient(Json::Value add_client_info) override;
	void removeClient(Json::Value remove_client_info) override;
	static ServerMediaManager* getInstance();
	static void releaseInstance();
};

