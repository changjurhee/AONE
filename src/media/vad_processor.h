#ifndef MEDIA_MEDIA_VAD_PROCESSOR_H_
#define MEDIA_MEDIA_VAD_PROCESSOR_H_

#include <thread>

#include "vad/webrtc_vad.h"
#include "litevad.h"
#include "audio_buffer.h"

namespace media {

// NOT MT_SAFE
class VadProcessor {
public:
	class VadStatusCallback {
	public:
		virtual void OnVadStatus(bool is_silent) = 0;
	protected:
		virtual ~VadStatusCallback() {}
	};

	VadProcessor();
    ~VadProcessor();

    // if sample_rate is 0, just clear all.
    bool Initialize(int sample_rate, int channels, int bits_per_sample);

	// If false, it means that the packets are silen.
	bool OnData(const AudioBuffer& buf, std::size_t frames_per_buffer);

private:
	litevad_handle_t vad_handle_;
	litevad_result_t last_vad_status_;

};
} // namespace media

#endif