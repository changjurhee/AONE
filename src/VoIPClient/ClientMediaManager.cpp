#include "ClientMediaManager.h"
#include "common/logger.h"

namespace media {

ClientMediaManager* ClientMediaManager::instance = nullptr;

ClientMediaManager::ClientMediaManager(int max_pipeline) : MediaManager(max_pipeline) {
	video_sink_pipe_mode_list_.push_back(PipeMode(MODE_DEVICE, MODE_UDP_1));
	video_src_pipe_mode_list_.push_back(PipeMode(MODE_UDP_1, MODE_DEVICE));
	audio_sink_pipe_mode_list_.push_back(PipeMode(MODE_DEVICE, MODE_UDP_1));
	audio_src_pipe_mode_list_.push_back(PipeMode(MODE_UDP_1, MODE_DEVICE));

	sessionCallback_ = nullptr;
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

void ClientMediaManager::setSessionCallback(ISessionMediaCallback* callback) {
	sessionCallback_ = callback;
	// sessionCallback_->requestVideoQualityChange(3);
}

void ClientMediaManager::setVideoQuality(int video_quality_index)
{
	LOG_OBJ_INFO() << "video_quality_index : " << video_quality_index << endl;
	vector<VideoMediaPipeline*> pipelines = getVideoPipeLine(DEFAULT_CLIENT_RID);
	struct VideoQualityInfo vq_info;
	vq_info.enable_recover_timer = false;
	vq_info.video_quality_index = video_quality_index;
	for (auto pipeline : pipelines) {
		if (pipeline == NULL) continue;
		pipeline->requestSetVideoQuality(&vq_info);
	}
};

ContactInfo* ClientMediaManager::get_contact_info(Json::Value client_join_info)
{
	ContactInfo *contact_info = new ContactInfo;
	contact_info->dest_ip = client_join_info["serverIp"].asString();
	contact_info->cid = DEFAULT_CLIENT_CID;

	string my_ip = client_join_info["myIp"].asString();
	contact_info->dest_video_port = get_port_number(my_ip, "video");
	contact_info->dest_audio_port = get_port_number(my_ip, "audio");

	contact_info->org_video_port = get_port_number(contact_info->dest_ip, "video");
	contact_info->org_audio_port = get_port_number(contact_info->dest_ip, "audio");
	return contact_info;
}

OperatingInfo* ClientMediaManager::get_operate_info(Json::Value client_join_info)
{
	OperatingInfo* operate_info = new OperatingInfo;
	operate_info->video_codec = client_join_info["videoCodec"].asString();
	operate_info->audio_codec = client_join_info["audioCodec"].asString();
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
	LOG_INFO("In");

	Pipelines pipelines;
	pipelines.video_pipelines.push_back(new VideoMediaPipeline(rid, video_sink_pipe_mode_list_, nullptr));
	pipelines.video_pipelines.push_back(new VideoMediaPipeline(rid, video_src_pipe_mode_list_, this));
	pipelines.audio_pipelines.push_back(new AudioMediaPipeline(rid, audio_sink_pipe_mode_list_, this));
	pipelines.audio_pipelines.push_back(new AudioMediaPipeline(rid, audio_src_pipe_mode_list_, nullptr));

	pipelineMap_[rid] = pipelines;
	vector<VideoMediaPipeline*> video_pipelines = getVideoPipeLine(rid);
	for (auto pipeline : video_pipelines) {
		if (pipeline == NULL) continue;
		pipeline->request_add_client(&contact_info_list[0]);
		pipeline->makePipeline(contact_info_list, *operate_info, view_handler_);
	}

	vector<AudioMediaPipeline*> audio_pipelines = getAudioPipeLine(rid);
	for (auto pipeline : audio_pipelines) {
		if (pipeline == NULL) continue;
		pipeline->request_add_client(&contact_info_list[0]);
		pipeline->makePipeline(contact_info_list, *operate_info, NULL);
	}
}

void ClientMediaManager::setViewHandler(handleptr view_handler)
{
	view_handler_ = view_handler;
}

void ClientMediaManager::endCall(Json::Value client_join_info) {
	LOG_INFO("In");
	return MediaManager::end_call_with_rid(DEFAULT_CLIENT_RID);
}

} // namespace media