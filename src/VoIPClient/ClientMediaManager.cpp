#include "ClientMediaManager.h"
#include "../common/debug.h"

ClientMediaManager* ClientMediaManager::instance = nullptr;

ClientMediaManager::ClientMediaManager(int max_pipeline) : MediaManager(max_pipeline) {
	video_sink_pipe_mode_list_.push_back(PipeMode(MODE_DEVICE, MODE_UDP_1));
	video_src_pipe_mode_list_.push_back(PipeMode(MODE_UDP_1, MODE_DEVICE));
	audio_sink_pipe_mode_list_.push_back(PipeMode(MODE_DEVICE, MODE_UDP_1));
	audio_src_pipe_mode_list_.push_back(PipeMode(MODE_UDP_1, MODE_DEVICE));

	gst_init(NULL, NULL);
};

ClientMediaManager* ClientMediaManager::getInstance() {
	if (instance == nullptr) {
		instance = new ClientMediaManager(1);
	}
	return instance;
}

void ClientMediaManager::releaseInstance() {
	if (instance != nullptr) {
		delete instance;
		instance = nullptr;
	}
}

void ClientMediaManager::registerNotifyVideoQualityCallback(void (*notifyVideoQuality_)(int)) {
	notifyVideoQuality_ = notifyVideoQuality_;
};

void ClientMediaManager::setVideoQuality(int video_quality_index)
{
	vector<VideoMediaPipeline*> pipelines = getVideoPipeLine(DEFAULT_CLIENT_CID);
	for (auto pipeline : pipelines) {
		if (pipeline == NULL) continue;
		pipeline->setVideoQuality(video_quality_index);
	}	
};

ContactInfo* ClientMediaManager::get_contact_info(Json::Value client_join_info)
{
	ContactInfo *contact_info = new ContactInfo;
	contact_info->dest_ip = client_join_info["serverip"].asString();
//	string my_ip = get_ip_address();
	// TODO : json 문구 확인하기
	string my_ip = client_join_info["myip"].asString();
	contact_info->dest_video_port = get_port_number(my_ip, "video");
	contact_info->dest_audio_port = get_port_number(my_ip, "audio");

	contact_info->org_video_port = get_port_number(contact_info->dest_ip, "video");
	contact_info->org_audio_port = get_port_number(contact_info->dest_ip, "audio");
	return contact_info;
}

OperatingInfo* ClientMediaManager::get_operate_info(Json::Value client_join_info)
{
	OperatingInfo* operate_info = new OperatingInfo;
	operate_info->video_codec = client_join_info["videocodec"].asString();
	operate_info->audio_codec = client_join_info["audiocodec"].asString();
	operate_info->encryption_alg = client_join_info["encryption_alg"].asString();
	operate_info->encryption_key = client_join_info["encryption_key"].asString();
	return operate_info;
}

void ClientMediaManager::startCall(Json::Value client_join_info)
{
	string rid = DEFAULT_CLIENT_RID;
	vector<ContactInfo> contact_info_list; 
	ContactInfo* contact_info = get_contact_info(client_join_info);
	OperatingInfo* operate_info = get_operate_info(client_join_info);

	contact_info_list.push_back(*contact_info);

	Pipelines pipelines;
	pipelines.video_pipelines.push_back(new VideoMediaPipeline(rid, video_sink_pipe_mode_list_));
	pipelines.video_pipelines.push_back(new VideoMediaPipeline(rid, video_src_pipe_mode_list_));
	pipelines.audio_pipelines.push_back(new AudioMediaPipeline(rid, audio_sink_pipe_mode_list_));
	pipelines.audio_pipelines.push_back(new AudioMediaPipeline(rid, audio_src_pipe_mode_list_));

	pipelineMap_.insert(make_pair(rid, pipelines));

	vector<VideoMediaPipeline*> video_pipelines = getVideoPipeLine(rid);
	for (auto pipeline : video_pipelines) {
		if (pipeline == NULL) continue;
		pipeline->add_client(&contact_info_list[0]);
		pipeline->makePipeline(contact_info_list, *operate_info);
	}

#if 0
	// TODO : audio 미동작
	vector<AudioMediaPipeline*> audio_pipelines = getAudioPipeLine(rid);
	for (auto pipeline : audio_pipelines) {
		if (pipeline == NULL) continue;
		pipeline->makePipeline(contact_info_list, operate_info);
	}
#endif 
}

void ClientMediaManager::endCall(Json::Value client_join_info) {
	return MediaManager::end_call_with_rid(DEFAULT_CLIENT_RID);
}
