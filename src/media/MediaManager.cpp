#include "MediaManager.h"

MediaManager::MediaManager(int max_pipeline) {
	max_pipleline_ = max_pipeline;
	// TODO: codec/encryption 관련 등록 기능 추가
}

VideoMediaPipeline* MediaManager::getVideoPipeLine(int call_index) {
	if (pipelineMap_.find(call_index) == pipelineMap_.end()) {
		debug_log("유효하지 않는 call입니다.");
		return NULL;
	}	
	return pipelineMap_[call_index].first;
}

AudioMediaPipeline* MediaManager::getAudioPipeLine(int call_index) {
	if (pipelineMap_.find(call_index) == pipelineMap_.end()) {
		debug_log("유효하지 않는 call입니다.");
		return NULL;
	}	
	return pipelineMap_[call_index].second;
}

void MediaManager::endCall(int call_index) {
	VideoMediaPipeline *video_pipleline = getVideoPipeLine(call_index);
	if (video_pipleline == NULL) {
		debug_log("유효하지 않는 call입니다.");
		return;
	}
	video_pipleline->end_call();

	AudioMediaPipeline * audio_pipleline = getAudioPipeLine(call_index);
	if (audio_pipleline == NULL) {
		debug_log("유효하지 않는 call입니다.");
		return;
	}
	audio_pipleline->end_call();
	pipelineMap_.erase(call_index);
}
