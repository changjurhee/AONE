#include "MediaManager.h"
//#include <winsock2.h>

#include "common/logger.h"
#include "media_config.h"

namespace media {

VideoPresetLevel MediaManager::ShouldChangeVideoQuality(const VideoPresetType& current_preset,
	const PipelineMonitorable::RtpStats& stats) {
	// do not use current_preset.
	VideoPresetLevel cur_level = vqa_data_per_room_[stats.rid].cur_level; //current_preset.level;
	VideoPresetLevel ret = cur_level;

	if (!vqa_data_per_room_[stats.rid].num_clients)
		return cur_level;

	LOG_OBJ_LOG() << "[rid:" << stats.rid << "] cur_bitrate " << current_preset.bitrate << ", num_lost " << stats.num_lost
		<< ", num_late " << stats.num_late << ", avg_jitter " << stats.avg_jitter_us << " us" << std::endl;

	// Check if it's reached firstly
	if (vqa_data_per_room_[stats.rid].next_check_time == 0) {
		vqa_data_per_room_[stats.rid].next_check_time = clock() + CLOCKS_PER_SEC; // 1 sec later
		LOG_OBJ_INFO() << "[rid:" << stats.rid << "] ================================FIRST TIME!!!!!!!!!!!!!!!!!!!" << std::endl;
		return cur_level;
	}

	// initialize if 0
	if (vqa_data_per_room_[stats.rid].lost_nums[stats.buffer_name] == 0) {
		vqa_data_per_room_[stats.rid].lost_nums[stats.buffer_name] = stats.num_lost;
		//LOG_OBJ_INFO() << "[rid:" << stats.rid << "] INIT " << stats.buffer_name << " " << vqa_data_per_room_[stats.rid].lost_nums[stats.buffer_name] << std::endl;
		return cur_level;
	}

	// Update lost_nums_per_sec until next check time.
	vqa_data_per_room_[stats.rid].lost_nums_per_sec[stats.buffer_name] =
		stats.num_lost - vqa_data_per_room_[stats.rid].lost_nums[stats.buffer_name];

	// If passed the next checking time Do checking
	if (clock() > vqa_data_per_room_[stats.rid].next_check_time) {
		LOG_OBJ_INFO() << "[rid:" << stats.rid << "] CHECKTIME " << stats.buffer_name << " " << vqa_data_per_room_[stats.rid].lost_nums[stats.buffer_name] << std::endl;
		// Reset values if we've done changing quality and reached here
		if (vqa_data_per_room_[stats.rid].need_restart) {
			LOG_INFO("Re-initialize");

			vqa_data_per_room_[stats.rid].need_restart = false;
			vqa_data_per_room_[stats.rid].next_check_time = clock() + CLOCKS_PER_SEC; // 1 sec later
			// reset all lost_num so that it will re-initialized new num_lost
			for_each(vqa_data_per_room_[stats.rid].lost_nums.begin(), vqa_data_per_room_[stats.rid].lost_nums.end(),
				[](auto& elem) { elem.second = 0; });

			LOG_OBJ_INFO() << "[rid:" << stats.rid << "] NEEDRESTART " << stats.buffer_name << " " << vqa_data_per_room_[stats.rid].lost_nums[stats.buffer_name] << std::endl;
			return cur_level;
		}

		// Do checking
		uint64_t largest_num_lost_per_sec =
			max_element(vqa_data_per_room_[stats.rid].lost_nums_per_sec.begin(),
				vqa_data_per_room_[stats.rid].lost_nums_per_sec.end(),
				[](const auto& a, const auto& b) { return a.second < b.second; })->second;

		LOG_OBJ_INFO() << "[rid:" << stats.rid << "] cur_bitrate " << current_preset.bitrate
			<< ", num_lost " << stats.num_lost << ", num_late " << stats.num_late
			<< ", avg_jitter " << stats.avg_jitter_us << " us"
			<< ", num_lost_per_last_1sec " << largest_num_lost_per_sec << std::endl;

		if ((largest_num_lost_per_sec > MediaConfig::ThresholdOfNumLostForChangingVideoQuality())) {

			// Check need to lower
			while (vqa_data_per_room_[stats.rid].lost_status.size() > MediaConfig::GetNumOfContinousLost()-1) {
				vqa_data_per_room_[stats.rid].lost_status.pop();
			}
			vqa_data_per_room_[stats.rid].lost_status.push(true);
			bool need_to_lower = true;
			std::queue<bool> lost_checkq = vqa_data_per_room_[stats.rid].lost_status;
			while (!lost_checkq.empty()) {
				if (!lost_checkq.front()) need_to_lower = false;
				lost_checkq.pop();
			};

			LOG_OBJ_WARN() << "[rid:" << stats.rid << "] num_lost_per_sec("
				<< vqa_data_per_room_[stats.rid].lost_nums_per_sec[stats.buffer_name]
				<< ") is bigger than threshold(" << MediaConfig::ThresholdOfNumLostForChangingVideoQuality()
				<< "). right now need_to_lower " << need_to_lower << std::endl;

			vqa_data_per_room_[stats.rid].next_check_time = clock() + CLOCKS_PER_SEC; // 1 secs later
			if (need_to_lower && (cur_level > VideoPresetLevel::kVideoPresetOff)) {
				ret = VideoPresetType::Lower(cur_level);
				// Reset
				while (!vqa_data_per_room_[stats.rid].lost_status.empty()) {
					vqa_data_per_room_[stats.rid].lost_status.pop();
				};

				vqa_data_per_room_[stats.rid].next_check_time = clock() + (CLOCKS_PER_SEC * 3); // 3 secs later
				vqa_data_per_room_[stats.rid].need_restart = true;
			}
		}
		else {
			// Check need to upper
			while (vqa_data_per_room_[stats.rid].lost_status.size() > MediaConfig::GetNumOfContinousLost() - 1) {
				vqa_data_per_room_[stats.rid].lost_status.pop();
			}
			vqa_data_per_room_[stats.rid].lost_status.push(false);
			bool need_to_upper = true;
			std::queue<bool> lost_checkq = vqa_data_per_room_[stats.rid].lost_status;
			while (!lost_checkq.empty()) {
				LOG_OBJ_WARN() << "[rid:" << stats.rid << "] upper check " << lost_checkq.front() << std::endl;
				if (lost_checkq.front()) need_to_upper = false;
				lost_checkq.pop();
			};

			LOG_OBJ_WARN() << "[rid:" << stats.rid << "] num_lost_per_sec("
				<< vqa_data_per_room_[stats.rid].lost_nums_per_sec[stats.buffer_name]
				<< ") is less than threshold(" << MediaConfig::ThresholdOfNumLostForChangingVideoQuality()
				<< "). right now need_to_upper " << need_to_upper << std::endl;

			vqa_data_per_room_[stats.rid].next_check_time = clock() + CLOCKS_PER_SEC; // 1 secs later
			if (need_to_upper && (cur_level < VideoPresetLevel::kVideoPresetMax)) {
				ret = VideoPresetType::Upper(cur_level);
				// Reset
				while (!vqa_data_per_room_[stats.rid].lost_status.empty()) {
					vqa_data_per_room_[stats.rid].lost_status.pop();
				};
				vqa_data_per_room_[stats.rid].next_check_time = clock() + (CLOCKS_PER_SEC * 3); // 3 secs later
				vqa_data_per_room_[stats.rid].need_restart = true;
			}
		}

		for_each(vqa_data_per_room_[stats.rid].lost_nums.begin(), vqa_data_per_room_[stats.rid].lost_nums.end(),
			[](auto& elem) { elem.second = 0; });
	}

	return ret;

}

MediaManager::MediaManager(int max_pipeline) {
	max_pipleline_ = max_pipeline;
	// TODO: codec/encryption 관련 등록 기능 추가
}

void MediaManager::SetRtpJitterBufferLatency(unsigned int latency) {
	for (auto room : pipelineMap_) {
		auto av_pipelines = room.second;
		for (auto pipeline : av_pipelines.video_pipelines) {
			pipeline->set_rtp_jitter_buffer_latency(latency);
		}
		for (auto pipeline : av_pipelines.audio_pipelines) {
			pipeline->set_rtp_jitter_buffer_latency(latency);
		}
	}
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
	string ip_part = "000";

	vector<string> nums = split(dest_ip, '.');
	if (nums.size() == 4) {
		ip_part = std::to_string(stoi(nums[2])%10);
		if (nums[3].size() == 1) {
			ip_part += "0" + nums[3];
		} else {
			ip_part += std::to_string(stoi(nums[3]) % 100);
		}
	}

	string type_number;
	if (type == "video") type_number = "1";
	else if(type == "audio") type_number = "2";
	else  type_number = "0";

	string port_num = "1" + ip_part + type_number;
	LOG_OBJ_INFO() << "IP : " << dest_ip << " port : " << port_num << endl;
	return stoi(port_num);
}

bool MediaManager::checkValidRID(string rid) {
	if (pipelineMap_.find(rid) == pipelineMap_.end()) {
		return FALSE;
	}
	return TRUE;
}

vector<VideoMediaPipeline*> MediaManager::getVideoPipeLine(string rid) {
	vector<VideoMediaPipeline*> null_vector;
	if (!checkValidRID(rid)) {
		LOG_OBJ_INFO() << " Invalid RID (" << rid << ")" << endl;
		return null_vector;
	}
	return pipelineMap_[rid].video_pipelines;
}

vector<AudioMediaPipeline*> MediaManager::getAudioPipeLine(string rid) {
	vector<AudioMediaPipeline*> null_vector;
	if (!checkValidRID(rid)) {
		LOG_OBJ_INFO() << " Invalid RID (" << rid << ")" << endl;
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
	for (auto pipeline : audio_piplelines) {
		if (pipeline == NULL) continue;
		pipeline->end_call();
	}
	pipelineMap_.erase(rid);
	vqa_data_per_room_.erase(rid);
}

void MediaManager::OnRtpStats(const VideoPresetType& current_preset, const PipelineMonitorable::RtpStats& stats) {
	LOG_LOG("IN");

	VideoPresetLevel next_preset_level = ShouldChangeVideoQuality(current_preset, stats);
	//if (next_preset_level != current_preset.level) {
	// Do not use the level of current_preset of pipeline because we decided to keep server's video quality due to no time for testing
	if (next_preset_level != vqa_data_per_room_[stats.rid].cur_level) {
		LOG_OBJ_WARN() << "Request to change video quality! current " << static_cast<int>(vqa_data_per_room_[stats.rid].cur_level)
			<< " to " << static_cast<int>(next_preset_level) << std::endl;

		vqa_data_per_room_[stats.rid].cur_level = next_preset_level;
		notifyVideoQualityChangeNeeded(stats.rid, next_preset_level);
	}
}

bool MediaManager::OnAudioBuffer(const AudioBuffer& buffer, size_t frames_per_buffer) {
	return true;
}

} // namespace media