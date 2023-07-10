#pragma once
#include "MediaPipeline.h"
#include <vector>

namespace media {

class AudioMediaPipeline: public MediaPipeline
{
protected:
	SubElements pipeline_make_input_device(GstBin* parent_bin, int bin_index, int client_index) override;
	SubElements pipeline_make_output_device(GstBin* parent_bin, int bin_index, int client_index) override;

	SubElements pipeline_make_convert(GstBin* parent_bin, int bin_index, int client_index) override;
	SubElements pipeline_make_rescale(GstBin* parent_bin, int bin_index, int client_index, int level) override;

	SubElements pipeline_make_encoding(GstBin* parent_bin, int bin_index, int client_index) override;
	SubElements pipeline_make_decoding(GstBin* parent_bin, int bin_index, int client_index) override;
	SubElements pipeline_make_adder(GstBin* parent_bin, int bin_index, int client_index) override;
	SubElements pipeline_make_jitter_buffer(GstBin* parent_bin, int bin_index, int client_index) override;
	SubElements pipeline_make_udp_sink(GstBin* parent_bin, int port, int client_index) override;
	SubElements pipeline_make_udp_src(GstBin* parent_bin, int port, int client_index) override;
	SubElements pipeline_make_encryption(GstBin* parent_bin, int bin_index, int client_index);
	SubElements pipeline_make_restoration(GstBin* parent_bin, int bin_index, int client_index);
	void update_adder_parameter(GstBin* parent_bin, int bin_index, int client_index) override;
public:
	AudioMediaPipeline(string rid, const vector<PipeMode>& pipe_mode_list, PipelineMonitorable::Callback* monitor_cb);
	//void makePipeline(std::vector<ContactInfo*> contact_info_list, OperatingInfo* operate_info);
	void setVideoQuality(int video_quality_index) override;
	//void add_client(ContactInfo* client_info);
	//void remove_client(ContactInfo* client_info);

private:
	static GstPadProbeReturn ProbeForProcessingVad(GstPad* pad, GstPadProbeInfo* info, gpointer user_data) {
		AudioMediaPipeline* pipeline = static_cast<AudioMediaPipeline*>(user_data);

		if (pipeline->ProcessVad(info))
			return GST_PAD_PROBE_OK;
		else
			// silent so the buffer should be dropped.
			return GST_PAD_PROBE_DROP;
	}

	bool ProcessVad(GstPadProbeInfo* info);
};

} // namespace media