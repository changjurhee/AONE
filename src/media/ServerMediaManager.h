#pragma once
#include "IServerMediaManager.h"
#include "../media/MediaManager.h"
#include "../media/VideoMediaPipeline.h"
#include "../media/AudioMediaPipeline.h"
#include "../VoIPServer/session/ISessionMediaCallback.h"

namespace media {
//class ServerMediaManager : public MediaManager, public IServerMediaManager, public ISessionMediaCallback {
class ServerMediaManager : public MediaManager, public IServerMediaManager {
private:
	static ServerMediaManager* instance;
	static int dummy_port;
	vector<PipeMode> video_pipe_mode_list_;
	vector<PipeMode> audio_pipe_mode_list_;
	OperatingInfo* ServerMediaManager::get_operate_info(void);
	ServerMediaManager(int max_pipeline);
	ContactInfo* get_contact_info(Json::Value add_client_info, bool is_remove);
	string server_ip;
	ISessionMediaCallback* sessionCallback_;
	ContactInfo* get_dummy_contact(int number);

	virtual bool notifyVideoQualityChangeNeeded(std::string rid, VideoPresetLevel level) override;
public:
	// For Test
	void updateClientVideoQuality(Json::Value info) override;
	void updateRtpJitterBufferLatency(Json::Value info) override;

	void startCall(Json::Value room_creat_info);
	void endCall(Json::Value room_remove_info) override;
	void addClient(Json::Value add_client_info) override;
	void removeClient(Json::Value remove_client_info) override;
	Json::Value getMediaProperty() override;
	void setSessionCallback(ISessionMediaCallback* callback);
	static ServerMediaManager* getInstance();
	static void releaseInstance();
};

} // namespace media