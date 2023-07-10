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
	static VideoPresetType ShouldChangeVideoQuality(const VideoPresetType& current_preset,
		const PipelineMonitorable::RtpStats& stats);

protected:
	int max_pipleline_;
	map<string, Pipelines> pipelineMap_;
	vector<VideoMediaPipeline*> getVideoPipeLine(string rid);
	vector<AudioMediaPipeline*> getAudioPipeLine(string rid);
	int get_port_number(string dest_ip, string type);
	string get_ip_address(void);
	vector<string> split(const string& str, char delim);
	void end_call_with_rid(string rid);
public:
	MediaManager(int max_pipeline);

private:
	// media::PipelineMonitorable::Callback implementation.
	void OnRtpStats(const VideoPresetType& current_preset, const PipelineMonitorable::RtpStats& stats);
	void OnAudioBuffer(const AudioBuffer& buffer);
};

} // namespace media