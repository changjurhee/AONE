#include <gst/gst.h>
#include <gst/video/videooverlay.h>
#include "recvMain.h"

// Create GStreamer loop
GMainLoop* gReceiverLoop;

// Create GStreamer pipeline for video
GstElement* gReceiveVideoPipeline;

// Create GStreamer pipeline elements for video
GstElement* gReceiveVideoSrc;
GstElement* gReceiveVideoSRTPCaps;
GstElement* gReceiveVideoJitterbuffer;
GstElement* gReceiveVideoRTPH264Depay;
GstElement* gReceiveVideoH264Dec;
GstElement* gReceiveVideoSink;

// Create GStreamer pipeline for audio
GstElement* gReceiveAudioPipeline;

// Create GStreamer pipeline elements for audio
GstElement* gReceiveAudioSrc;
GstElement* gReceiveAudioCapsfilter;
GstElement* gReceiveAudioJitterbuffer;
GstElement* gReceiveAudioDec;
GstElement* gReceiveAudioDepay;
GstElement* gReceiveAudioConv;
GstElement* gReceiveAudioSink;

// Create GStreamer pipeline elements for decryption
GstElement* gReceiveVideoSRTPDec;
GstElement* gReceiveAudioSRTPDec;

// define callback function
static gboolean on_video_bus_message(GstBus* bus, GstMessage* const message, gpointer user_data) {
    switch (GST_MESSAGE_TYPE(message)) {
    case GST_MESSAGE_ELEMENT: {
        if (gst_message_has_name(message, "jitterbuffer")) {
            const GstStructure* stats = gst_message_get_structure(message);
            if (stats != NULL && gst_structure_has_field(stats, "jitter")) {
                guint64 jitter;
                gst_structure_get_uint64(stats, "jitter", &jitter);
                g_print("Receiver : Jitter: %" G_GUINT64_FORMAT " ns\n", jitter);
            }
        }
        else {
            const GstStructure* structure = gst_message_get_structure(message);
            if (structure != NULL) {
                const gchar* name = gst_structure_get_name(structure);
                if (g_strcmp0(name, "authentication-failure") == 0) {
                    // SRTP 인증 실패 처리
                    g_print("Receiver: SRTP authentication failure!\n");

                    // 패킷 드롭 등 추가 동작 수행
                }
            }
        }
        break;
    }
    default:
        break;
    }
    return TRUE;
}

// define callback function
static gboolean on_audio_bus_message(GstBus* bus, GstMessage* const message, gpointer user_data) {
    g_print("Receiver: Callback!\n");
    return FALSE;
}

int recvMain(HWND hwnd)
{
    g_printerr("Receiver : Start recvMain thread\n");

    // Create GStreamer pipeline for video
    gReceiveVideoPipeline = gst_pipeline_new("videoReceiver_pipeline");

    // Create GStreamer pipeline elements for video
    gReceiveVideoSrc = gst_element_factory_make("udpsrc", "videoSrc");
    gReceiveVideoSRTPCaps = gst_element_factory_make("capsfilter", "videoSRTPCaps");
    gReceiveVideoJitterbuffer = gst_element_factory_make("rtpjitterbuffer", "videoJitterbuffer");
    gReceiveVideoRTPH264Depay = gst_element_factory_make("rtph264depay", "videoDepay");
    gReceiveVideoH264Dec = gst_element_factory_make("avdec_h264", "videoDec");
    gReceiveVideoSink = gst_element_factory_make("d3dvideosink", "videoSink");

    // Create GStreamer pipeline for audio
    gReceiveAudioPipeline = gst_pipeline_new("audioReceiver_pipeline");

    // Create GStreamer pipeline elements for audio
    gReceiveAudioSrc = gst_element_factory_make("udpsrc", "audioSrc");
    gReceiveAudioCapsfilter = gst_element_factory_make("capsfilter", "audioCapsfilter");
    gReceiveAudioJitterbuffer = gst_element_factory_make("rtpjitterbuffer", "audioJitterbuffer");
    gReceiveAudioDec = gst_element_factory_make("opusdec", "audioDec");
    gReceiveAudioDepay = gst_element_factory_make("rtpopusdepay", "audioDepay");
    gReceiveAudioConv = gst_element_factory_make("audioconvert", "audioConv");
    gReceiveAudioSink = gst_element_factory_make("autoaudiosink", "audioSink");

    // Create GStreamer pipeline elements for encryption
    gReceiveVideoSRTPDec = gst_element_factory_make("srtpdec", "videoDecrypt");
    gReceiveAudioSRTPDec = gst_element_factory_make("srtpdec", "audioDecrypt");

    // set up video bus and callback function for jitter statistics
    GstBus* videoBus = gst_element_get_bus(gReceiveVideoPipeline);
    gst_bus_add_watch(videoBus, on_video_bus_message, NULL);
    gst_object_unref(videoBus);

    // set up audio bus and callback function to check whether an audio packet has been received
    GstBus* audioBus = gst_element_get_bus(gReceiveAudioPipeline);
    gst_bus_add_watch(audioBus, on_audio_bus_message, NULL);
    gst_object_unref(audioBus);

    // Add element to pipeline
    gst_bin_add_many(GST_BIN(gReceiveVideoPipeline),
        gReceiveVideoSrc, gReceiveVideoSRTPCaps, gReceiveVideoSRTPDec, gReceiveVideoJitterbuffer, gReceiveVideoRTPH264Depay, gReceiveVideoH264Dec, gReceiveVideoSink,
        NULL);
    gst_bin_add_many(GST_BIN(gReceiveAudioPipeline),
        gReceiveAudioSrc, gReceiveAudioCapsfilter, gReceiveAudioJitterbuffer, gReceiveAudioDec, gReceiveAudioDepay, gReceiveAudioConv, gReceiveAudioSink,
        NULL);

    // linking elements
    gst_element_link_many(gReceiveVideoSrc, gReceiveVideoSRTPCaps, gReceiveVideoSRTPDec, gReceiveVideoJitterbuffer, gReceiveVideoRTPH264Depay, gReceiveVideoH264Dec, gReceiveVideoSink,
        NULL);
    gst_element_link_many(gReceiveAudioSrc, gReceiveAudioCapsfilter, gReceiveAudioJitterbuffer, gReceiveAudioDepay, gReceiveAudioDec, gReceiveAudioConv, gReceiveAudioSink,
        NULL);

    // Receive port setting for video
    g_object_set(gReceiveVideoSrc, "port", 5001, NULL);
    g_object_set(gReceiveAudioSrc, "port", 5002, NULL);

    // RTP video format (caps) settings
    GstCaps* videoCaps = 
        gst_caps_from_string("application/x-srtp, encoding-name=(string)H264, payload=(int)96, ssrc=(uint)1356955624, srtp-key=(buffer)012345678901234567890123456789, srtp-cipher=(string)aes-128-icm, srtp-auth=(string)hmac-sha1-80, srtcp-cipher=(string)aes-128-icm, srtcp-auth=(string)hmac-sha1-80, roc=(uint)0");
    g_object_set(G_OBJECT(gReceiveVideoSRTPCaps), "caps", videoCaps, NULL);
    gst_caps_unref(videoCaps);
  
    // RTP audio format (caps) settings
    GstCaps* audioCaps = gst_caps_from_string("application/x-rtp, media=(string)audio, encoding-name=OPUS,  payload=(int)96");
    g_object_set(G_OBJECT(gReceiveAudioCapsfilter), "caps", audioCaps, NULL);
    gst_caps_unref(audioCaps);

    // Set RTP jitter-buffer latency
    g_object_set(gReceiveVideoJitterbuffer, "latency", 0, "do-lost", TRUE, NULL);
    g_object_set(gReceiveAudioJitterbuffer, "latency", 0, "do-lost", TRUE, NULL);

    // Set video sink
    g_object_set(G_OBJECT(gReceiveVideoSink), "force-aspect-ratio", TRUE, NULL);
    gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(gReceiveVideoSink), (guintptr)hwnd);

    // Pipeline execution
    GstStateChangeReturn videoRet = gst_element_set_state(gReceiveVideoPipeline, GST_STATE_PLAYING);
    GstStateChangeReturn audioRet = gst_element_set_state(gReceiveAudioPipeline, GST_STATE_PLAYING);
    if (videoRet == GST_STATE_CHANGE_FAILURE || audioRet == GST_STATE_CHANGE_FAILURE)
    {
        g_printerr("Unable to start the pipeline.\n");
        gst_object_unref(gReceiveVideoPipeline);
        return -1;
    }

    g_printerr("Receiver : Run receiver main loop!\n");

    // Start the main loop
    gReceiverLoop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(gReceiverLoop);

    g_printerr("Receiver : Stop receiver main loop!\n");

    // Release the pipeline
    gst_element_set_state(gReceiveVideoPipeline, GST_STATE_NULL);
    gst_element_set_state(gReceiveAudioPipeline, GST_STATE_NULL);
    gst_object_unref(gReceiveVideoPipeline);
    gst_object_unref(gReceiveAudioPipeline);
    g_main_loop_unref(gReceiverLoop);

    return 0;
}

void quiteReceiverMain()
{
    g_main_loop_quit(gReceiverLoop);
}