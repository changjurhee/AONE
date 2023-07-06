#pragma once
#include <map>
#include <vector>
#include "VideoMediaPipeline.h"
#include "AudioMediaPipeline.h"
typedef pair<VideoMediaPipeline*, AudioMediaPipeline*> Pipelines;

class MediaManager
{
protected:
	int max_pipleline_;
	map<int, Pipelines> pipelineMap_;
	VideoMediaPipeline* getVideoPipeLine(int call_index);
	AudioMediaPipeline* getAudioPipeLine(int call_index);
public:
	MediaManager(int max_pipeline);
	void endCall(int call_index);
};

