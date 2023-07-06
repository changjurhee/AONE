#include "VideoMediaPipeline.h"

VideoMediaPipeline::VideoMediaPipeline(int call_index, const vector<PipeMode>& pipe_mode_list) : MediaPipeline(call_index, pipe_mode_list) {
}

void VideoMediaPipeline::setVideoQuality(int video_quality_index)
{
	//TODO : 기능 구현하기
}

SubElements VideoMediaPipeline::pipeline_make_input_device(GstBin* parent_bin) {
	std::string name = "input_device";
	GstElement* element = gst_element_factory_make("mfvideosrc", name.c_str());
	g_object_set(element, "device-index", 0, NULL);
	gst_bin_add(GST_BIN(parent_bin), element);

	return SubElements(element, element);
};

SubElements VideoMediaPipeline::pipeline_make_output_device(GstBin* parent_bin) {
	std::string name = "output_device";
	GstElement* element = gst_element_factory_make("autovideosink", name.c_str());
	gst_bin_add(GST_BIN(parent_bin), element);

	return SubElements(element, element);
};

SubElements VideoMediaPipeline::pipeline_make_convert(GstBin* parent_bin, int bin_index, int client_index) {
#if 0
	std::string name = "convert_" + std::to_string(bin_index) + "_" + std::to_string(client_index);
	GstElement* element = gst_element_factory_make("videoconvert", name.c_str());
	gst_bin_add(GST_BIN(parent_bin), element);
	return SubElements(element, element);
#else
	return SubElements(NULL, NULL);
#endif
}

SubElements VideoMediaPipeline::pipeline_make_rescale(GstBin* parent_bin, int bin_index, int client_index, int level) {
	std::string name = "scale_"+std::to_string(bin_index)+"_"+std::to_string(client_index);
    GstElement* scale_element = gst_element_factory_make("videoscale", name.c_str());

	GstElement* caps_element = gst_element_factory_make("capsfilter", "videoCapsFilter");	

    // Set the video resolution using capsfilter
	// TODO : level에 따른 해상도 적용
    GstCaps* caps = gst_caps_new_simple("video/x-raw",
        "width", G_TYPE_INT, 1280,
        "height", G_TYPE_INT, 720,
        "framerate", GST_TYPE_FRACTION, 30, 1,
        NULL);
    g_object_set(caps_element, "caps", caps, NULL);
    gst_caps_unref(caps);
	gst_bin_add(GST_BIN(parent_bin), scale_element);
	gst_bin_add(GST_BIN(parent_bin), caps_element);
	gst_element_link(scale_element, caps_element);
	return SubElements(scale_element, caps_element);
};

SubElements VideoMediaPipeline::pipeline_make_encoding(GstBin* parent_bin, int bin_index, int client_index=0) {
	std::string encoding_name = "encoding_"+std::to_string(bin_index)+"_"+std::to_string(client_index);
    GstElement* encoding_element = gst_element_factory_make("x264enc", encoding_name.c_str());
	// TODO : 파라메터 추가검토
	//  g_object_set(encoding_element, 
		//"tune", H_264_TUNE_ZEROLATENCY,
		//"key-int-max", 30,
		//"bitrate", 1024
		//, NULL);
	g_object_set(encoding_element, "tune", H_264_TUNE_ZEROLATENCY, NULL);

	std::string rtp_name = "encoding_rtp_"+std::to_string(bin_index)+"_"+std::to_string(client_index);
    GstElement* rtp_element = gst_element_factory_make("rtph264pay", rtp_name.c_str());
	gst_bin_add(GST_BIN(parent_bin), encoding_element);
	gst_bin_add(GST_BIN(parent_bin), rtp_element);
	gst_element_link(encoding_element, rtp_element);
	return SubElements(encoding_element, rtp_element);
};

SubElements VideoMediaPipeline::pipeline_make_decoding(GstBin* parent_bin, int bin_index, int client_index) {
	std::string rtp_name = "decoding_rtp_"+std::to_string(bin_index)+"_"+std::to_string(client_index);
    GstElement* rtp_element = gst_element_factory_make("rtph264depay", rtp_name.c_str());

	std::string decoding_name = "decoding_"+std::to_string(bin_index)+"_"+std::to_string(client_index);
    GstElement* decoding_element = gst_element_factory_make("avdec_h264", decoding_name.c_str());
	gst_bin_add(GST_BIN(parent_bin), rtp_element);
	gst_bin_add(GST_BIN(parent_bin), decoding_element);
	gst_element_link(rtp_element, decoding_element);
	return SubElements(rtp_element, decoding_element);
};

SubElements VideoMediaPipeline::pipeline_make_adder(GstBin* parent_bin, int bin_index, int client_index) {
	std::string name = "adder_"+std::to_string(bin_index)+"_"+std::to_string(client_index);
    GstElement* element = gst_element_factory_make("compositor", name.c_str());
	g_printerr(("video"+name + "\n").c_str());

	//TODO : 해당 값 동적 적용 검토
    //g_object_set(G_OBJECT(element), "sink_0::xpos", 0, "sink_0::ypos", 0,
    //             "sink_0::width", 640, "sink_0::height", 480,
    //             "sink_1::xpos", 0, "sink_1::ypos", 480, "sink_1::width", 640, "sink_1::height", 480,
    //             "sink_2::xpos", 640, "sink_2::ypos", 0, "sink_2::width", 640, "sink_2::height", 480,
    //             "sink_3::xpos", 640, "sink_3::ypos", 480, "sink_3::width", 640, "sink_3::height", 480,
    //             NULL);

	gst_bin_add(GST_BIN(parent_bin), element);
	return SubElements(element, element);
};

SubElements VideoMediaPipeline::pipeline_make_jitter_buffer(GstBin* parent_bin, int bin_index, int client_index=0) {
	std::string name = "jitterbuffer_"+std::to_string(bin_index)+"_"+std::to_string(client_index);
    GstElement* element = gst_element_factory_make("rtpjitterbuffer", name.c_str());
    g_object_set(element, "latency", 500, "do-lost", TRUE, NULL);
	gst_bin_add(GST_BIN(parent_bin), element);
	return SubElements(element, element);
}


SubElements VideoMediaPipeline::pipeline_make_udp_sink(GstBin* parent_bin, int bin_index, int client_index) {
	return MediaPipeline::pipeline_make_udp_sink_with_port(parent_bin, bin_index, client_index, contact_info_list_[client_index].dest_video_port);
};

SubElements VideoMediaPipeline::pipeline_make_udp_src(GstBin* parent_bin, int bin_index, int client_index) {
	SubElements udpsrc_pair = MediaPipeline::pipeline_make_udp_src_with_port(parent_bin, bin_index, client_index, contact_info_list_[client_index].org_video_port);

	std::string name = "capsfilter_" + std::to_string(bin_index) + "_" + std::to_string(client_index);
	GstElement* videoCapsfilter = gst_element_factory_make("capsfilter", name.c_str());
	GstCaps* videoCaps = gst_caps_from_string("application/x-rtp, media=(string)video, payload=(int)96");
	g_object_set(G_OBJECT(videoCapsfilter), "caps", videoCaps, NULL);
	gst_caps_unref(videoCaps);

	gst_bin_add(GST_BIN(parent_bin), videoCapsfilter);
	gst_element_link(udpsrc_pair.second, videoCapsfilter);
	return SubElements(udpsrc_pair.first, videoCapsfilter);
};