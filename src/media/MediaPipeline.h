#pragma once
#include "call_info.h"
#include "../common/debug.h"

#include<vector>
#include<thread>
#include <gst/gst.h>
#include <queue>
#include <map>
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

enum element_type {
	TYPE_INPUT_DEVICE,
	TYPE_OUTPUT_DEVICE,
	TYPE_CONVERT,
	TYPE_RESCALE,
	TYPE_RESCALE_CAPS,
	TYPE_UDP_CAPS,
	TYPE_ENCODING,
	TYPE_ENCODING_RTP,
	TYPE_DECODING,
	TYPE_DECODING_RTP,
	TYPE_ADDER,
	TYPE_JITTER,
	TYPE_UDP_SINK,
	TYPE_UDP_SRC,
	TYPE_QUEUE,
	TYPE_TEE,
	TYPE_MAX
};

typedef pair<GstElement*, GstElement*> SubElements;
typedef pair<int, int> PipeMode;
typedef pair<int, bool> CIDInfo;
typedef unsigned long long handleptr;

class MediaPipeline
{
protected :
	string rid_;
	int media_mode_;
	int send_input_mode_;
	int send_output_mode_;
	int receive_input_mode_;
	int receive_output_mode_;
	std::thread pipeline_thread_;
	vector<PipeMode> pipe_mode_list_;
	map<int, ContactInfo> contact_info_list_;
	map<string, CIDInfo> client_id_list_;
	OperatingInfo operate_info_;
	handleptr view_handler_;
	GstElement* pipeline;
	queue<ContactInfo> client_queue_;
	bool start_pipeline_;
	virtual SubElements pipeline_make_input_device(GstBin* parent_bin, int bin_index, int client_index) = 0;
	virtual SubElements pipeline_make_output_device(GstBin* parent_bin, int bin_index, int client_index) = 0;
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

	virtual void update_adder_parameter(GstBin* parent_bin, int bin_index, int client_index) = 0;
	SubElements make_front_device(GstBin* parent_bin, int bin_index, int client_index);
	SubElements make_front_udp_n(GstBin* parent_bin, int bin_index, int client_index);
	SubElements make_back_device(GstBin* parent_bin, int bin_index, int client_index);
	SubElements make_back_udp_n(GstBin* parent_bin, int bin_index, int client_index);
	void add_client_in_front(GstBin* parent_bin, int bin_index, int client_index);
	void add_client_in_back(GstBin* parent_bin, int bin_index, int client_index);
	void remove_client_in_front(GstBin* parent_bin, int bin_index, int client_index);
	void remove_client_in_back(GstBin* parent_bin, int bin_index, int client_index);
	void make_bin(GstBin* parent_bin, int bin_index, int front, int back);
	int get_client_index(ContactInfo* client_info);
	void disable_client_index(ContactInfo* client_info);
	void logPipelineElements(GstElement* element, int level);
	vector<GstElement*> get_elements_list(element_type etype);
	string getLinkedElements(GstElement* element);
	SubElements connect_subElements(SubElements front, SubElements back);
	string get_elements_name(element_type etype, int bin_index, int client_index);
	SubElements get_elements_by_name(GstBin* parent_bin, element_type etype, int bin_index, int client_index);
	bool check_pipeline(ContactInfo* client_info);
	void add_waiting_client(void);
	void enable_debugging(void);
public:
	MediaPipeline(string rid, const vector<PipeMode>& pipe_mode_list);
	void makePipeline(vector<ContactInfo> &contact_info_list, OperatingInfo& operate_info, handleptr handler);
	void pipeline_run();
	void add_client(ContactInfo* client_info);
	void remove_client(ContactInfo* client_info);
	virtual void setVideoQuality(int video_quality_index) = 0;
	
	void end_call();
};

