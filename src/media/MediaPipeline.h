#pragma once
#include "call_info.h"
#include "../common/debug.h"

#include<vector>
#include<thread>
#include <gst/gst.h>
using namespace std;

#define BASE_CLIENT_ID 99
enum pipe_topology_mode {
	MODE_NONE,
	MODE_DEVICE,
	MODE_UDP_1,
	MODE_UDP_N,
	MODE_UDP_REMOVE_ME,
	MODE_MAX
};

typedef pair<GstElement*, GstElement*> SubElements;
typedef pair<int, int> PipeMode;

class MediaPipeline
{
protected :
	int call_index_;
	int media_mode_;
	int send_input_mode_;
	int send_output_mode_;
	int receive_input_mode_;
	int receive_output_mode_;
	std::thread pipeline_thread_;
	vector<PipeMode> pipe_mode_list_;
	vector<ContactInfo> contact_info_list_;
	OperatingInfo operate_info_;
	GstElement* pipeline;
	virtual SubElements pipeline_make_input_device(GstBin* parent_bin) = 0;
	virtual SubElements pipeline_make_output_device(GstBin* parent_bin) = 0;
	virtual SubElements pipeline_make_convert(GstBin* parent_bin, int bin_index, int client_index) = 0;
	virtual SubElements pipeline_make_rescale(GstBin* parent_bin, int bin_index, int client_index, int level) = 0;
	virtual SubElements pipeline_make_encoding(GstBin* parent_bin, int bin_index, int client_index) = 0;
	virtual SubElements pipeline_make_decoding(GstBin* parent_bin, int bin_index, int client_index) = 0;

	virtual SubElements pipeline_make_adder(GstBin* parent_bin, int bin_index, int client_index) = 0;
	virtual SubElements pipeline_make_jitter_buffer(GstBin* parent_bin, int bin_index, int client_index) = 0;
	virtual SubElements pipeline_make_udp_sink(GstBin* parent_bin, int bin_index, int client_index) = 0;
	virtual SubElements pipeline_make_udp_src(GstBin* parent_bin, int bin_index, int client_index) = 0;
	SubElements pipeline_make_udp_sink_with_port(GstBin* parent_bin, int bin_index, int client_index, int port);
	SubElements pipeline_make_udp_src_with_port(GstBin* parent_bin, int bin_index, int client_index, int port);
	SubElements pipeline_make_queue(GstBin* parent_bin, int bin_index, int client_index);
	SubElements pipeline_make_tee(GstBin* parent_bin, int bin_index, int client_index);

	SubElements make_front_device(GstBin* parent_bin, int bin_index, int client_index);
	SubElements make_front_udp_1(GstBin* parent_bin, int bin_index, int client_index);
	SubElements make_front_udp_n(GstBin* parent_bin, int bin_index, int client_index);
	SubElements make_back_udp_1(GstBin* parent_bin, int bin_index, int client_index);
	SubElements make_back_device(GstBin* parent_bin, int bin_index, int client_index);
	SubElements MediaPipeline::make_back_udp_n(GstBin* parent_bin, int bin_index, int client_index);
	void MediaPipeline::add_client_in_front(GstBin* parent_bin, int bin_index, int client_index);
	void MediaPipeline::add_client_in_back(GstBin* parent_bin, int bin_index, int client_index);
	void MediaPipeline::remove_client_in_front(GstBin* parent_bin, int bin_index, int client_index);
	void MediaPipeline::remove_client_in_back(GstBin* parent_bin, int bin_index, int client_index);
	void MediaPipeline::make_bin(GstBin* parent_bin, int bin_index, int front, int back);
	int MediaPipeline::get_client_index(ContactInfo* client_info);
	void logPipelineElements(GstElement* element, int level);
	string MediaPipeline::getLinkedElements(GstElement* element);
public:
	MediaPipeline(int call_index, const vector<PipeMode>& pipe_mode_list);
	void makePipeline(vector<ContactInfo> &contact_info_list, OperatingInfo& operate_info);
	void pipeline_run();
	void add_client(ContactInfo* client_info);
	void remove_client(ContactInfo* client_info);
	virtual void setVideoQuality(int video_quality_index) = 0;
	
	void end_call();
};

