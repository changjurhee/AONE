#include "AudioMediaPipeline.h"

AudioMediaPipeline::AudioMediaPipeline(int call_index, const vector<PipeMode>& pipe_mode_list) : MediaPipeline(call_index, pipe_mode_list) {};

SubElements AudioMediaPipeline::pipeline_make_input_device(GstBin* parent_bin) {
	std::string name = "input_device";
	GstElement* element = gst_element_factory_make("autoaudiosrc", name.c_str());
	gst_bin_add(GST_BIN(parent_bin), element);

	return SubElements(element, element);
};


SubElements AudioMediaPipeline::pipeline_make_rescale(GstBin* parent_bin, int bin_index, int client_index, int level) {
	return SubElements(NULL, NULL);
};

void AudioMediaPipeline::setVideoQuality(int video_quality_index)
{
	return;
};

SubElements AudioMediaPipeline::pipeline_make_output_device(GstBin* parent_bin) {
	std::string name = "output_device";
	GstElement* element =  gst_element_factory_make("autoaudiosink", name.c_str());
	gst_bin_add(GST_BIN(parent_bin), element);

	return SubElements(element, element);
};

SubElements AudioMediaPipeline::pipeline_make_convert(GstBin* parent_bin, int bin_index, int client_index) {
	std::string name = "convert_" + std::to_string(bin_index) + "_" + std::to_string(client_index);
	GstElement* element = gst_element_factory_make("audioconvert", name.c_str());	
	gst_bin_add(GST_BIN(parent_bin), element);
	return SubElements(element, element);
};

SubElements AudioMediaPipeline::pipeline_make_encoding(GstBin* parent_bin, int bin_index, int client_index=0) {
	std::string resampling_name = "resampling_name_"+std::to_string(bin_index)+"_"+std::to_string(client_index);
    GstElement* resampling_element = gst_element_factory_make("audioresample", resampling_name.c_str());

	std::string encoding_name = "encoding_"+std::to_string(bin_index)+"_"+std::to_string(client_index);
    GstElement* encoding_element = gst_element_factory_make("opusenc", encoding_name.c_str());
    g_object_set(encoding_element, "audio-type", 2051, NULL);

	std::string rtp_name = "encoding_rtp_"+std::to_string(bin_index)+"_"+std::to_string(client_index);
    GstElement* rtp_element = gst_element_factory_make("rtpopuspay", rtp_name.c_str());

	gst_bin_add_many(GST_BIN(parent_bin), resampling_element, encoding_element, rtp_element, NULL);
	gst_element_link_many(resampling_element, encoding_element, rtp_element, NULL);
	return make_pair(resampling_element, rtp_element); 	
};

SubElements AudioMediaPipeline::pipeline_make_decoding(GstBin* parent_bin, int bin_index, int client_index=0) {
	std::string rtp_name = "decoding_rtp_"+std::to_string(bin_index)+"_"+std::to_string(client_index);
    GstElement* rtp_element = gst_element_factory_make("rtpopusdepay", rtp_name.c_str());

	std::string decoding_name = "decoding_"+std::to_string(bin_index)+"_"+std::to_string(client_index);
    GstElement* decoding_element = gst_element_factory_make("opusdec", decoding_name.c_str());
	gst_bin_add(GST_BIN(parent_bin), rtp_element);
	gst_bin_add(GST_BIN(parent_bin), decoding_element);
	gst_element_link(rtp_element, decoding_element);
	return make_pair(rtp_element, decoding_element);
};

SubElements AudioMediaPipeline::pipeline_make_adder(GstBin* parent_bin, int bin_index, int client_index=0) {
	std::string name = "adder_"+std::to_string(bin_index)+"_"+std::to_string(client_index);
    GstElement* element = gst_element_factory_make("audiomixer", name.c_str());
	g_printerr(("adiuo" + name + "\n").c_str());

	gst_bin_add(GST_BIN(parent_bin), element);
	return make_pair(element, element);
};

SubElements AudioMediaPipeline::pipeline_make_jitter_buffer(GstBin* parent_bin, int bin_index, int client_index=0) {
	std::string name = "jitterbuffer_"+std::to_string(bin_index)+"_"+std::to_string(client_index);
    GstElement* element = gst_element_factory_make("rtpjitterbuffer", name.c_str());

    g_object_set(element, "latency", 500, "do-lost", TRUE, NULL);
	gst_bin_add(GST_BIN(parent_bin), element);
	return make_pair(element, element);
};

SubElements AudioMediaPipeline::pipeline_make_udp_sink(GstBin* parent_bin, int bin_index, int client_index) {
	return MediaPipeline::pipeline_make_udp_sink_with_port(parent_bin, bin_index, client_index, contact_info_list_[client_index].dest_audio_port);
};

SubElements AudioMediaPipeline::pipeline_make_udp_src(GstBin* parent_bin, int bin_index, int client_index) {
	SubElements udpsrc_pair = MediaPipeline::pipeline_make_udp_src_with_port(parent_bin, bin_index, client_index, contact_info_list_[client_index].org_video_port);

	std::string name = "capsfilter_" + std::to_string(bin_index) + "_" + std::to_string(client_index);
	GstElement* audioCapsfilter = gst_element_factory_make("capsfilter", name.c_str());
	GstCaps* audioCaps = gst_caps_from_string("application/x-rtp, media=(string)audio, encoding-name=OPUS,  payload=(int)96");
	g_object_set(G_OBJECT(audioCapsfilter), "caps", audioCaps, NULL);
	gst_caps_unref(audioCaps);

	gst_bin_add(GST_BIN(parent_bin), audioCapsfilter);
	gst_element_link(udpsrc_pair.second, audioCapsfilter);
	return SubElements(udpsrc_pair.first, audioCapsfilter);
};