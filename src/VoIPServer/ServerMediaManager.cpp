#include "ServerMediaManager.h"
#include "../common/debug.h"

ServerMediaManager* ServerMediaManager::instance = nullptr;

ServerMediaManager::ServerMediaManager(int max_pipeline) : MediaManager(max_pipeline) {
	video_pipe_mode_list_.push_back(PipeMode(MODE_UDP_N, MODE_UDP_N));
	audio_pipe_mode_list_.push_back(PipeMode(MODE_UDP_N, MODE_UDP_N));
	sessionCallback_ = nullptr;
	gst_init(NULL, NULL);

};

ServerMediaManager* ServerMediaManager::getInstance() {
	if (instance == nullptr) {
		instance = new ServerMediaManager(1);
	}
	return instance;
}

void ServerMediaManager::releaseInstance() {
	if (instance != nullptr) {
		delete instance;
		instance = nullptr;
	}
}

void ServerMediaManager::setSessionCallback(ISessionMediaCallback* callback) {
	sessionCallback_ = callback;
	//sessionCallback_->notifyVideoQualityChanged("rid", 3);
}

void ServerMediaManager::updateClientVideoQuality(Json::Value info)
{
	// TODO : VideoQuality 변경 로직 적용 
	//string rid, string cid, int level
};


OperatingInfo* ServerMediaManager::get_operate_info(void)
{
	// TODO : SET codec & encryption
	OperatingInfo* operate_info = new OperatingInfo;
	operate_info->video_codec = "h264";
	operate_info->audio_codec = "openus";
	operate_info->encryption_alg = "";
	operate_info->encryption_key = "";
	return operate_info;
}

void ServerMediaManager::startCall(Json::Value room_creat_info)
{
#if 1
	vector<ContactInfo> contact_info_list;
	string rid = room_creat_info["rid"].asString();
	server_ip = room_creat_info["myIp"].asString();

	OperatingInfo* operate_info = get_operate_info();

	Pipelines pipelines;
	pipelines.video_pipelines.push_back(new VideoMediaPipeline(rid, video_pipe_mode_list_));
	pipelines.audio_pipelines.push_back(new AudioMediaPipeline(rid, audio_pipe_mode_list_));

	pipelineMap_.insert(make_pair(rid, pipelines));
	vector<VideoMediaPipeline*> video_pipelines = getVideoPipeLine(rid);
	for (auto pipeline : video_pipelines) {
		if (pipeline == NULL) continue;
		pipeline->makePipeline(contact_info_list, *operate_info, NULL);
	}
#endif
#if 0
	// TODO : audio 미동작
	vector<AudioMediaPipeline*> audio_pipelines = getAudioPipeLine(rid);
	for (auto pipeline : audio_pipelines) {
		if (pipeline == NULL) continue;
		pipeline->makePipeline(contact_info_list, operate_info);
	}
#endif 
}

//
//void ServerMediaManager::endCall(Json::Value room_remove_info)
//{
//	string rid = room_remove_info["rid"].asInt();
//	return MediaManager::endCall(rid);
//}


ContactInfo* ServerMediaManager::get_contact_info(Json::Value add_client_info, bool is_remove)
{
	ContactInfo* contact_info = new ContactInfo;
	contact_info->cid = add_client_info["cid"].asString();

	if (is_remove) return contact_info;
	contact_info->dest_ip = add_client_info["clientIp"].asString();
	string my_ip = server_ip;
	contact_info->dest_video_port = get_port_number(my_ip, "video");
	contact_info->dest_audio_port = get_port_number(my_ip, "audio");

	contact_info->org_video_port = get_port_number(contact_info->dest_ip, "video");
	contact_info->org_audio_port = get_port_number(contact_info->dest_ip, "audio");
	return contact_info;
}


void ServerMediaManager::addClient(Json::Value add_client_info)
{
	string rid = add_client_info["rid"].asString();

	//TODO : connect pipleline
	vector<VideoMediaPipeline*> video_pipelines = getVideoPipeLine(rid);
	ContactInfo* client_info;
	for (auto pipeline : video_pipelines) {
		if (pipeline == NULL) continue;
		client_info = get_contact_info(add_client_info, false);
		pipeline->add_client(client_info);
	}

#if 0
	// TODO : audio 미동작
	vector<AudioMediaPipeline*> audio_pipelines = getAudioPipeLine(rid);
	for (auto pipeline : audio_pipelines) {
		if (pipeline == NULL) continue;
		client_info = get_contact_info(add_client_info, false);
		pipeline->add_client(contact_info_list, operate_info);
	}
#endif
}

void ServerMediaManager::removeClient(Json::Value remove_client_info)
{
  	//TODO : connect pipleline
	string rid = remove_client_info["rid"].asString();
	ContactInfo* client_info;

	vector<VideoMediaPipeline*> video_pipelines = getVideoPipeLine(rid);
	for (auto pipeline : video_pipelines) {
		if (pipeline == NULL) continue;
		client_info = get_contact_info(remove_client_info, true);
		pipeline->remove_client(client_info);
	}

#if 0
	// TODO : audio 미동작
	vector<AudioMediaPipeline*> audio_pipelines = getAudioPipeLine(rid);
	for (auto pipeline : audio_pipelines) {
		if (pipeline == NULL) continue;
		client_info = get_contact_info(remove_client_info, true);
		pipeline->remove_client(contact_info_list, operate_info);
	}
#endif

}

Json::Value ServerMediaManager::getMediaProperty()
{
	Json::Value payload;
	payload["videoCodec"] = "codec";
	payload["audioCodec"] = "codec";
	payload["encryption_alg"] = "alg";
	payload["encryption_key"] = "key";
	return payload;
}

void ServerMediaManager::endCall(Json::Value room_remove_info)
{
	string rid = room_remove_info["rid"].asString();
	return end_call_with_rid(rid);
};
