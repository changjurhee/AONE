#include "ClientMediaManager.h"
#include "../common/debug.h"

ClientMediaManager::ClientMediaManager(int max_pipeline) : MediaManager(max_pipeline) {
	pipe_mode_list1_.push_back(PipeMode(MODE_DEVICE, MODE_UDP_1));
	pipe_mode_list2_.push_back(PipeMode(MODE_UDP_1, MODE_DEVICE));
	gst_init(NULL, NULL);
};

void ClientMediaManager::registerNotifyVideoQualityCallback(void (*notifyVideoQuality_)(int)) {
	notifyVideoQuality_ = notifyVideoQuality_;
};

void ClientMediaManager::setVideoQuality(int video_quality_index) {
	VideoMediaPipeline *pipleline = getVideoPipeLine(0);
	if (pipleline == NULL) {
		debug_log("유효하지 않는 call입니다.");
		return;
	}
	pipleline->setVideoQuality(video_quality_index);
};

void ClientMediaManager::startCall(ContactInfo& contact_info, OperatingInfo& operate_info) {
	int call_index = 0;
	vector<ContactInfo> contact_info_list; 
	contact_info_list.push_back(contact_info);
	
	VideoMediaPipeline *video_pipleline1 = new VideoMediaPipeline(call_index, pipe_mode_list1_);
	VideoMediaPipeline* video_pipleline2 = new VideoMediaPipeline(call_index, pipe_mode_list2_);
	AudioMediaPipeline *audio_pipleline = new AudioMediaPipeline(call_index, pipe_mode_list1_);
	
	pipelineMap_.insert(pair<int, Pipelines>(call_index, Pipelines(video_pipleline1, audio_pipleline)));
	video_pipleline1->makePipeline(contact_info_list, operate_info);
	video_pipleline2->makePipeline(contact_info_list, operate_info);
	//video_pipleline->pipeline_run();
	//video_pipleline->add_client(contact_info);

	//audio_pipleline->makePipeline(contact_info_list, operate_info);
	//audio_pipleline->add_client(contact_info);
}

void ClientMediaManager::endCall(void) {
	return MediaManager::endCall(0);
}
