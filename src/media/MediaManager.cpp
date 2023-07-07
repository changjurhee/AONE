#include "MediaManager.h"
//#include <winsock2.h> 

MediaManager::MediaManager(int max_pipeline) {
	max_pipleline_ = max_pipeline;
	// TODO: codec/encryption 관련 등록 기능 추가
}

string MediaManager::get_ip_address(void) {
	return "127.0.0.1";
#if 0
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		return "";
	}

	char hostName[256];
	if (gethostname(hostName, sizeof(hostName)) == 0) {
		struct hostent* host = gethostbyname(hostName);
		if (host != nullptr) {
			struct in_addr addr;
			memcpy(&addr, host->h_addr_list[0], sizeof(struct in_addr));
			return inet_ntoa(addr);
		}
	}
	return "";
#endif
}

vector<string> MediaManager::split(const string& str, char delim)
{
	auto i = 0;
	vector<string> list;

	auto pos = str.find(delim);
	while (pos != string::npos)
	{
		list.push_back(str.substr(i, pos - i));
		i = ++pos;
		pos = str.find(delim, pos);
	}

	list.push_back(str.substr(i, str.length()));
	return list;
}

int MediaManager::get_port_number(string dest_ip, string type) {
	string port_num = "000";

	vector<string> nums = split(dest_ip, '.');
	if (nums.size() == 4) {
		port_num = std::to_string(stoi(nums[2])%10);
		if (nums[3].size() == 1) {
			port_num += "0" + nums[3];
		} else {
			port_num += std::to_string(stoi(nums[3]) % 100);
		}
	}

	string type_number;
	if (type == "video") type_number = "1";
	else if(type == "audio") type_number = "2";
	else  type_number = "0";

	debug_log("IP : " + dest_ip + " port : " + "1" + port_num + type_number);
	return stoi("1"+ port_num+ type_number);
}

vector<VideoMediaPipeline*> MediaManager::getVideoPipeLine(string rid) {
	vector<VideoMediaPipeline*> null_vector;
	if (pipelineMap_.find(rid) == pipelineMap_.end()) {
		debug_log("유효하지 않는 call입니다.");
		return null_vector;
	}
	return pipelineMap_[rid].video_pipelines;
}

vector<AudioMediaPipeline*> MediaManager::getAudioPipeLine(string rid) {
	vector<AudioMediaPipeline*> null_vector;
	if (pipelineMap_.find(rid) == pipelineMap_.end()) {
		debug_log("유효하지 않는 call입니다.");
		return null_vector;
	}	
	return pipelineMap_[rid].audio_pipelines;
}

void MediaManager::end_call_with_rid(string rid)
{
	vector<VideoMediaPipeline*> video_piplelines = getVideoPipeLine(rid);
	for (auto pipeline : video_piplelines) {
		if (pipeline == NULL) continue;
		pipeline->end_call();
	}

	vector<AudioMediaPipeline*> audio_piplelines = getAudioPipeLine(rid);
	for (auto pipeline : video_piplelines) {
		if (pipeline == NULL) continue;
		pipeline->end_call();
	}
	pipelineMap_.erase(rid);
}
