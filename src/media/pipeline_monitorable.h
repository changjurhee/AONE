#ifndef MEDIA_PIPELINE_MONITORABLE_H_
#define MEDIA_PIPELINE_MONITORABLE_H_

#include <cstdint>

#include "media_types.h"
#include "audio_buffer.h"

namespace media {

class PipelineMonitorable {
public:
	struct RtpStats {
		std::string rid;
		std::string buffer_name;
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
		virtual bool OnAudioBuffer(const AudioBuffer& buffer, size_t frames_per_buffer) = 0;

	protected:
		virtual ~Callback() {}
	};

	PipelineMonitorable() = delete;
	PipelineMonitorable(Callback* rtpstats_cb, Callback* data_cb):
		rtpstats_cb_(rtpstats_cb),
		data_cb_(data_cb)
	{}

protected:
	Callback* rtpstats_cb_;
	Callback* data_cb_;

	virtual ~PipelineMonitorable() {}
};

}

#endif