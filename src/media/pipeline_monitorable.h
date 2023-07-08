#ifndef MEDIA_PIPELINE_MONITORABLE_H_
#define MEDIA_PIPELINE_MONITORABLE_H_

#include <cstdint>

#include "media_types.h"
#include "audio_buffer.h"

namespace media {

class PipelineMonitorable {
public:
	struct RtpStats {
		uint64_t num_lost;
		uint64_t num_late;
		uint64_t avg_jitter_us;

		RtpStats():
			num_lost(0),
			num_late(0),
			avg_jitter_us(0)
		{}
	};

	class Callback {
	public:
		Callback() {}

		virtual void OnRtpStats(const VideoPresetType& current_preset, const PipelineMonitorable::RtpStats& stats) = 0;
		virtual void OnAudioBuffer(const AudioBuffer& buffer) = 0;

	protected:
		virtual ~Callback() {}
	};

	PipelineMonitorable() = delete;
	PipelineMonitorable(Callback* monitor_cb):
		monitor_cb_(monitor_cb)
	{}

protected:
	Callback* monitor_cb_;

	virtual ~PipelineMonitorable() {}
};

}

#endif