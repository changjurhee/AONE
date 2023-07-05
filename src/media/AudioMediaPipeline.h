#pragma once
#include "MediaPipeline.h"
#include <vector>

class AudioMediaPipeline: public MediaPipeline
{
protected:
	SubElements pipeline_make_input_device(GstBin* parent_bin) override;
	SubElements pipeline_make_output_device(GstBin* parent_bin) override;

	SubElements pipeline_make_convert(GstBin* parent_bin, int bin_index, int client_index) override;
	SubElements pipeline_make_rescale(GstBin* parent_bin, int bin_index, int client_index, int level) override;

	SubElements pipeline_make_encoding(GstBin* parent_bin, int bin_index, int client_index) override;
	SubElements pipeline_make_decoding(GstBin* parent_bin, int bin_index, int client_index) override;
	SubElements pipeline_make_adder(GstBin* parent_bin, int bin_index, int client_index) override;
	SubElements pipeline_make_jitter_buffer(GstBin* parent_bin, int bin_index, int client_index) override;
	SubElements pipeline_make_udp_sink(GstBin* parent_bin, int port, int client_index) override;
	SubElements pipeline_make_udp_src(GstBin* parent_bin, int port, int client_index) override;
public:
	AudioMediaPipeline(int call_index, const vector<PipeMode>& pipe_mode_list);
	//void makePipeline(std::vector<ContactInfo*> contact_info_list, OperatingInfo* operate_info);
	void setVideoQuality(int video_quality_index) override;
	//void add_client(ContactInfo* client_info);
	//void remove_client(ContactInfo* client_info);
};

