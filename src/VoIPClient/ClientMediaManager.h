#pragma once
#include "IClientMediaManager.h"
#include "../media/MediaManager.h"
#include "../media/VideoMediaPipeline.h"
#include "../media/AudioMediaPipeline.h"
#include "session/IRequestSessionMediaCallback.h"
#include "media/vad_processor.h"

#define DEFAULT_CLIENT_CID "0"
#define DEFAULT_CLIENT_RID "0"

namespace media {

class ClientMediaManager : public MediaManager, public IClientMediaManager {
private:
	static ClientMediaManager* instance;
	handleptr view_handler_;
	vector<PipeMode> video_sink_pipe_mode_list_;
	vector<PipeMode> video_src_pipe_mode_list_;
	vector<PipeMode> audio_sink_pipe_mode_list_;
	vector<PipeMode> audio_src_pipe_mode_list_;
	ContactInfo* get_contact_info(Json::Value client_join_info);
	OperatingInfo* get_operate_info(Json::Value client_join_info);

	IRequestSessionMediaCallback* sessionCallback_;
	media::VadProcessor vad_;
	bool vad_on_;
	ClientMediaManager(int max_pipeline);

	virtual bool OnAudioBuffer(const AudioBuffer& buffer, size_t frames_per_buffer) override;
public:
	void setVideoQuality(int video_quality_index) override;
	void startCall(Json::Value client_join_info) override;
	void setViewHandler(handleptr view_handler) override;
	void endCall(Json::Value client_join_info) override;
	void setSessionCallback(IRequestSessionMediaCallback* callback);
	static ClientMediaManager* getInstance();
	static void releaseInstance();
};

} // namespace media