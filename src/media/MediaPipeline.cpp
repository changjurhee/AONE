#include "MediaPipeline.h"
#include <thread>

MediaPipeline::MediaPipeline(int call_index, const vector<PipeMode>& pipe_mode_list) {
	call_index_ = call_index;
	pipe_mode_list_ = pipe_mode_list;

	vector<ContactInfo> contact_info_vector;
	contact_info_list_ = contact_info_vector;

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
			name = "input_device";
			break;
		case TYPE_OUTPUT_DEVICE :
			name = "output_device";
			break;
		case TYPE_CONVERT :
			name = "convert_" + std::to_string(bin_index) + "_" + std::to_string(client_index);
			break;
		case TYPE_RESCALE :
			name = "scale_" + std::to_string(bin_index) + "_" + std::to_string(client_index);
			break;
		case TYPE_CAPS:
			name = "caps_" + std::to_string(bin_index) + "_" + std::to_string(client_index);
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
			name = "udpsrc_" + std::to_string(bin_index) + "_" + std::to_string(client_index);
		case TYPE_UDP_SRC :
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

SubElements MediaPipeline::get_elements_by_name(GstBin* parent_bin, element_type etype, int bin_index, int client_index) {
	string name = get_elements_name(etype, bin_index, client_index);
	GstElement* element = gst_bin_get_by_name(GST_BIN(parent_bin), name.c_str());
	return SubElements(element, element);
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
#if 0
	// TODO : Adder 미동작 수정
	GstElement* start = NULL;
	GstElement* last = NULL;
	SubElements adder_pair = pipeline_make_adder(parent_bin, bin_index, client_index);

	start = adder_pair.first;
	last = adder_pair.second;
	SubElements convert_pair = pipeline_make_convert(parent_bin, bin_index, client_index);
	if (convert_pair.first != NULL) {
		gst_element_link(last, convert_pair.first);
		last = convert_pair.second;
	}

	return SubElements(start, last); 
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
#if 0 
	SubElements tee_pair = pipeline_make_tee(parent_bin, bin_index, client_index);
	if (tee_pair.first != NULL) {
		gst_element_link(last, tee_pair.first);
		last = tee_pair.second;
	}
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

void enable_debugging() {
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

	add_client(&contact_info_list_[0]);

    // Pipeline execution
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
	GMainLoop* gSenderLoop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(gSenderLoop);

    // Release the pipeline
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
}


void MediaPipeline::makePipeline(vector<ContactInfo> &contact_info_list, OperatingInfo& operate_info) {
	contact_info_list_ = contact_info_list;
	memcpy(&operate_info_, &operate_info, sizeof(OperatingInfo));

	pipeline_thread_ = std::thread(&MediaPipeline::pipeline_run, this);
}

void MediaPipeline::end_call()
{
	// TODO : 동작 확인하기
	// Release the pipeline
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
}

int MediaPipeline::get_client_index(ContactInfo* client_info) {
	return 0;
};

void MediaPipeline::add_client_in_front(GstBin* parent_bin, int bin_index, int client_index) {
#if 0
	// TODO : adder 미동작 수정
	std::string name = "adder_"+std::to_string(bin_index)+"_"+std::to_string(BASE_CLIENT_ID);
	GstElement* adder = gst_bin_get_by_name(GST_BIN(parent_bin), name.c_str());
#endif
	SubElements ret_sub_elements = SubElements(NULL, NULL);

	SubElements udp_src_pair = pipeline_make_udp_src(parent_bin, bin_index, client_index);
	ret_sub_elements = connect_subElements(ret_sub_elements, udp_src_pair);

	SubElements jitter_pair = pipeline_make_jitter_buffer(parent_bin, bin_index, client_index);
	ret_sub_elements = connect_subElements(ret_sub_elements, jitter_pair);

	SubElements restoration_pair = pipeline_make_restoration(parent_bin, bin_index, client_index);
	ret_sub_elements = connect_subElements(ret_sub_elements, restoration_pair);

	SubElements decoding_pair = pipeline_make_decoding(parent_bin, bin_index, client_index);
	ret_sub_elements = connect_subElements(ret_sub_elements, decoding_pair);

	SubElements convert_pair = pipeline_make_convert(parent_bin, bin_index, client_index);
	ret_sub_elements = connect_subElements(ret_sub_elements, convert_pair);

	// TODO : QUEUE 미동작 수정
	//SubElements queue_pair = pipeline_make_queue(parent_bin, bin_index, client_index);
	//ret_sub_elements = connect_subElements(ret_sub_elements, queue_pair);

	SubElements adder = get_elements_by_name(parent_bin, TYPE_OUTPUT_DEVICE, bin_index, BASE_CLIENT_ID);
	ret_sub_elements = connect_subElements(ret_sub_elements, adder);

	return;
}

void MediaPipeline::add_client_in_back(GstBin* parent_bin, int bin_index, int client_index) {

#if 0
	std::string name = "tee_" + std::to_string(bin_index) + "_" + std::to_string(BASE_CLIENT_ID);
	GstElement* tee = gst_bin_get_by_name(GST_BIN(parent_bin), name.c_str());

	GstElement* start = NULL;
	GstElement* last = NULL;
	SubElements queue_pair = pipeline_make_queue(parent_bin, bin_index, client_index);
	start = queue_pair.first;
	last = queue_pair.second;
	SubElements udp_sink_pair = pipeline_make_udp_sink(parent_bin, bin_index, client_index);
	if (udp_sink_pair.first != NULL) {
		gst_element_link(last, udp_sink_pair.first);
		last = udp_sink_pair.second;
	}
	gst_element_link(tee, start);
#endif
#if 1
	std::string name = "encoding_rtp_" + std::to_string(bin_index) + "_" + std::to_string(BASE_CLIENT_ID);
	GstElement* rtp = gst_bin_get_by_name(GST_BIN(parent_bin), name.c_str());
	SubElements udp_sink_pair = pipeline_make_udp_sink(parent_bin, bin_index, client_index);

	gst_element_link(rtp, udp_sink_pair.first);
	g_printerr(("After tee : " + getLinkedElements(rtp) + "\n").c_str());

#endif
	return;
}

void MediaPipeline::remove_client_in_front(GstBin* parent_bin, int bin_index, int client_index) {
	std::string current_name = "udpsrc_"+std::to_string(bin_index)+"_"+std::to_string(client_index);
	std::string target_name = "adder_"+std::to_string(bin_index)+"_"+std::to_string(client_index);
	while (true) {
		// Determine the element linked to 'source'
		GstElement* current_element = gst_bin_get_by_name(parent_bin, current_name.c_str());

		GstPad *srcPad = gst_element_get_static_pad(current_element, "src");
		GstPad *linkedPad = gst_pad_get_peer(srcPad);
		GstElement *linkedElement = gst_pad_get_parent_element(linkedPad);
		gst_object_unref(srcPad);
		gst_object_unref(linkedPad);
		gst_object_unref(current_element);
		if (linkedElement == NULL || gst_element_get_name(linkedElement) == target_name) {
			break;
		}
		current_name = gst_element_get_name(linkedElement);
	}
}

void MediaPipeline::remove_client_in_back(GstBin* parent_bin, int bin_index, int client_index) {
	std::string current_name = "udpsink_"+std::to_string(bin_index)+"_"+std::to_string(client_index);
	std::string target_name = "tee_"+std::to_string(bin_index)+"_"+std::to_string(client_index);
	while (true) {
		// Determine the element linked to 'source'
		GstElement* current_element = gst_bin_get_by_name(GST_BIN(parent_bin), current_name.c_str());

		GstPad *sinkPad = gst_element_get_static_pad(current_element, "sink");
		GstPad *linkedPad = gst_pad_get_peer(sinkPad);
		GstElement *linkedElement = gst_pad_get_parent_element(linkedPad);
		gst_object_unref(sinkPad);
		gst_object_unref(linkedPad);
		gst_object_unref(current_element);
		if (linkedElement == NULL || gst_element_get_name(linkedElement) == target_name) {
			break;
		}
		current_name = gst_element_get_name(linkedElement);
	}
}

void MediaPipeline::add_client(ContactInfo* client_info) {
	int client_index = get_client_index(client_info);
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
}

void MediaPipeline::remove_client(ContactInfo* client_info) {
	int client_index = get_client_index(client_info);
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