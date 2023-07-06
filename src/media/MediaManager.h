#pragma once
#include <map>
#include <vector>
#include "VideoMediaPipeline.h"
#include "AudioMediaPipeline.h"
#include "../json/json.h"

struct Pipelines {
	vector<VideoMediaPipeline*> video_pipelines;
	vector<AudioMediaPipeline*> audio_pipelines;
};

class MediaManager
{
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
};

