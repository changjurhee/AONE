#pragma once
#include <map>
#include <vector>
#include "VideoMediaPipeline.h"
#include "AudioMediaPipeline.h"
#include "../json/json.h"
#include "pipeline_monitorable.h"
#include "media_types.h"

namespace media {

struct Pipelines {
	vector<VideoMediaPipeline*> video_pipelines;
	vector<AudioMediaPipeline*> audio_pipelines;
};

class MediaManager : public PipelineMonitorable::Callback
{
	VideoPresetLevel ShouldChangeVideoQuality(const VideoPresetType& current_preset,
		const PipelineMonitorable::RtpStats& stats);

protected:
	struct VqaData {
		VqaData() :
			next_check_time(0), num_clients(0), need_restart(true), cur_level(VideoPresetLevel::kVideoPreset5) {}
		clock_t next_check_time; // If a changing happens then we'll try to check again 3 seconds later.
		int num_clients;
		bool need_restart;
		VideoPresetLevel cur_level;
		std::map<std::string, uint64_t> lost_nums; // store last seen num_lost every 1 sec.
		std::map<std::string, uint64_t> lost_nums_per_sec;
	};
	int max_pipleline_;
	map<string, Pipelines> pipelineMap_;
	std::map<std::string, VqaData> vqa_data_per_room_;
	vector<VideoMediaPipeline*> getVideoPipeLine(string rid);
	vector<AudioMediaPipeline*> getAudioPipeLine(string rid);
	int get_port_number(string dest_ip, string type);
	bool checkValidRID(string rid);
	string get_ip_address(void);
	vector<string> split(const string& str, char delim);
	void end_call_with_rid(string rid);
public:
	MediaManager(int max_pipeline);
	void SetRtpJitterBufferLatency(unsigned int latency);

private:
	// media::PipelineMonitorable::Callback implementation.
	virtual void OnRtpStats(const VideoPresetType& current_preset, const PipelineMonitorable::RtpStats& stats) override ;
	virtual bool OnAudioBuffer(const AudioBuffer& buffer, size_t frames_per_buffer) override ;

	virtual bool notifyVideoQualityChangeNeeded(std::string rid, VideoPresetLevel level) { return true; };
};

} // namespace media