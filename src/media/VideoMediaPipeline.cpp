#include "VideoMediaPipeline.h"
#include <gst/video/videooverlay.h>

namespace media {

VideoMediaPipeline::VideoMediaPipeline(string rid, const vector<PipeMode>& pipe_mode_list, PipelineMonitorable::Callback* monitor_cb) :
	MediaPipeline(rid, pipe_mode_list, monitor_cb) {

}

void VideoMediaPipeline::setVideoQuality(int video_quality_index)
{
	//TODO : 기능 구현하기
	GstState cur_state;
	vector<GstElement*> element_rescale;
	vector<GstElement*> element_encode;
	GstElement* rescaleElements;
	GstElement* encodeElements;

	int videoWidth;
	int videoHeight;
	int videoBitRate;

	unset_pipe_block_flag(BLOCK_VIODE_STOP);
	// Set video resolution & bitrate
	switch(video_quality_index){
	default:
	case 0:
		// Stop pipeline for stop video transmit
		set_pipe_block_flag(BLOCK_VIODE_STOP);
		stop_state_pipeline(true);
		return;
		break;
	case 1:
		videoWidth = kVideoPresets.at(VideoPresetLevel::kVideoPreset1).width;
		videoHeight = kVideoPresets.at(VideoPresetLevel::kVideoPreset1).height;
		videoBitRate = kVideoPresets.at(VideoPresetLevel::kVideoPreset1).bitrate;
		break;
	case 2:
		videoWidth = kVideoPresets.at(VideoPresetLevel::kVideoPreset2).width;
		videoHeight = kVideoPresets.at(VideoPresetLevel::kVideoPreset2).height;
		videoBitRate = kVideoPresets.at(VideoPresetLevel::kVideoPreset2).bitrate;
		break;
	case 3:
		videoWidth = kVideoPresets.at(VideoPresetLevel::kVideoPreset3).width;
		videoHeight = kVideoPresets.at(VideoPresetLevel::kVideoPreset3).height;
		videoBitRate = kVideoPresets.at(VideoPresetLevel::kVideoPreset3).bitrate;
		break;
	case 4:
		videoWidth = kVideoPresets.at(VideoPresetLevel::kVideoPreset4).width;
		videoHeight = kVideoPresets.at(VideoPresetLevel::kVideoPreset4).height;
		videoBitRate = kVideoPresets.at(VideoPresetLevel::kVideoPreset4).bitrate;
		break;
	case 5:
		videoWidth = kVideoPresets.at(VideoPresetLevel::kVideoPreset5).width;
		videoHeight = kVideoPresets.at(VideoPresetLevel::kVideoPreset5).height;
		videoBitRate = kVideoPresets.at(VideoPresetLevel::kVideoPreset5).bitrate;
		break;
	}

	// Get rescale element
	element_rescale = get_elements_list(TYPE_RESCALE_CAPS);
	if (element_rescale.size() != 0) {
		rescaleElements = element_rescale[0];
	}
	else {
		return;
	}

	// Stop pipeline
	stop_state_pipeline(true);

	// Set the video resolution using capsfilter
	GstCaps* capsRescale = gst_caps_new_simple("video/x-raw",
		"width", G_TYPE_INT, videoWidth,
		"height", G_TYPE_INT, videoHeight,
		"framerate", GST_TYPE_FRACTION, 30, 1,
		NULL);

	g_object_set(rescaleElements, "caps", capsRescale, NULL);
	gst_caps_unref(capsRescale);

	// Get endcode element
	element_encode = get_elements_list(TYPE_ENCODING);
	if (element_encode.size() != 0) {
		encodeElements = element_encode[0];
	}
	else {
		return;
	}

	// Set the video bitrate
	g_object_set(encodeElements, "bitrate", videoBitRate, NULL);
}

SubElements VideoMediaPipeline::pipeline_make_input_device(GstBin* parent_bin, int bin_index, int client_index) {
	std::string name = get_elements_name(TYPE_INPUT_DEVICE, bin_index, client_index);
	GstElement* input_element = gst_element_factory_make("ksvideosrc", name.c_str());
	g_object_set(input_element, "device-index", 0, NULL);

	std::string caps_name = get_elements_name(TYPE_INPUT_CAPS, bin_index, client_index);
	GstElement* caps_element = gst_element_factory_make("capsfilter", caps_name.c_str());

	GstCaps* caps = gst_caps_new_simple("video/x-raw",
		"width", G_TYPE_INT, kVideoPresets.at(VideoPresetLevel::kVideoPreset5).width,
		"height", G_TYPE_INT, kVideoPresets.at(VideoPresetLevel::kVideoPreset5).height,
		"framerate", GST_TYPE_FRACTION, 30, 1,
		NULL);
	g_object_set(caps_element, "caps", caps, NULL);
	gst_caps_unref(caps);

	gst_bin_add(GST_BIN(parent_bin), input_element);
	gst_bin_add(GST_BIN(parent_bin), caps_element);
	gst_element_link(input_element, caps_element);
	return SubElements(input_element, caps_element);
};

SubElements VideoMediaPipeline::pipeline_make_output_device(GstBin* parent_bin, int bin_index, int client_index) {
	std::string name = get_elements_name(TYPE_OUTPUT_DEVICE, bin_index, client_index);
#if 0
	GstElement* element = gst_element_factory_make("autovideosink", name.c_str());
#else
	GstElement* element = gst_element_factory_make("d3dvideosink", name.c_str());

	// Set video sink
	g_object_set(G_OBJECT(element), "force-aspect-ratio", TRUE, "sync", FALSE, NULL);

	//GST_VIDEO_OVERLAY(element);
	gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(element), (guintptr) view_handler_);
#endif
	gst_bin_add(GST_BIN(parent_bin), element);


	return SubElements(element, element);
};

SubElements VideoMediaPipeline::pipeline_make_convert(GstBin* parent_bin, int bin_index, int client_index) {
	std::string name = get_elements_name(TYPE_CONVERT, bin_index, client_index);
	GstElement* element = gst_element_factory_make("videoconvert", name.c_str());
	gst_bin_add(GST_BIN(parent_bin), element);
	return SubElements(element, element);
}

SubElements VideoMediaPipeline::pipeline_make_rescale(GstBin* parent_bin, int bin_index, int client_index, int level) {
	std::string scale_name = get_elements_name(TYPE_RESCALE, bin_index, client_index);
	GstElement* scale_element = gst_element_factory_make("videoscale", scale_name.c_str());

	std::string caps_name = get_elements_name(TYPE_RESCALE_CAPS, bin_index, client_index);
	GstElement* caps_element = gst_element_factory_make("capsfilter", caps_name.c_str());

    // Set the video resolution using capsfilter
    GstCaps* caps = gst_caps_new_simple("video/x-raw",
        "width", G_TYPE_INT, kVideoPresets.at(VideoPresetLevel::kVideoPreset5).width,
        "height", G_TYPE_INT, kVideoPresets.at(VideoPresetLevel::kVideoPreset5).height,
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
	std::string encoding_name = get_elements_name(TYPE_ENCODING, bin_index, client_index);
    GstElement* encoding_element = gst_element_factory_make("x264enc", encoding_name.c_str());
	  g_object_set(encoding_element,
		"tune", H_264_TUNE_ZEROLATENCY,
		"key-int-max", 30,
		"bitrate", kVideoPresets.at(VideoPresetLevel::kVideoPreset5).bitrate
		, NULL);

	std::string rtp_name = get_elements_name(TYPE_ENCODING_RTP, bin_index, client_index);
    GstElement* rtp_element = gst_element_factory_make("rtph264pay", rtp_name.c_str());
	gst_bin_add(GST_BIN(parent_bin), encoding_element);
	gst_bin_add(GST_BIN(parent_bin), rtp_element);
	gst_element_link(encoding_element, rtp_element);
	return SubElements(encoding_element, rtp_element);
};

SubElements VideoMediaPipeline::pipeline_make_decoding(GstBin* parent_bin, int bin_index, int client_index) {
	std::string rtp_name = get_elements_name(TYPE_DECODING_RTP, bin_index, client_index);
    GstElement* rtp_element = gst_element_factory_make("rtph264depay", rtp_name.c_str());

	std::string decoding_name = get_elements_name(TYPE_DECODING, bin_index, client_index);
    GstElement* decoding_element = gst_element_factory_make("avdec_h264", decoding_name.c_str());
	gst_bin_add(GST_BIN(parent_bin), rtp_element);
	gst_bin_add(GST_BIN(parent_bin), decoding_element);
	gst_element_link(rtp_element, decoding_element);
	return SubElements(rtp_element, decoding_element);
};

SubElements VideoMediaPipeline::pipeline_make_adder(GstBin* parent_bin, int bin_index, int client_index) {
	std::string name = get_elements_name(TYPE_ADDER, bin_index, client_index);
    GstElement* element = gst_element_factory_make("compositor", name.c_str());

	gst_bin_add(GST_BIN(parent_bin), element);
	return SubElements(element, element);
};

SubElements VideoMediaPipeline::pipeline_make_jitter_buffer(GstBin* parent_bin, int bin_index, int client_index) {
	std::string name = get_elements_name(TYPE_JITTER, bin_index, client_index);
    GstElement* element = gst_element_factory_make("rtpjitterbuffer", name.c_str());

	// TODO : video jitter buffer 설정
    g_object_set(element, "latency", 500, "do-lost", TRUE, NULL);
	gst_bin_add(GST_BIN(parent_bin), element);
	return SubElements(element, element);
}

SubElements VideoMediaPipeline::pipeline_make_udp_sink(GstBin* parent_bin, int bin_index, int client_index) {
	return MediaPipeline::pipeline_make_udp_sink_with_port(parent_bin, bin_index, client_index, contact_info_list_[client_index].dest_video_port);
};

SubElements VideoMediaPipeline::pipeline_make_udp_src(GstBin* parent_bin, int bin_index, int client_index) {
	SubElements udpsrc_pair = MediaPipeline::pipeline_make_udp_src_with_port(parent_bin, bin_index, client_index, contact_info_list_[client_index].org_video_port);

	std::string name = get_elements_name(TYPE_UDP_CAPS, bin_index, client_index);
	GstElement* videoCapsfilter = gst_element_factory_make("capsfilter", name.c_str());
	GstCaps* videoCaps = gst_caps_from_string("application/x-rtp, media=(string)video, payload=(int)96");
	g_object_set(G_OBJECT(videoCapsfilter), "caps", videoCaps, NULL);
	gst_caps_unref(videoCaps);

	gst_bin_add(GST_BIN(parent_bin), videoCapsfilter);
	gst_element_link(udpsrc_pair.second, videoCapsfilter);
	return SubElements(udpsrc_pair.first, videoCapsfilter);
}

void VideoMediaPipeline::update_adder_parameter(GstBin* parent_bin, int bin_index, int client_index)
{
	int base_width = kVideoPresets.at(VideoPresetLevel::kVideoPreset5).height;
	int base_hight = kVideoPresets.at(VideoPresetLevel::kVideoPreset5).width;

	// get pad
	GstElement* queue = get_elements_by_name(parent_bin, TYPE_FQUEUE, bin_index, client_index).second;

	GstPad* srcPad = gst_element_get_static_pad(queue, "src");
	GstPad* linkedPad = gst_pad_get_peer(srcPad);

	int x_pos = base_width*(client_index/2);
	int y_pos = client_index%2==0 ? 0 : base_hight;

	g_object_set(linkedPad, "xpos", x_pos, NULL);
	g_object_set(linkedPad, "ypos", y_pos, NULL);

	gst_object_unref(linkedPad);
};

} // namespace media