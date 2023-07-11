#pragma once
#include <gst/gst.h>
#include "MediaPipeline.h"

#define H_264_TUNE_NONE         (0x00000000)
#define H_264_TUNE_STILLIMAGE   (0x00000001)
#define H_264_TUNE_FASTCODE     (0x00000002)
#define H_264_TUNE_ZEROLATENCY  (0x00000004)

#define SRTP_ENABLE (1)

namespace media {

class VideoMediaPipeline: public MediaPipeline
{
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
	SubElements pipeline_make_overlay(GstBin* parent_bin, int bin_index, int client_index) override;

	void update_adder_parameter(GstBin* parent_bin, int bin_index, int client_index) override;
public:
	VideoMediaPipeline(string rid, const vector<PipeMode>& pipe_mode_list,
		PipelineMonitorable::Callback* rtpstats_cb, PipelineMonitorable::Callback* data_cb);
	void setVideoQuality(int video_quality_index) override;
	//void makePipeline(vector<ContactInfo*> contact_info_list, OperatingInfo* operate_info);
	//void add_client(ContactInfo* client_info):
	//void remove_client(ContactInfo* client_info);
};

} // namespace media