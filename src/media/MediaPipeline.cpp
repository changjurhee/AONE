#include "MediaPipeline.h"
#include <thread>
#include <functional>
#include <algorithm>
#include <format>

#include "common/logger.h"

MediaPipeline::MediaPipeline(string rid, const vector<PipeMode>& pipe_mode_list) {
	rid_ = rid;
	pipe_mode_list_ = pipe_mode_list;
	start_pipeline_ = false;
	view_handler_ = NULL;

	bus_watch_id_ = 0;
	timer_id_ = 0;

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
		case TYPE_QUEUE :
			name = "queue_" + std::to_string(bin_index) + "_" + std::to_string(client_index);
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
			if (!iter->second.second)
				continue;
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

SubElements MediaPipeline::pipeline_make_queue(GstBin* parent_bin, int bin_index, int client_index) {
	std::string name = get_elements_name(TYPE_QUEUE, bin_index, client_index);
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
	g_object_set(element, "host", host.c_str(), "port", port, NULL);
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

SubElements MediaPipeline::make_front_udp_n(GstBin* parent_bin, int bin_index, int client_index) {
#if 1
	// TODO : Adder 미동작 수정

	SubElements ret_sub_elements = SubElements(NULL, NULL);
	SubElements adder_pair = pipeline_make_adder(parent_bin, bin_index, BASE_CLIENT_ID);
	ret_sub_elements = connect_subElements(ret_sub_elements, adder_pair);

	SubElements convert_pair = pipeline_make_convert(parent_bin, bin_index, client_index);
	ret_sub_elements = connect_subElements(ret_sub_elements, convert_pair);

	return ret_sub_elements;
#else
	return pipeline_make_convert(parent_bin, bin_index, client_index);
#endif
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

#if 1
	SubElements tee_pair = pipeline_make_tee(parent_bin, bin_index, client_index);
	ret_sub_elements = connect_subElements(ret_sub_elements, tee_pair);
#endif

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

		case MODE_UDP_REMOVE_ME:
			// TODO
			break;
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

	start_pipeline_ = true;
	add_waiting_client();

	LOG_INFO("Setting handler on bus");
	GstBus* bus = gst_element_get_bus(pipeline);
	gst_bus_set_sync_handler(bus, BusSyncHandlerCallback, (gpointer)this, NULL);
	bus_watch_id_ = gst_bus_add_watch(bus, (GstBusFunc)BusHandlerCallback, (gpointer)this);
	gst_object_unref(bus);

	LOG_INFO("Setting timer (500 ms)");
	g_timeout_add(500, (GSourceFunc)TimerTask, (gpointer)this);

    // Pipeline execution
	LOG_INFO("Setting pipeline to PLAYING");
    GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        g_printerr("Sender : Unable to start the pipeline.\n");
        gst_object_unref(pipeline);
        return;
    }

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
	// TODO : 동작 확인하기
	// Release the pipeline
	gst_element_set_state(pipeline, GST_STATE_NULL);
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

void MediaPipeline::add_client_in_front(GstBin* parent_bin, int bin_index, int client_index)
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

	SubElements queue_pair = pipeline_make_queue(parent_bin, bin_index, client_index);
	ret_sub_elements = connect_subElements(ret_sub_elements, queue_pair);

	SubElements adder = get_elements_by_name(parent_bin, TYPE_ADDER, bin_index, BASE_CLIENT_ID);
	ret_sub_elements = connect_subElements(ret_sub_elements, adder);
	
	update_adder_parameter(parent_bin, bin_index, client_index);
	return;
}

void MediaPipeline::add_client_in_back(GstBin* parent_bin, int bin_index, int client_index)
{
	SubElements ret_sub_elements = SubElements(NULL, NULL);

	SubElements tee = get_elements_by_name(parent_bin, TYPE_TEE, bin_index, BASE_CLIENT_ID);
	ret_sub_elements = connect_subElements(ret_sub_elements, tee);

	SubElements queue_pair = pipeline_make_queue(parent_bin, bin_index, client_index+20);
	ret_sub_elements = connect_subElements(ret_sub_elements, queue_pair);

	SubElements udp_sink_pair = pipeline_make_udp_sink(parent_bin, bin_index, client_index);
	ret_sub_elements = connect_subElements(ret_sub_elements, udp_sink_pair);

	return;
}

void MediaPipeline::unref_element(GstBin* parent_bin, GstElement* element)
{
#if 1
	gst_bin_remove(GST_BIN(parent_bin), element);
	//TODO : 동작 확인 필요
	//while (GST_OBJECT_REFCOUNT(GST_OBJECT(element)) != 0)
	//	gst_object_unref(element);
#endif
}

void MediaPipeline::remove_client_in_front(GstBin* parent_bin, int bin_index, int client_index)
{
	std::string current_name = "udpsrc_"+std::to_string(bin_index)+"_"+std::to_string(client_index);
	std::string target_name = "adder_"+std::to_string(bin_index)+"_"+std::to_string(BASE_CLIENT_ID);
	while (current_name != target_name) {
		// Determine the element linked to 'source'
		GstElement* current_element = gst_bin_get_by_name(parent_bin, current_name.c_str());

		GstPad *srcPad = gst_element_get_static_pad(current_element, "src");
		GstPad *linkedPad = gst_pad_get_peer(srcPad);
		GstElement *linkedElement = gst_pad_get_parent_element(linkedPad);
		if (linkedElement == NULL) break;

		gst_element_unlink(current_element, linkedElement);
		gst_object_unref(srcPad);
		gst_object_unref(linkedPad);
		unref_element(parent_bin, current_element);
		current_name = gst_element_get_name(linkedElement);
	}
}

void MediaPipeline::remove_client_in_back(GstBin* parent_bin, int bin_index, int client_index)
{
	std::string current_name = "udpsink_"+std::to_string(bin_index)+"_"+std::to_string(client_index);
	std::string target_name = "tee_"+std::to_string(bin_index)+"_"+std::to_string(BASE_CLIENT_ID);
	while (current_name != target_name) {
		// Determine the element linked to 'source'
		GstElement* current_element = gst_bin_get_by_name(GST_BIN(parent_bin), current_name.c_str());

		GstPad *sinkPad = gst_element_get_static_pad(current_element, "sink");
		GstPad *linkedPad = gst_pad_get_peer(sinkPad);
		GstElement *linkedElement = gst_pad_get_parent_element(linkedPad);
		if (linkedElement == NULL) break;

		gst_element_unlink(current_element, linkedElement);
		gst_object_unref(sinkPad);
		gst_object_unref(linkedPad);
		unref_element(parent_bin, current_element);
		current_name = gst_element_get_name(linkedElement);
	}
}

void MediaPipeline::add_waiting_client(void)
{
	while (!client_queue_.empty())
	{
		ContactInfo client_info = client_queue_.front();

		add_client(&client_info);
		client_queue_.pop();
	}
}

bool MediaPipeline::check_pipeline(ContactInfo* client_info)
{
	if (start_pipeline_)
		return false;
	client_queue_.push(*client_info);
	return true;
}

void MediaPipeline::add_client(ContactInfo* client_info)
{
	if (check_pipeline(client_info))
		return;

	int client_index = get_client_index(client_info, true);
	if (client_index < 0) return;

	GstState cur_state;
	gst_element_get_state(GST_ELEMENT(pipeline), &cur_state, NULL, 0);
	if (cur_state != GST_STATE_NULL) {
		gst_element_set_state(pipeline, GST_STATE_NULL);
	}

	for (int index = 0; index < pipe_mode_list_.size(); index++) {
		std::string name = "bin_"+std::to_string(index);
		GstBin *bin = GST_BIN(gst_bin_get_by_name(GST_BIN(pipeline), name.c_str()));

		int front = pipe_mode_list_[index].first;
		if (front == MODE_UDP_1 || front == MODE_UDP_N) {
			add_client_in_front(bin, index, client_index);
		}

		int back = pipe_mode_list_[index].second;
		if (back == MODE_UDP_1 || back == MODE_UDP_N) {
			add_client_in_back(bin, index, client_index);
		}
	}
	// Pipeline replay(?)
	if (cur_state != GST_STATE_PLAYING) {
		gst_element_set_state(pipeline, GST_STATE_PLAYING);
	}
	logPipelineElements(pipeline, 0);

}

void MediaPipeline::remove_client(ContactInfo* client_info)
{
	GstState cur_state;
	gst_element_get_state(GST_ELEMENT(pipeline), &cur_state, NULL, 0);
	int client_index = get_client_index(client_info, false);
	if (client_index < 0) return;

	if (cur_state != GST_STATE_NULL) {
		GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_NULL);
	}

	for (int bin_index = 0; bin_index < pipe_mode_list_.size(); bin_index++) {
		std::string name = "bin_" + std::to_string(bin_index);
		GstBin* bin = GST_BIN(gst_bin_get_by_name(GST_BIN(pipeline), name.c_str()));

		int front = pipe_mode_list_[bin_index].first;
		if (front == MODE_UDP_1 || front == MODE_UDP_N) {
			remove_client_in_front(bin, bin_index, client_index);
		}

		int back = pipe_mode_list_[bin_index].second;
		if (back == MODE_UDP_1 || back == MODE_UDP_N) {
			remove_client_in_back(bin, bin_index, client_index);
		}
	}
	disable_client_index(client_info);
	// Pipeline replay(?)
	if (cur_state != GST_STATE_PLAYING) {
		GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
	}
}

string MediaPipeline::getLinkedElements(GstElement* element)
{
		GstPad* srcPad = gst_element_get_static_pad(element, "src");

		if (srcPad && GST_PAD_IS_LINKED(srcPad)) {
			GstPad* linkedPad = gst_pad_get_peer(srcPad);
			GstElement* linkedElement = gst_pad_get_parent_element(linkedPad);
			if (linkedElement == NULL) return "NoName";
			return gst_element_get_name(linkedElement);
		}
		return "";
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
	gchar* logMessage = g_strdup_printf("%*s%s ->(%s)\n", level * 2, "", elementName, linkName.c_str());

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

bool MediaPipeline::TimerTask(gpointer data) {
	static_cast<MediaPipeline*>(data)->ReadAndNotifyRtpStats();

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

			guint64 lost, late, avg_jitter;
			GstStructure* s;
			g_object_get(elem, "stats", &s, NULL);
			gst_structure_get_uint64(s, "num-lost", &lost);
			gst_structure_get_uint64(s, "num-late", &late);
			gst_structure_get_uint64(s, "avg-jitter", &avg_jitter);

			LOG_OBJ_DEBUG() << gst_element_get_name(elem) << ": lost " << lost << ", late " << late
				<< ", avg_jitter " << avg_jitter/1000 << " us" << std::endl;
		});
}