#pragma once
#include "IClientMediaManager.h"
#include "../media/MediaManager.h"
#include "../media/VideoMediaPipeline.h"
#include "../media/AudioMediaPipeline.h"

#define DEFAULT_CLIENT_CID "0"
#define DEFAULT_CLIENT_RID "0"
class ClientMediaManager : public MediaManager, public IClientMediaManager {
private:
	static ClientMediaManager* instance;
	handleptr view_handler_;
	vector<PipeMode> video_sink_pipe_mode_list_;
	vector<PipeMode> video_src_pipe_mode_list_;
	vector<PipeMode> audio_sink_pipe_mode_list_;
	vector<PipeMode> audio_src_pipe_mode_list_;
	void (*requestVideoQuality_)(int);
	ContactInfo* get_contact_info(Json::Value client_join_info);
	OperatingInfo* get_operate_info(Json::Value client_join_info);
	ClientMediaManager(int max_pipeline);
public:
	void registerRequestVideoQualityCallback(void (*requestVideoQuality)(int)) override;
	void setVideoQuality(int video_quality_index) override;
	void startCall(Json::Value client_join_info) override;
	void setViewHandler(handleptr view_handler) override;
	void endCall(Json::Value client_join_info) override;
	static ClientMediaManager* getInstance();
	static void releaseInstance();
};

