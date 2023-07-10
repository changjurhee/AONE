#include "vad_processor.h"

#include <cassert>
#include "common/logger.h"

namespace media {

VadProcessor::VadProcessor() :
    vad_handle_(nullptr),
    last_vad_status_(LITEVAD_RESULT_NOTSET)
{
}

VadProcessor::~VadProcessor() {
    if (vad_handle_) {
        litevad_destroy(vad_handle_);
        vad_handle_ = nullptr;
        last_vad_status_ = LITEVAD_RESULT_NOTSET;
    }
}

bool VadProcessor::Initialize(int sample_rate, int channels, int bits_per_sample) {
    if (vad_handle_) {
        litevad_destroy(vad_handle_);
        vad_handle_ = nullptr;
        last_vad_status_ = LITEVAD_RESULT_NOTSET;
    }

    if (!sample_rate)
        return true;

    assert(vad_handle_ == nullptr);
    assert(last_vad_status_ == LITEVAD_RESULT_NOTSET);

    vad_handle_ = litevad_create(sample_rate, channels, bits_per_sample);
    if (!vad_handle_) {
        LOG_WARN("Failed litevad_create()");
        return false;
    }

    return true;
}

bool VadProcessor::OnData(const AudioBuffer& buf, std::size_t frames_per_buffer) {
    //vad_state = litevad_process(vad_handle, (const int16_t*)(pbMicInputBuffer + (i * BytesPerFrame)), FRAMES_PER_BUFFER);

    //LOG_OBJ_DEBUG() << buf.size() << " " << frames_per_buffer << std::endl;
    litevad_result_t vad_state =
        litevad_process(vad_handle_, (const int16_t*)buf.data(), frames_per_buffer);

    if (vad_state != last_vad_status_)
    {
        //PostMessage(hWndMain, WM_VAD_STATE, vad_state, 0);
        last_vad_status_ = vad_state;
        switch (vad_state) {
        case LITEVAD_RESULT_SPEECH_BEGIN:
            LOG_OBJ_DEBUG() << "Speech Begin" << std::endl;
            break;
        case LITEVAD_RESULT_SPEECH_END:
            LOG_OBJ_DEBUG() << "Speech End" << std::endl;
            break;
        case LITEVAD_RESULT_SPEECH_BEGIN_AND_END:
            LOG_OBJ_DEBUG() << "Speech Begin & End" << std::endl;
            break;
        case LITEVAD_RESULT_FRAME_SILENCE:
            LOG_OBJ_DEBUG() << "Silence" << std::endl;
            break;
        case LITEVAD_RESULT_FRAME_ACTIVE:
            //std::cout << "Frame Active" << std::endl;
            break;
        case LITEVAD_RESULT_ERROR:
            LOG_OBJ_DEBUG() << "VAD Error" << std::endl;
            break;
        default:
            LOG_OBJ_DEBUG() << "VAD State Unknown" << std::endl;
            break;
        }
    }

    if (vad_state == LITEVAD_RESULT_FRAME_SILENCE)
        return false;
    else
        return true;
}

} // namespace media