#pragma once
#include "call_info.h"
#include "pipeline_monitorable.h"
#include "media_types.h"

#include<vector>
#include<thread>
#include <gst/gst.h>
#include <queue>
#include <map>
#include <mutex>
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

enum pipe_block_flag {
    BLOCK_MONITOR,
	BLOCK_NO_JOIN,
	BLOCK_VIODE_STOP,
	BLOCK_EXIT,
	BLOCK_MAX
};

enum element_type {
	TYPE_INPUT_DEVICE,
	TYPE_INPUT_CAPS,
	TYPE_OUTPUT_DEVICE,
	TYPE_CONVERT,
	TYPE_RESCALE,
	TYPE_RESCALE_CAPS,
	TYPE_UDP_CAPS,
	TYPE_ENCODING,
	TYPE_ENCODING_RTP,
	TYPE_DECODING,
	TYPE_DECODING_RTP,
	TYPE_OVERLAY,
	TYPE_ADDER,
	TYPE_JITTER,
	TYPE_UDP_SINK,
	TYPE_UDP_SRC,
	TYPE_FQUEUE,
	TYPE_BQUEUE,
	TYPE_TEE,
	TYPE_SRTPENC,
	TYPE_SRTPENC_CAPS,
	TYPE_SRTPDEC,
	TYPE_SRTPDEC_CAPS, 
	TYPE_MAX
};

struct VideoQualityInfo
{
	int video_quality_index;
	bool enable_recover_timer;
};

typedef pair<GstElement*, GstElement*> SubElements;
typedef pair<int, int> PipeMode;
typedef pair<int, bool> CIDInfo;
typedef unsigned long long handleptr;

namespace media {

class MediaPipeline : public PipelineMonitorable
{
protected :
	std::mutex message_mutex_;
	string rid_;
	string media_mode_;
	std::thread pipeline_thread_;
	vector<PipeMode> pipe_mode_list_;
	map<int, ContactInfo> contact_info_list_;
	map<string, CIDInfo> client_id_list_;
	OperatingInfo operate_info_;
	handleptr view_handler_;
	GstElement* pipeline;
	queue<ContactInfo> client_queue_;
	queue<VideoQualityInfo> video_quality_queue_;
	bool start_pipeline_;
	int pipe_block_flag_;
	GMainLoop* mainLoop_;
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
	virtual SubElements pipeline_make_overlay(GstBin* parent_bin, int bin_index, int client_index) = 0;
	SubElements pipeline_make_udp_sink_with_port(GstBin* parent_bin, int bin_index, int client_index, int port);
	SubElements pipeline_make_udp_src_with_port(GstBin* parent_bin, int bin_index, int client_index, int port);
	SubElements pipeline_make_queue(GstBin* parent_bin, int bin_index, int client_index, bool is_front);
	SubElements pipeline_make_tee(GstBin* parent_bin, int bin_index, int client_index);
	SubElements pipeline_make_encryption(GstBin* parent_bin, int bin_index, int client_index);
	SubElements pipeline_make_restoration(GstBin* parent_bin, int bin_index, int client_index);

	void unref_element(GstBin* parent_bin, GstElement* current_element);
	virtual void update_adder_parameter(GstBin* parent_bin, int bin_index, int client_index) = 0;
	SubElements make_front_device(GstBin* parent_bin, int bin_index, int client_index);
	SubElements make_front_udp_n(GstBin* parent_bin, int bin_index, int client_index);
	SubElements make_back_device(GstBin* parent_bin, int bin_index, int client_index);
	SubElements make_back_udp_n(GstBin* parent_bin, int bin_index, int client_index);
	void add_client_in_front(GstBin* parent_bin, int bin_index, int client_index);
	void add_client_in_back(GstBin* parent_bin, int bin_index, int client_index);
	SubElements add_client_at_src(GstBin* parent_bin, int bin_index, int client_index);

	void add_client_udp_remove_me(GstBin* parent_bin, int bin_index, int client_index);
	void remove_element_list(GstBin* parent_bin, string current_name, string target_name, string pad_type);
	void remove_client_in_front(GstBin* parent_bin, int bin_index, int client_index);
	void remove_client_in_back(GstBin* parent_bin, int bin_index, int client_index);
	void remove_client_udp_remove_me(GstBin* parent_bin, int bin_index, int client_index);
	void unref_and_unlink_all_pads(GstBin* parent_bin, string target_name, string pad_type);
	void make_bin(GstBin* parent_bin, int bin_index, int front, int back);
	int get_client_index(ContactInfo* client_info, bool new_client);
	void disable_client_index(ContactInfo* client_info);
	void logPipelineElements(GstElement* element, int level);
	vector<GstElement*> get_elements_list(element_type etype);
	string getLinkedElements(GstElement* element);
	SubElements connect_subElements(SubElements front, SubElements back);
	string get_elements_name(element_type etype, int bin_index, int client_index, std::string suffix = "");
	SubElements get_elements_by_name(GstBin* parent_bin, element_type etype, int bin_index, int client_index);
	void enable_debugging(void);
	string get_pipeline_info(int bin_index);
	string get_pipe_mode_name(int mode);
	int count_active_client(void);
public:
	MediaPipeline(string rid, const vector<PipeMode>& pipe_mode_list, PipelineMonitorable::Callback* monitor_cb);
	void makePipeline(vector<ContactInfo> &contact_info_list, OperatingInfo& operate_info, handleptr handler);
	void pipeline_run();
	void request_add_client(ContactInfo* client_info);
	void request_remove_client(ContactInfo* client_info);
	void add_client(ContactInfo* client_info);
	void remove_client(ContactInfo* client_info);
	void requestSetVideoQuality(VideoQualityInfo* vq_info);
	virtual void setVideoQuality(int video_quality_index) = 0;
	void set_pipe_block_flag(pipe_block_flag flag_type);
	void unset_pipe_block_flag(pipe_block_flag flag_type);
	void stop_state_pipeline(bool stop);
	void end_call();

private:
	uint32_t bus_watch_id_;
	uint32_t timer_id_;

	static GstBusSyncReply BusSyncHandlerCallback(GstBus* bus, GstMessage* message, gpointer data) {
		MediaPipeline* pipeline = static_cast<MediaPipeline*>(data);
		return pipeline->BusSyncHandler(bus, message, pipeline);
	}
	static bool BusHandlerCallback(GstBus* bus, GstMessage* message, gpointer data) {
		MediaPipeline* pipeline = static_cast<MediaPipeline*>(data);
		return pipeline->BusHandler(bus, message, pipeline);
	}
	GstBusSyncReply BusSyncHandler(GstBus* bus, GstMessage* message, gpointer data);
	bool BusHandler(GstBus* bus, GstMessage* message, gpointer data);
	static bool messageTask(gpointer data);
	void checkMessageQueue(void);
	static bool TimerTask(gpointer data);
	void ReadAndNotifyRtpStats();

};

} // namespace media
