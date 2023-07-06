#pragma once
#include "IClientMediaManager.h"
#include "../media/MediaManager.h"
#include "../media/VideoMediaPipeline.h"
#include "../media/AudioMediaPipeline.h"
class ClientMediaManager : public MediaManager, public IClientMediaManager {
private:
	vector<PipeMode> pipe_mode_list1_;
	vector<PipeMode> pipe_mode_list2_;
	void (*notifyVideoQuality_)(int);
public:
  ClientMediaManager(int max_pipeline);
  void registerNotifyVideoQualityCallback(void (*notifyVideoQuality)(int)) override;
  void setVideoQuality(int video_quality_index) override;
  void startCall(ContactInfo& contact_info, OperatingInfo& operate_info) override;
  void endCall() override;
};

