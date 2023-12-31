#include "ServerMediaManager.h"
#include "common/logger.h"

namespace media {

ServerMediaManager* ServerMediaManager::instance = nullptr;
int ServerMediaManager::dummy_port = 10000;

ServerMediaManager::ServerMediaManager(int max_pipeline) : MediaManager(max_pipeline) {
	video_pipe_mode_list_.push_back(PipeMode(MODE_UDP_N, MODE_UDP_N));
	audio_pipe_mode_list_.push_back(PipeMode(MODE_UDP_REMOVE_ME, MODE_NONE));
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
}

void ServerMediaManager::updateClientVideoQuality(Json::Value info)
{
	int level = std::stoi(info["level"].asString());
	LOG_OBJ_INFO() << level << std::endl;

	if (pipelineMap_.begin() == pipelineMap_.end()) {
		LOG_WARN("Currently no running room! just return");
		return;
	}
	if (!sessionCallback_) {
		LOG_WARN("Currently no sessionCallback_ just return");
		return;
	}

	for_each(pipelineMap_.begin(), pipelineMap_.end(), [this, level](auto elem) {
		LOG_OBJ_INFO() << "notifyVideoQualityChanged rid " << elem.first << ", level " << level << std::endl;
		sessionCallback_->notifyVideoQualityChanged(elem.first, level);
		});
};

void ServerMediaManager::updateRtpJitterBufferLatency(Json::Value info) {
	int latency = std::stoi(info["latency"].asString());
	LOG_OBJ_INFO() << latency << std::endl;

	if (pipelineMap_.begin() == pipelineMap_.end()) {
		LOG_WARN("Currently no running room! just return");
		return;
	}

	auto set_latency = [latency](MediaPipeline* pipeline) {
		pipeline->set_rtp_jitter_buffer_latency(latency);
	};

	for_each(pipelineMap_.begin()->second.audio_pipelines.begin(),
		pipelineMap_.begin()->second.audio_pipelines.end(), set_latency);
	for_each(pipelineMap_.begin()->second.video_pipelines.begin(),
		pipelineMap_.begin()->second.video_pipelines.end(), set_latency);
}


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

ContactInfo* ServerMediaManager::get_dummy_contact(int number)
{
	ContactInfo* contact_info = new ContactInfo;
	contact_info->cid = "__dummy_"+std::to_string(number);
	string my_ip = server_ip;
	contact_info->my_ip = my_ip;
	contact_info->dest_ip = "0.0.0.0";
	contact_info->client_name = "__dummy_" + std::to_string(number);
	contact_info->dest_video_port = dummy_port++;
	contact_info->dest_audio_port = dummy_port++;

	contact_info->org_video_port = dummy_port++;
	contact_info->org_audio_port = dummy_port++;
	return contact_info;
}

bool ServerMediaManager::notifyVideoQualityChangeNeeded(std::string rid, VideoPresetLevel level) {
	if (!sessionCallback_)
		return false;

	sessionCallback_->notifyVideoQualityChanged(rid, (int)level);
	return true;
}

void ServerMediaManager::startCall(Json::Value room_creat_info)
{

	vector<ContactInfo> contact_info_list;
	string rid = room_creat_info["rid"].asString();
	server_ip = room_creat_info["myIp"].asString();
	int conferenceSize = room_creat_info["conferenceSize"].asInt();

	OperatingInfo* operate_info = get_operate_info();
	LOG_OBJ_INFO() << "rid : " << rid << endl;
	for (int i = 0; i < conferenceSize; i++)
		contact_info_list.push_back(*get_dummy_contact(i));

	Pipelines pipelines;
	pipelines.video_pipelines.push_back(new VideoMediaPipeline(rid, video_pipe_mode_list_, this, nullptr));
	pipelines.audio_pipelines.push_back(new AudioMediaPipeline(rid, audio_pipe_mode_list_, nullptr, nullptr));

	pipelineMap_.insert(make_pair(rid, pipelines));
	vector<VideoMediaPipeline*> video_pipelines = getVideoPipeLine(rid);
	for (auto pipeline : video_pipelines) {
		if (pipeline == NULL) continue;
		pipeline->makePipeline(contact_info_list, *operate_info, NULL);
	}

	vector<AudioMediaPipeline*> audio_pipelines = getAudioPipeLine(rid);
	for (auto pipeline : audio_pipelines) {
		if (pipeline == NULL) continue;
		pipeline->makePipeline(contact_info_list, *operate_info, NULL);
	}
}

ContactInfo* ServerMediaManager::get_contact_info(Json::Value add_client_info, bool is_remove)
{
	ContactInfo* contact_info = new ContactInfo;
	contact_info->cid = add_client_info["cid"].asString();

	contact_info->dummy_client = false;
	if (is_remove) return contact_info;
	string my_ip = server_ip;
	contact_info->my_ip = my_ip;
	contact_info->dest_ip = split(add_client_info["clientIp"].asString(), ':')[0];
	contact_info->client_name = add_client_info["name"].asString();;
	contact_info->dest_video_port = get_port_number(my_ip, "video");
	contact_info->dest_audio_port = get_port_number(my_ip, "audio");

	contact_info->org_video_port = get_port_number(contact_info->dest_ip, "video");
	contact_info->org_audio_port = get_port_number(contact_info->dest_ip, "audio");

	return contact_info;
}


void ServerMediaManager::addClient(Json::Value add_client_info)
{
	string rid = add_client_info["rid"].asString();
	if (!checkValidRID(rid)) {
		LOG_OBJ_INFO() << " Invalid RID (" << rid << ")" << endl;
		return;
	}

	vqa_data_per_room_[rid].num_clients++;

	vector<VideoMediaPipeline*> video_pipelines = getVideoPipeLine(rid);
	ContactInfo* client_info = get_contact_info(add_client_info, false);;
	for (auto pipeline : video_pipelines) {
		if (pipeline == NULL) continue;
		pipeline->request_add_client(client_info);
	}

	vector<AudioMediaPipeline*> audio_pipelines = getAudioPipeLine(rid);
	for (auto pipeline : audio_pipelines) {
		if (pipeline == NULL) continue;
		pipeline->request_add_client(client_info);
	}
}

void ServerMediaManager::removeClient(Json::Value remove_client_info)
{
	string rid = remove_client_info["rid"].asString();
	if (!checkValidRID(rid)) {
		LOG_OBJ_INFO() << " Invalid RID (" << rid << ")" << endl;
		return;
	}

	vqa_data_per_room_[rid].num_clients--;

	vector<VideoMediaPipeline*> video_pipelines = getVideoPipeLine(rid);
	ContactInfo* client_info = get_contact_info(remove_client_info, true);
	LOG_OBJ_INFO() << "rid : " << rid << ", cid: " << client_info->cid << endl;
	for (auto pipeline : video_pipelines) {
		if (pipeline == NULL) continue;
		pipeline->request_remove_client(client_info);
	}

	vector<AudioMediaPipeline*> audio_pipelines = getAudioPipeLine(rid);
	for (auto pipeline : audio_pipelines) {
		if (pipeline == NULL) continue;
		pipeline->request_remove_client(client_info);
	}
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
	LOG_OBJ_INFO() << "rid : " << rid << endl;
	return end_call_with_rid(rid);
};

} // namespace media