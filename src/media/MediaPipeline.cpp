#include "MediaPipeline.h"
#include <thread>
#include <functional>
#include <algorithm>
#include <format>

#include "common/logger.h"

namespace media {

MediaPipeline::MediaPipeline(string rid, const vector<PipeMode>& pipe_mode_list, PipelineMonitorable::Callback* monitor_cb) :
	PipelineMonitorable(monitor_cb) {

	rid_ = rid;
	pipe_mode_list_ = pipe_mode_list;
	start_pipeline_ = false;
	view_handler_ = NULL;

	bus_watch_id_ = 0;
	timer_id_ = 0;
	pipe_block_flag_ = 0;

	OperatingInfo* operate_info_ptr = new OperatingInfo();
	operate_info_ = *operate_info_ptr;
};

SubElements MediaPipeline::connect_subElements(SubElements front, SubElements back) {
	if (front.second == NULL) return back;
	if (back.first == NULL) return front;

	gst_element_link(front.second, back.first);
	return SubElements(front.first, back.second);
}

string MediaPipeline::get_elements_name(element_type etype, int bin_index, int client_index) {
	string name;
	switch (etype)
	{
		case TYPE_INPUT_DEVICE :
			name = "input_device_" + std::to_string(bin_index) + "_" + std::to_string(client_index);;
			break;
		case TYPE_INPUT_CAPS:
			name = "input_caps_" + std::to_string(bin_index) + "_" + std::to_string(client_index);;
			break;
		case TYPE_OUTPUT_DEVICE :
			name = "output_device_" + std::to_string(bin_index) + "_" + std::to_string(client_index);;
			break;
		case TYPE_CONVERT :
			name = "convert_" + std::to_string(bin_index) + "_" + std::to_string(client_index);
			break;
		case TYPE_RESCALE :
			name = "scale_" + std::to_string(bin_index) + "_" + std::to_string(client_index);
			break;
		case TYPE_RESCALE_CAPS:
			name = "recale_caps_" + std::to_string(bin_index) + "_" + std::to_string(client_index);
			break;
		case TYPE_UDP_CAPS:
			name = "udp_caps_" + std::to_string(bin_index) + "_" + std::to_string(client_index);
			break;
		case TYPE_ENCODING :
			name = "encoding_" + std::to_string(bin_index) + "_" + std::to_string(client_index);
			break;
		case TYPE_ENCODING_RTP:
			name = "encoding_rtp_" + std::to_string(bin_index) + "_" + std::to_string(client_index);
			break;
		case TYPE_DECODING:
			name = "decoding_" + std::to_string(bin_index) + "_" + std::to_string(client_index);
			break;
		case TYPE_DECODING_RTP:
			name = "decoding_rtp_" + std::to_string(bin_index) + "_" + std::to_string(client_index);
			break;
		case TYPE_ADDER :
			name = "adder_" + std::to_string(bin_index) + "_" + std::to_string(client_index);
			break;
		case TYPE_JITTER :
			name = "jitterbuffer_" + std::to_string(bin_index) + "_" + std::to_string(client_index);
			break;
		case TYPE_UDP_SINK :
			name = "udpsink_" + std::to_string(bin_index) + "_" + std::to_string(client_index);
			break;
		case TYPE_UDP_SRC :
			name = "udpsrc_" + std::to_string(bin_index) + "_" + std::to_string(client_index);
			break;
		case TYPE_TEE :
			name = "tee_" + std::to_string(bin_index) + "_" + std::to_string(client_index);
			break;
		case TYPE_FQUEUE :
			name = "front_queue_" + std::to_string(bin_index) + "_" + std::to_string(client_index);
			break;
		case TYPE_BQUEUE:
			name = "back_queue_" + std::to_string(bin_index) + "_" + std::to_string(client_index);
			break;
		default:
			break;
	}
	return name;
}

SubElements MediaPipeline::get_elements_by_name(GstBin* parent_bin, element_type etype, int bin_index, int client_index)
{
	string name = get_elements_name(etype, bin_index, client_index);
	GstElement* element = gst_bin_get_by_name(GST_BIN(parent_bin), name.c_str());
	return SubElements(element, element);
}

vector<GstElement*> MediaPipeline::get_elements_list(element_type etype)
{
	vector<GstElement*> elements_list;
	for (int bin_index = 0; bin_index < pipe_mode_list_.size(); bin_index++) {
		std::string name = "bin_" + std::to_string(bin_index);
		GstBin* bin = GST_BIN(gst_bin_get_by_name(GST_BIN(pipeline), name.c_str()));

		auto iter = client_id_list_.begin();
		while (iter != client_id_list_.end()) {
			if (!iter->second.second) {
				++iter;
				continue;
			}
			int client_index = iter->second.first;
			string name = get_elements_name(etype, bin_index, client_index);
			GstElement* element = gst_bin_get_by_name(bin, name.c_str());
			if (element != NULL)
				elements_list.push_back(element);
			++iter;
		}
		name = get_elements_name(etype, bin_index, BASE_CLIENT_ID);
		GstElement* element = gst_bin_get_by_name(bin, name.c_str());
		if (element != NULL)
			elements_list.push_back(element);
	}
	return elements_list;
}

SubElements pipeline_make_encryption(GstBin* parent_bin, int bin_index, int client_index) {
	return SubElements(NULL, NULL);
}

SubElements pipeline_make_restoration(GstBin* parent_bin, int bin_index, int client_index) {
	return SubElements(NULL, NULL);
}

SubElements MediaPipeline::pipeline_make_queue(GstBin* parent_bin, int bin_index, int client_index, bool is_front) {
	std::string name = get_elements_name(is_front ? TYPE_FQUEUE : TYPE_BQUEUE, bin_index, client_index);
	GstElement *element = gst_element_factory_make("queue", name.c_str());
	gst_bin_add(GST_BIN(parent_bin), element);
	return SubElements(element, element);
};

SubElements MediaPipeline::pipeline_make_tee(GstBin* parent_bin, int bin_index, int client_index) {
	std::string name = get_elements_name(TYPE_TEE, bin_index, client_index);
	GstElement *element = gst_element_factory_make("tee", name.c_str());
	gst_bin_add(GST_BIN(parent_bin), element);
	return SubElements(element, element);
};

SubElements MediaPipeline::pipeline_make_udp_sink_with_port(GstBin* parent_bin, int bin_index, int client_index, int port) {
	std::string name = get_elements_name(TYPE_UDP_SINK, bin_index, client_index);
	GstElement* element = gst_element_factory_make("udpsink", name.c_str());

	std::string host = contact_info_list_[client_index].dest_ip;

	g_printerr(("host " + host + " port : " + std::to_string(port)+"\n").c_str());
	g_object_set(element, "host", host.c_str(), "port", port, "sync", FALSE, "processing-deadline", 0, NULL);
	gst_bin_add(GST_BIN(parent_bin), element);

	return SubElements(element, element);
};

SubElements MediaPipeline::pipeline_make_udp_src_with_port(GstBin* parent_bin, int bin_index, int client_index, int port) {
	std::string name = get_elements_name(TYPE_UDP_SRC, bin_index, client_index);
	GstElement* element = gst_element_factory_make("udpsrc", name.c_str());
	g_object_set(element, "port", port, NULL);
	gst_bin_add(GST_BIN(parent_bin), element);

	return SubElements(element, element);
};

SubElements MediaPipeline::make_front_device(GstBin* parent_bin, int bin_index, int client_index) {
	SubElements ret_sub_elements = SubElements(NULL, NULL);
	SubElements input_pair = pipeline_make_input_device(parent_bin, bin_index, client_index);
	ret_sub_elements = connect_subElements(ret_sub_elements, input_pair);

	SubElements convert_pair = pipeline_make_convert(parent_bin, bin_index, client_index);
	ret_sub_elements = connect_subElements(ret_sub_elements, convert_pair);

	SubElements rescale_pair = pipeline_make_rescale(parent_bin, bin_index, client_index, 0);
	ret_sub_elements = connect_subElements(ret_sub_elements, rescale_pair);

	return ret_sub_elements;
}

SubElements MediaPipeline::make_front_udp_n(GstBin* parent_bin, int bin_index, int client_index)
{
	SubElements ret_sub_elements = SubElements(NULL, NULL);
	SubElements adder_pair = pipeline_make_adder(parent_bin, bin_index, client_index);
	ret_sub_elements = connect_subElements(ret_sub_elements, adder_pair);

	SubElements convert_pair = pipeline_make_convert(parent_bin, bin_index, client_index);
	ret_sub_elements = connect_subElements(ret_sub_elements, convert_pair);

	return ret_sub_elements;
}


SubElements MediaPipeline::make_back_device(GstBin* parent_bin, int bin_index, int client_index) {
	return pipeline_make_output_device(parent_bin, bin_index, client_index);
}

SubElements MediaPipeline::make_back_udp_n(GstBin* parent_bin, int bin_index, int client_index) {
	SubElements ret_sub_elements = SubElements(NULL, NULL);

	SubElements encoding_pair = pipeline_make_encoding(parent_bin, bin_index, client_index);
	ret_sub_elements = connect_subElements(ret_sub_elements, encoding_pair);

	SubElements encryption_pair = pipeline_make_encryption(parent_bin, bin_index, client_index);
	ret_sub_elements = connect_subElements(ret_sub_elements, encryption_pair);

	SubElements tee_pair = pipeline_make_tee(parent_bin, bin_index, client_index);
	ret_sub_elements = connect_subElements(ret_sub_elements, tee_pair);

	return ret_sub_elements;
}


void MediaPipeline::make_bin(GstBin* parent_bin, int bin_index, int front, int back) {

	SubElements front_pair = SubElements(NULL, NULL);
	switch (front)
	{
		case MODE_DEVICE:
			front_pair = make_front_device(parent_bin, bin_index, BASE_CLIENT_ID);
			break;

		case MODE_UDP_1:
		case MODE_UDP_N:
			front_pair = make_front_udp_n(parent_bin, bin_index, BASE_CLIENT_ID);
			break;

		case MODE_NONE:
		case MODE_UDP_REMOVE_ME: // nothing
		default:
			break;
	}

	SubElements back_pair = SubElements(NULL, NULL);
	switch (back)
	{
		case MODE_DEVICE:
			back_pair = make_back_device(parent_bin, bin_index, BASE_CLIENT_ID);
			break;

		case MODE_UDP_1:
		case MODE_UDP_N:
			back_pair = make_back_udp_n(parent_bin, bin_index, BASE_CLIENT_ID);
			break;

		case MODE_NONE:
		default:
			break;
	}
	if (back_pair.first != NULL && front_pair.second)
		gst_element_link(front_pair.second, back_pair.first);
}

void MediaPipeline::enable_debugging(void)
{
	//GstDebugLevel logLevel = GST_LEVEL_DEBUG;
	//gst_debug_set_threshold_for_name("mfvideosrc", logLevel);
	//gst_debug_set_threshold_for_name("videoscale", logLevel);
	//gst_debug_set_threshold_for_name("scale", logLevel);
	//gst_debug_set_threshold_for_name("capsfilter", logLevel);
	//gst_debug_set_threshold_for_name("x264enc", logLevel);
	//gst_debug_set_threshold_for_name("rtph264pay", logLevel);
	//gst_debug_set_threshold_for_name("udpsink", logLevel);
	/* Set default debug level */
	//gst_debug_set_default_threshold(GST_LEVEL_FIXME);
	//g_printerr("log start!!!\n");

	//for (int etype_int = TYPE_INPUT_DEVICE; etype_int != TYPE_MAX; etype_int++)
	//{
	//	element_type etype = static_cast<element_type>(etype_int);
	//	vector<GstElement*> element_list = get_elements_list(etype);
	//	g_printerr(("int : " +std::to_string(etype_int) + " , size : "+ std::to_string(element_list.size())+"\n").c_str());
	//}
}

void MediaPipeline::pipeline_run() {
	g_printerr("pipeline : Start!!!.\n");
	pipeline = gst_pipeline_new("pipeline");
	for (int index = 0; index < pipe_mode_list_.size(); index++) {
		std::string name = "bin_"+std::to_string(index);
		GstElement *bin = gst_bin_new(name.c_str());
		make_bin(GST_BIN(bin), index, pipe_mode_list_[index].first, pipe_mode_list_[index].second);
		gst_bin_add(GST_BIN(pipeline), bin);
	}

	LOG_INFO("Setting handler on bus");
	GstBus* bus = gst_element_get_bus(pipeline);
	gst_bus_set_sync_handler(bus, BusSyncHandlerCallback, (gpointer)this, NULL);
	bus_watch_id_ = gst_bus_add_watch(bus, (GstBusFunc)BusHandlerCallback, (gpointer)this);
	gst_object_unref(bus);

	LOG_INFO("Setting timer (500 ms)");
	g_timeout_add(500, (GSourceFunc)TimerTask, (gpointer)this);

	g_timeout_add(100, (GSourceFunc)messageTask, (gpointer)this);

	start_pipeline_ = true;
	// Pipeline execution
	LOG_INFO("Setting pipeline to PLAYING");
    GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        g_printerr("Sender : Unable to start the pipeline.\n");
        gst_object_unref(pipeline);
        return;
    }

	logPipelineElements(pipeline, 0);
	enable_debugging();
	// Start the main loop
	mainLoop_ = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(mainLoop_);

    // Release the pipeline
    gst_object_unref(pipeline);
  	g_source_remove(bus_watch_id_);
    g_printerr("pipeline : End!!!.\n");
}


void MediaPipeline::makePipeline(vector<ContactInfo> &contact_info_list, OperatingInfo& operate_info, handleptr view_handler)
{
	contact_info_list_;
	memcpy(&operate_info_, &operate_info, sizeof(OperatingInfo));
	view_handler_ = view_handler;

	pipeline_thread_ = std::thread(&MediaPipeline::pipeline_run, this);
}


void MediaPipeline::end_call()
{
	set_pipe_block_flag(BLOCK_EXIT);
	// Release the pipeline
	stop_state_pipeline(true);
	g_main_loop_quit(mainLoop_);
}

int MediaPipeline::get_client_index(ContactInfo* client_info, bool new_client)
{
	string cid = client_info->cid;
	int client_id = 0;
	if (client_id_list_.find(cid) == client_id_list_.end()) {
		if (!new_client) return -1;
		client_id = client_id_list_.size();
		client_id_list_[cid] = make_pair(client_id, true);
		contact_info_list_[client_id] = *client_info;
	} else if (client_id_list_[cid].second) {
		if (new_client) return -1;
		client_id = client_id_list_[cid].first;
	} else {
		if (!new_client) return -1;
		client_id = client_id_list_[cid].first;
		client_id_list_[cid].second = true;
	}

	return client_id;
};

void MediaPipeline::disable_client_index(ContactInfo* client_info)
{
	string cid = client_info->cid;
	if (client_id_list_.find(cid) != client_id_list_.end()) {
		client_id_list_[cid].second = false;
	}
}

SubElements MediaPipeline::add_client_at_src(GstBin * parent_bin, int bin_index, int client_index)
{
	SubElements ret_sub_elements = SubElements(NULL, NULL);

	SubElements udp_src_pair = pipeline_make_udp_src(parent_bin, bin_index, client_index);
	ret_sub_elements = connect_subElements(ret_sub_elements, udp_src_pair);

	SubElements jitter_pair = pipeline_make_jitter_buffer(parent_bin, bin_index, client_index);
	ret_sub_elements = connect_subElements(ret_sub_elements, jitter_pair);

	SubElements restoration_pair = pipeline_make_restoration(parent_bin, bin_index, client_index);
	ret_sub_elements = connect_subElements(ret_sub_elements, restoration_pair);

	SubElements decoding_pair = pipeline_make_decoding(parent_bin, bin_index, client_index);
	ret_sub_elements = connect_subElements(ret_sub_elements, decoding_pair);

	SubElements queue_pair = pipeline_make_queue(parent_bin, bin_index, client_index, true);
	ret_sub_elements = connect_subElements(ret_sub_elements, queue_pair);

	return ret_sub_elements;
}

void MediaPipeline::add_client_in_front(GstBin* parent_bin, int bin_index, int client_index)
{
	SubElements ret_sub_elements = SubElements(NULL, NULL);
	ret_sub_elements = add_client_at_src(parent_bin, bin_index, client_index);

	SubElements adder = get_elements_by_name(parent_bin, TYPE_ADDER, bin_index, BASE_CLIENT_ID);
	ret_sub_elements = connect_subElements(ret_sub_elements, adder);

	update_adder_parameter(parent_bin, bin_index, client_index);
}

void MediaPipeline::add_client_in_back(GstBin* parent_bin, int bin_index, int client_index)
{
	SubElements ret_sub_elements = SubElements(NULL, NULL);

	SubElements tee = get_elements_by_name(parent_bin, TYPE_TEE, bin_index, BASE_CLIENT_ID);
	ret_sub_elements = connect_subElements(ret_sub_elements, tee);

	SubElements queue_pair = pipeline_make_queue(parent_bin, bin_index, client_index, false);
	ret_sub_elements = connect_subElements(ret_sub_elements, queue_pair);

	SubElements udp_sink_pair = pipeline_make_udp_sink(parent_bin, bin_index, client_index);
	ret_sub_elements = connect_subElements(ret_sub_elements, udp_sink_pair);

	return;
}

void MediaPipeline::add_client_udp_remove_me(GstBin* parent_bin, int bin_index, int client_index)
{
#define REMOVE_ME_CONVERT_FLAG 10000
#define REMOVE_ME_SHIFT 100

	SubElements ret_sub_elements = SubElements(NULL, NULL);

	SubElements front_sub = add_client_at_src(parent_bin, bin_index, client_index);
	ret_sub_elements = connect_subElements(ret_sub_elements, front_sub);

	SubElements tee_pair = pipeline_make_tee(parent_bin, bin_index, client_index);
	ret_sub_elements = connect_subElements(ret_sub_elements, tee_pair);


	SubElements adder_pair = pipeline_make_adder(parent_bin, bin_index, client_index);
	ret_sub_elements = connect_subElements(ret_sub_elements, adder_pair);

	SubElements queue_pair = pipeline_make_queue(parent_bin, bin_index, client_index, false);
	ret_sub_elements = connect_subElements(ret_sub_elements, queue_pair);

	SubElements convert_pair = pipeline_make_convert(parent_bin, bin_index, client_index);
	ret_sub_elements = connect_subElements(ret_sub_elements, convert_pair);

	SubElements encoding_pair = pipeline_make_encoding(parent_bin, bin_index, client_index);
	ret_sub_elements = connect_subElements(ret_sub_elements, encoding_pair);

	SubElements encryption_pair = pipeline_make_encryption(parent_bin, bin_index, client_index);
	ret_sub_elements = connect_subElements(ret_sub_elements, encryption_pair);

	SubElements udp_sink_pair = pipeline_make_udp_sink(parent_bin, bin_index, client_index);
	ret_sub_elements = connect_subElements(ret_sub_elements, udp_sink_pair);

	for (const auto& item : client_id_list_) {
		bool client_enanle = item.second.second;
		int other_client_id = item.second.first;
		if (!client_enanle || other_client_id == client_index)
			continue;

		SubElements other_adder = get_elements_by_name(parent_bin, TYPE_ADDER, bin_index, other_client_id);
		SubElements other_tee = get_elements_by_name(parent_bin, TYPE_TEE, bin_index, other_client_id);

		SubElements convert_1 = pipeline_make_convert(parent_bin, bin_index, REMOVE_ME_CONVERT_FLAG + client_index* REMOVE_ME_SHIFT+ other_client_id);
		SubElements convert_2 = pipeline_make_convert(parent_bin, bin_index, REMOVE_ME_CONVERT_FLAG + other_client_id * REMOVE_ME_SHIFT + client_index);

		connect_subElements(tee_pair, convert_1);
		connect_subElements(convert_1, other_adder);

		connect_subElements(other_tee, convert_2);
		connect_subElements(convert_2, adder_pair);
	}
}

void MediaPipeline::unref_element(GstBin* parent_bin, GstElement* element)
{
	gst_bin_remove(GST_BIN(parent_bin), element);
}

void MediaPipeline::remove_element_list(GstBin* parent_bin, string current_name, string target_name, string pad_type)
{
	while (current_name != target_name) {
		// Determine the element linked to 'source'
		GstElement* current_element = gst_bin_get_by_name(parent_bin, current_name.c_str());

		GstPad* pad = gst_element_get_static_pad(current_element, pad_type.c_str());
		GstPad* linkedPad = gst_pad_get_peer(pad);
		GstElement* linkedElement = gst_pad_get_parent_element(linkedPad);
		if (linkedElement == NULL) break;

		gst_element_unlink(current_element, linkedElement);
		gst_object_unref(pad);
		gst_object_unref(linkedPad);
		unref_element(parent_bin, current_element);
		current_name = gst_element_get_name(linkedElement);
		if (target_name == "") break;
	}
	return;
}

void MediaPipeline::remove_client_in_front(GstBin* parent_bin, int bin_index, int client_index)
{
	string current_name = get_elements_name(TYPE_UDP_SRC, bin_index, client_index);
	string target_name = get_elements_name(TYPE_ADDER, bin_index, BASE_CLIENT_ID);
	remove_element_list(parent_bin, current_name, target_name, "src");
}

void MediaPipeline::remove_client_in_back(GstBin* parent_bin, int bin_index, int client_index)
{
	string current_name = get_elements_name(TYPE_UDP_SINK, bin_index, client_index);
	string target_name = get_elements_name(TYPE_TEE, bin_index, BASE_CLIENT_ID);
	remove_element_list(parent_bin, current_name, target_name, "sink");
}

void MediaPipeline::unref_and_unlink_all_pads(GstBin* parent_bin, string target_name, string pad_type)
{
	GstElement* element = gst_bin_get_by_name(parent_bin, target_name.c_str());
	GstIterator* it;
	if (pad_type == "src")
		it = gst_element_iterate_src_pads(element);
	else
		it = gst_element_iterate_sink_pads(element);
	gboolean done = FALSE;
	GstPad* pad;
	GValue item = G_VALUE_INIT;

	while (!done) {
		switch (gst_iterator_next(it, &item)) {
		case GST_ITERATOR_OK:
			pad = (GstPad*)g_value_get_object(&item);
			if (pad && GST_PAD_IS_LINKED(pad)) {
				GstPad* linkedPad = gst_pad_get_peer(pad);
				GstElement* linkedElement = gst_pad_get_parent_element(linkedPad);
				gst_element_unlink(element, linkedElement);
				remove_element_list(parent_bin, gst_element_get_name(linkedElement), "", pad_type);
			}
			g_value_reset(&item);
			break;
		case GST_ITERATOR_RESYNC:
			gst_iterator_resync(it);
			break;
		case GST_ITERATOR_ERROR:
		case GST_ITERATOR_DONE:
			done = TRUE;
			break;
		}
	}
	unref_element(parent_bin, element);
}

void MediaPipeline::remove_client_udp_remove_me(GstBin* parent_bin, int bin_index, int client_index)
{
	string udp_src_name = get_elements_name(TYPE_UDP_SRC, bin_index, client_index);
	string tee_name = get_elements_name(TYPE_TEE, bin_index, client_index);
	remove_element_list(parent_bin, udp_src_name, tee_name, "src");

	string udp_sink_name = get_elements_name(TYPE_UDP_SINK, bin_index, client_index);
	string adder_name = get_elements_name(TYPE_ADDER, bin_index, client_index);
	remove_element_list(parent_bin, udp_sink_name, adder_name, "sink");

	unref_and_unlink_all_pads(parent_bin, tee_name, "src");
	unref_and_unlink_all_pads(parent_bin, adder_name, "sink");
}

void MediaPipeline::request_add_client(ContactInfo* client_info)
{
	message_mutex_.lock();
	client_info->new_client = true;
	client_queue_.push(*client_info);
	message_mutex_.unlock();
}

void MediaPipeline::add_client(ContactInfo* client_info)
{
	int client_index = get_client_index(client_info, true);
	if (client_index < 0) return;

	stop_state_pipeline(true);
	for (int index = 0; index < pipe_mode_list_.size(); index++) {
		std::string name = "bin_"+std::to_string(index);
		GstBin *bin = GST_BIN(gst_bin_get_by_name(GST_BIN(pipeline), name.c_str()));

		int front = pipe_mode_list_[index].first;
		if (front == MODE_UDP_1 || front == MODE_UDP_N) {
			add_client_in_front(bin, index, client_index);
		}
		else if (front == MODE_UDP_REMOVE_ME) {
			add_client_udp_remove_me(bin, index, client_index);
		}

		int back = pipe_mode_list_[index].second;
		if (back == MODE_UDP_1 || back == MODE_UDP_N) {
			add_client_in_back(bin, index, client_index);
		}
	}
	logPipelineElements(pipeline, 0);
}

void MediaPipeline::request_remove_client(ContactInfo* client_info)
{
	message_mutex_.lock();
	client_info->new_client = false;
	client_queue_.push(*client_info);
	message_mutex_.unlock();
}

void MediaPipeline::remove_client(ContactInfo * client_info)
{
	GstState cur_state;
	gst_element_get_state(GST_ELEMENT(pipeline), &cur_state, NULL, 0);
	int client_index = get_client_index(client_info, false);
	if (client_index < 0) return;

	stop_state_pipeline(true);
	for (int bin_index = 0; bin_index < pipe_mode_list_.size(); bin_index++) {
		std::string name = "bin_" + std::to_string(bin_index);
		GstBin* bin = GST_BIN(gst_bin_get_by_name(GST_BIN(pipeline), name.c_str()));

		int front = pipe_mode_list_[bin_index].first;
		if (front == MODE_UDP_1 || front == MODE_UDP_N) {
			remove_client_in_front(bin, bin_index, client_index);
		}
		else if (front == MODE_UDP_REMOVE_ME) {
			remove_client_udp_remove_me(bin, bin_index, client_index);
		}

		int back = pipe_mode_list_[bin_index].second;
		if (back == MODE_UDP_1 || back == MODE_UDP_N) {
			remove_client_in_back(bin, bin_index, client_index);
		}
	}
	disable_client_index(client_info);
	logPipelineElements(pipeline, 0);
}

void MediaPipeline::requestSetVideoQuality(VideoQualityInfo* vq_info)
{
	message_mutex_.lock();
	video_quality_queue_.push(*vq_info);
	message_mutex_.unlock();
}

string MediaPipeline::getLinkedElements(GstElement* element)
{
#if 1
	string src_name = "";

	GstIterator* it = gst_element_iterate_src_pads(element);
	gboolean done = FALSE;
	GstPad* srcPad;
	GValue item = G_VALUE_INIT;

	while (!done) {
		switch (gst_iterator_next(it, &item)) {
		case GST_ITERATOR_OK:
			srcPad = (GstPad*)g_value_get_object(&item);
			if (srcPad && GST_PAD_IS_LINKED(srcPad)) {
				GstPad* linkedPad = gst_pad_get_peer(srcPad);
				GstElement* linkedElement = gst_pad_get_parent_element(linkedPad);
				if (linkedElement == NULL){
					src_name += "NoName";
				}
				else {
					src_name += gst_element_get_name(linkedElement);
				}
				src_name += ", ";
			}
			g_value_reset(&item);
			break;
		case GST_ITERATOR_RESYNC:
			gst_iterator_resync(it);
			break;
		case GST_ITERATOR_ERROR:
		case GST_ITERATOR_DONE:
			done = TRUE;
			break;
		}
	}

	g_value_unset(&item);
	gst_iterator_free(it);
	return src_name.length() > 2 ? src_name.substr(0, src_name.length()-2) : src_name;
#else
		GstPad* srcPad = gst_element_get_static_pad(element, "src");

		if (srcPad && GST_PAD_IS_LINKED(srcPad)) {
			GstPad* linkedPad = gst_pad_get_peer(srcPad);
			GstElement* linkedElement = gst_pad_get_parent_element(linkedPad);
			if (linkedElement == NULL) return "NoName";
			return gst_element_get_name(linkedElement);
		}
		return "";
#endif
}

// Function to recursively log the elements in the pipeline
void MediaPipeline::logPipelineElements(GstElement * element, int level) {
	if (!element) {
		return;
	}

	gchar* elementName = gst_element_get_name(element);
	//gchar* elementFactory = gst_element_factory_get_longname(gst_element_get_factory(element));
	string linkName = getLinkedElements(element);
	// Construct the log message with indentation to show hierarchy level
	gchar* logMessage = g_strdup_printf("%*s%s -> %s\n", level * 2, "", elementName, linkName.c_str());

	// Log the message using GStreamer log API
	g_printerr(logMessage);

	g_free(elementName);
	//g_free(elementFactory);
	g_free(logMessage);

	// Recursively log linked elements
	if (GST_IS_BIN(element)) {
		GstIterator* iter = gst_bin_iterate_elements(GST_BIN(element));
		if (iter) {
			GValue item = G_VALUE_INIT;
			while (gst_iterator_next(iter, &item) == GST_ITERATOR_OK) {
				GstElement* childElement = static_cast<GstElement*>(g_value_get_object(&item));
				logPipelineElements(childElement, level + 1);
				g_value_reset(&item);
			}
			//gst_iterator_free(iter);
		}
	}
}

GstBusSyncReply MediaPipeline::BusSyncHandler(GstBus* bus, GstMessage* message, gpointer data) {
	GstElement* pipeline = (GstElement*)data;

	switch (GST_MESSAGE_TYPE(message)) {
	case GST_MESSAGE_STATE_CHANGED:
		/* we only care about pipeline state change messages */
		if (GST_MESSAGE_SRC(message) == GST_OBJECT_CAST(pipeline)) {
			GstState old, news, pending;
			gchar* state_transition_name;

			gst_message_parse_state_changed(message, &old, &news, &pending);

			state_transition_name = g_strdup_printf("%s_%s",
				gst_element_state_get_name(old), gst_element_state_get_name(news));

			g_free(state_transition_name);
		}
		break;
	case GST_MESSAGE_ERROR: {
		GError* error = NULL;
		gchar* debug_info = NULL;

		gst_message_parse_error(message, &error, &debug_info);
		LOG_OBJ_ERROR() << "Error received from element " << GST_OBJECT_NAME(message->src)
			<< ": " << error->message << std::endl;
		LOG_OBJ_ERROR() << "Debugging information: " << (debug_info ? debug_info : "none") << std::endl;

		// Free resources
		g_error_free(error);
		g_free(debug_info);

		//g_main_loop_quit(loop);
		break;
	}
	default:
		break;
	}

	return GST_BUS_PASS;
}
bool MediaPipeline::BusHandler(GstBus* bus, GstMessage* message, gpointer data) {

	return true;
}


void MediaPipeline::set_pipe_block_flag(pipe_block_flag flag_type)
{
	pipe_block_flag_ |= 1 << (flag_type);
}

void MediaPipeline::unset_pipe_block_flag(pipe_block_flag flag_type)
{
	message_mutex_.lock();
	pipe_block_flag_ &= ~(1 << (flag_type));
	message_mutex_.unlock();
}

void MediaPipeline::stop_state_pipeline(bool stop)
{
	GstState cur_state;
	gst_element_get_state(GST_ELEMENT(pipeline), &cur_state, NULL, 0);
	if (stop) {
		if (pipe_block_flag_ && cur_state != GST_STATE_NULL) {
			GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_NULL);
		}
	}
	else {
		if (!pipe_block_flag_ && cur_state != GST_STATE_PLAYING) {
			GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
		}
	}
}

void MediaPipeline::checkMessageQueue(void) {
	bool queue_empty;

	if (!start_pipeline_) return;

	set_pipe_block_flag(BLOCK_MONITOR);
	while (true) {
		ContactInfo client_info;
		bool success = message_mutex_.try_lock();
		if (!success) break;
		queue_empty = client_queue_.empty();
		if (!queue_empty) {
			client_info = client_queue_.front();
			client_queue_.pop();
		}
		message_mutex_.unlock();
		if (queue_empty) break;
		if (client_info.new_client) {
			add_client(&client_info);
		}
		else {
			remove_client(&client_info);
		}
	}
	while (true) {
		VideoQualityInfo vq_info;
		bool success = message_mutex_.try_lock();
		if (!success) break;
		queue_empty = video_quality_queue_.empty();
		if (!queue_empty) {
			vq_info = video_quality_queue_.front();
			video_quality_queue_.pop();
		}
		message_mutex_.unlock();
		if (queue_empty) break;
		setVideoQuality(vq_info.video_quality_index);
	}

	unset_pipe_block_flag(BLOCK_MONITOR);
	stop_state_pipeline(false);
}

bool MediaPipeline::messageTask(gpointer data)
{
	static_cast<MediaPipeline*>(data)->checkMessageQueue();

	return true;
}

bool MediaPipeline::TimerTask(gpointer data) {
	MediaPipeline* pipeline = static_cast<MediaPipeline*>(data);

	pipeline->ReadAndNotifyRtpStats();

	return true;
}
void MediaPipeline::ReadAndNotifyRtpStats() {
	std::vector<GstElement*> rtp_jitter_buffers;
	rtp_jitter_buffers = get_elements_list(TYPE_JITTER);

	for_each(rtp_jitter_buffers.begin(), rtp_jitter_buffers.end(),
		[&](auto elem) {
			//guint64 num-pushed: the number of packets pushed out.
			//guint64 num-lost: the number of packets considered lost.
			//guint64 num-late : the number of packets arriving too late.
			//guint64 num-duplicates : the number of duplicate packets.
			//guint64 avg-jitter : the average jitter in nanoseconds.
			//guint64 rtx-count : the number of retransmissions requested.
			//guint64 rtx-success - count : the number of successful retransmissions.
			//gdouble rtx-per - packet : average number of RTX per packet.
			//guint64 rtx-rtt : average round trip time per RTX.

			if (!elem) return;

			RtpStats stats;
			GstStructure* s;
			g_object_get(elem, "stats", &s, NULL);
			gst_structure_get_uint64(s, "num-lost", &stats.num_lost);
			gst_structure_get_uint64(s, "num-late", &stats.num_late);
			gst_structure_get_uint64(s, "avg-jitter", &stats.avg_jitter_us);
			stats.avg_jitter_us = stats.avg_jitter_us / 1000;

			LOG_OBJ_LOG() << gst_element_get_name(elem) << ": lost " << stats.num_lost << ", late "
				<< stats.num_late << ", avg_jitter " << stats.avg_jitter_us << " us" << std::endl;

			VideoPresetType current_preset;
			if (monitor_cb_)
				monitor_cb_->OnRtpStats(current_preset, stats);
		});
}

} // namespace media