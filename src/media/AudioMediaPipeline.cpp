#include "AudioMediaPipeline.h"

namespace media {

AudioMediaPipeline::AudioMediaPipeline(string rid, const vector<PipeMode>& pipe_mode_list, PipelineMonitorable::Callback* monitor_cb) :
	MediaPipeline(rid, pipe_mode_list, monitor_cb) {};

SubElements AudioMediaPipeline::pipeline_make_input_device(GstBin* parent_bin, int bin_index, int client_index)
{
	std::string name = get_elements_name(TYPE_INPUT_DEVICE, bin_index, client_index);
	GstElement* element = gst_element_factory_make("wasapi2src", name.c_str());
	g_object_set(element, "low-latency", TRUE, NULL);

	std::string cname = get_elements_name(TYPE_CONVERT, 1, client_index, "to1channel_for_vad");
	GstElement* celement = gst_element_factory_make("audioconvert", cname.c_str());
	
	std::string fname = get_elements_name(TYPE_UDP_CAPS, 1, client_index, "to1channel_for_vad");
	GstElement* audioCapsfilter = gst_element_factory_make("capsfilter", fname.c_str());
	GstCaps* audioCaps = gst_caps_from_string("audio/x-raw, channels=1");
	g_object_set(G_OBJECT(audioCapsfilter), "caps", audioCaps, NULL);
	gst_caps_unref(audioCaps);

	gst_bin_add(GST_BIN(parent_bin), element);
	gst_bin_add(GST_BIN(parent_bin), celement);
	gst_bin_add(GST_BIN(parent_bin), audioCapsfilter);
	gst_element_link(element, celement);
	gst_element_link(celement, audioCapsfilter);
	return SubElements(element, audioCapsfilter);
};


SubElements AudioMediaPipeline::pipeline_make_rescale(GstBin* parent_bin, int bin_index, int client_index, int level)
{
	std::string name = get_elements_name(TYPE_RESCALE, bin_index, client_index);
	GstElement* element = gst_element_factory_make("audioresample", name.c_str());
	gst_bin_add(GST_BIN(parent_bin), element);

	return SubElements(element, element);
};

void AudioMediaPipeline::setVideoQuality(int video_quality_index)
{
	return;
};

SubElements AudioMediaPipeline::pipeline_make_output_device(GstBin* parent_bin, int bin_index, int client_index)
{
	std::string name = get_elements_name(TYPE_OUTPUT_DEVICE, bin_index, client_index);
	GstElement* element =  gst_element_factory_make("wasapi2sink", name.c_str());
	g_object_set(element, "low-latency", TRUE, "sync", TRUE, NULL);
	gst_bin_add(GST_BIN(parent_bin), element);

	return SubElements(element, element);
};

SubElements AudioMediaPipeline::pipeline_make_convert(GstBin* parent_bin, int bin_index, int client_index) {
	std::string name = get_elements_name(TYPE_CONVERT, bin_index, client_index);
	GstElement* element = gst_element_factory_make("audioconvert", name.c_str());
	gst_bin_add(GST_BIN(parent_bin), element);
	return SubElements(element, element);
};

	SubElements AudioMediaPipeline::pipeline_make_encoding(GstBin* parent_bin, int bin_index, int client_index) {
	std::string encoding_name = get_elements_name(TYPE_ENCODING, bin_index, client_index);
    GstElement* encoding_element = gst_element_factory_make("opusenc", encoding_name.c_str());
	g_object_set(encoding_element, "audio-type", 2051, "frame-size", 10, NULL);
	// add probe for processing vad
	GstPad* sinkpad = gst_element_get_static_pad(encoding_element, "sink");
	gst_pad_add_probe(sinkpad, GST_PAD_PROBE_TYPE_BUFFER, (GstPadProbeCallback)
		AudioMediaPipeline::ProbeForProcessingVad, this, NULL);

	std::string rtp_name = get_elements_name(TYPE_ENCODING_RTP, bin_index, client_index);
    GstElement* rtp_element = gst_element_factory_make("rtpopuspay", rtp_name.c_str());

	gst_bin_add_many(GST_BIN(parent_bin),  encoding_element, rtp_element, NULL);
	gst_element_link(encoding_element, rtp_element);
	return make_pair(encoding_element, rtp_element);
};

	SubElements AudioMediaPipeline::pipeline_make_decoding(GstBin* parent_bin, int bin_index, int client_index) {
	std::string rtp_name = get_elements_name(TYPE_DECODING_RTP, bin_index, client_index);
	GstElement* rtp_element = gst_element_factory_make("rtpopusdepay", rtp_name.c_str());

	std::string decoding_name = get_elements_name(TYPE_DECODING, bin_index, client_index);
    GstElement* decoding_element = gst_element_factory_make("opusdec", decoding_name.c_str());
	g_object_set(decoding_element, "plc", TRUE, "min-latency", 5000000, NULL);

	gst_bin_add(GST_BIN(parent_bin), rtp_element);
	gst_bin_add(GST_BIN(parent_bin), decoding_element);
	gst_element_link(rtp_element, decoding_element);
	return make_pair(rtp_element, decoding_element);
};

	SubElements AudioMediaPipeline::pipeline_make_adder(GstBin* parent_bin, int bin_index, int client_index) {
	std::string name = get_elements_name(TYPE_ADDER, bin_index, client_index);
	GstElement* element = gst_element_factory_make("audiomixer", name.c_str());
	g_printerr(("adiuo" + name + "\n").c_str());

	gst_bin_add(GST_BIN(parent_bin), element);
	return make_pair(element, element);
};

	SubElements AudioMediaPipeline::pipeline_make_jitter_buffer(GstBin* parent_bin, int bin_index, int client_index) {
	std::string name = get_elements_name(TYPE_JITTER, bin_index, client_index);
	GstElement* element = gst_element_factory_make("rtpjitterbuffer", name.c_str());

    g_object_set(element, "latency", 80, "do-lost", TRUE, NULL);
	gst_bin_add(GST_BIN(parent_bin), element);
	return make_pair(element, element);
};

SubElements AudioMediaPipeline::pipeline_make_udp_sink(GstBin* parent_bin, int bin_index, int client_index) {
	return MediaPipeline::pipeline_make_udp_sink_with_port(parent_bin, bin_index, client_index, contact_info_list_[client_index].dest_audio_port);
};

SubElements AudioMediaPipeline::pipeline_make_udp_src(GstBin* parent_bin, int bin_index, int client_index) {
	SubElements udpsrc_pair = MediaPipeline::pipeline_make_udp_src_with_port(parent_bin, bin_index, client_index, contact_info_list_[client_index].org_audio_port);

	std::string name = get_elements_name(TYPE_UDP_CAPS, bin_index, client_index);
	GstElement* audioCapsfilter = gst_element_factory_make("capsfilter", name.c_str());
	GstCaps* audioCaps = 
		gst_caps_from_string("application/x-srtp, media=(string)audio, encoding-name=OPUS, payload=(int)96, ssrc=(uint)112233, srtp-key=(buffer)000000000000000000000000000000000000000000000000000000000000, srtp-cipher=(string)aes-128-icm, srtp-auth=(string)hmac-sha1-80, srtcp-cipher=(string)aes-128-icm, srtcp-auth=(string)hmac-sha1-80, roc=(uint)0");
	g_object_set(G_OBJECT(audioCapsfilter), "caps", audioCaps, NULL);
	gst_caps_unref(audioCaps);

	gst_bin_add(GST_BIN(parent_bin), audioCapsfilter);
	gst_element_link(udpsrc_pair.second, audioCapsfilter);
	return SubElements(udpsrc_pair.first, audioCapsfilter);
}



void AudioMediaPipeline::update_adder_parameter(GstBin* parent_bin, int bin_index, int client_index)
{
	return;
}

bool AudioMediaPipeline::ProcessVad(GstPadProbeInfo* info) {
	if (!monitor_cb_)
		return true;

	GstBuffer* buffer = GST_PAD_PROBE_INFO_BUFFER(info);
	GstMapInfo buffer_info;
	gst_buffer_map(buffer, &buffer_info, GST_MAP_READ);

	AudioBuffer audio_buffer(48000, 1, 16, (char*)buffer_info.data, buffer_info.size);
	return monitor_cb_->OnAudioBuffer(audio_buffer, 48000/100); // 48000/100 meand frames of 10 ms. same with frame-size of encoder.
}

} // namespace media