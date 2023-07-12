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
			num_lost_last(0), begin_time(0), elapsed_time(0), num_clients(0), cur_level(VideoPresetLevel::kVideoPreset5) {}
		uint64_t num_lost_last; // store last seen num_lost every 1 sec.
		clock_t begin_time;
		clock_t elapsed_time;
		int num_clients;
		VideoPresetLevel cur_level;
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