#include "sendMain.h"
#include <gst/gst.h>

#define H_264_TUNE_NONE         (0x00000000)
#define H_264_TUNE_STILLIMAGE   (0x00000001)
#define H_264_TUNE_FASTCODE     (0x00000002)
#define H_264_TUNE_ZEROLATENCY  (0x00000004)

// Create GStreamer loop
GMainLoop* gSenderLoop;

// Create GStreamer pipline
GstElement* gSendPipeline;

// Create GStreamer pipline elements for video
GstElement* gSendVideoSrc;
GstElement* gSendVideoScale;
GstElement* gSendVideoResCaps;
GstElement* gSendVideoH264Enc;
GstElement* gSendVideoRTPH264Pay;
GstElement* gSendVideoSRTPEnc;
GstElement* gSendVideoSRTPCaps;
GstElement* gSendVideoQueue;
GstElement* gSendVideoSink;

// Create GStreamer pipline elements for audio
GstElement* gSendAudioSrc; 
GstElement* gSendAudioConv;
GstElement* gSendAudioResample;
GstElement* gSendAudioOpusenc;
GstElement* gSendAudioRTPOpusPay;
GstElement* gSendAudioSRTPEnc;
GstElement* gSendAudioSRTPCaps;
GstElement* gSendAudioQueue;
GstElement* gSendAudioSink;

GstCaps* gCaps;
GstPad* gSinkpad;

void changeVideoResolution(int width, int height)
{
    // Pipeline stop(?)
    GstStateChangeReturn ret = gst_element_set_state(gSendPipeline, GST_STATE_NULL);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        g_printerr("Sender : Unable to start the pipeline.\n");
        gst_object_unref(gSendPipeline);
    }

    // Set the video resolution using capsfilter
    gCaps = gst_caps_new_simple("video/x-raw",
        "width", G_TYPE_INT, width,
        "height", G_TYPE_INT, height,
        "framerate", GST_TYPE_FRACTION, 30, 1,
        NULL);

    g_object_set(gSendVideoResCaps, "caps", gCaps, NULL);
    gst_caps_unref(gCaps);

    // Pipeline replay(?)
    ret = gst_element_set_state(gSendPipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        g_printerr("Sender : Unable to start the pipeline.\n");
        gst_object_unref(gSendPipeline);
    }
}

int sendMain()
{
    g_printerr("Sender : Start sendMain thread\n");

    // Create GStreamer pipline
    gSendPipeline = gst_pipeline_new("pipeline");

    // Create GStreamer pipline elements for video
    gSendVideoSrc = gst_element_factory_make("mfvideosrc", "videoSrc");
    gSendVideoScale = gst_element_factory_make("videoscale", "videoScale");
    gSendVideoResCaps = gst_element_factory_make("capsfilter", "videoResCaps");
    gSendVideoH264Enc = gst_element_factory_make("x264enc", "videoEnc");
    gSendVideoRTPH264Pay = gst_element_factory_make("rtph264pay", "videoPay");
    gSendVideoSRTPEnc = gst_element_factory_make("srtpenc", "videoEncrypt");
    gSendVideoSRTPCaps = gst_element_factory_make("capsfilter", "videoSRTPCaps");
    gSendVideoQueue = gst_element_factory_make("queue", "videoQueue");
    gSendVideoSink = gst_element_factory_make("udpsink", "videoSink");

    // Create GStreamer pipline elements for audio
    gSendAudioSrc = gst_element_factory_make("autoaudiosrc", "audioSrc");
    gSendAudioConv = gst_element_factory_make("audioconvert", "audioConv");
    gSendAudioResample = gst_element_factory_make("audioresample", "audioResample");
    gSendAudioOpusenc = gst_element_factory_make("opusenc", "audioOpusenc");
    gSendAudioRTPOpusPay = gst_element_factory_make("rtpopuspay", "audioPay");
    gSendAudioSRTPEnc = gst_element_factory_make("srtpenc", "audioEncrypt");
    gSendAudioSRTPCaps = gst_element_factory_make("capsfilter", "audioSRTPCaps");
    gSendAudioQueue = gst_element_factory_make("queue", "audioQueue");
    gSendAudioSink = gst_element_factory_make("udpsink", "audioSink");

    // Add element to pipeline
    gst_bin_add_many(GST_BIN(gSendPipeline),
        gSendVideoSrc, gSendVideoScale, gSendVideoResCaps, gSendVideoH264Enc, gSendVideoRTPH264Pay, gSendVideoSRTPEnc, gSendVideoSRTPCaps, gSendVideoQueue, gSendVideoSink,
        gSendAudioSrc, gSendAudioConv, gSendAudioResample, gSendAudioOpusenc, gSendAudioRTPOpusPay, gSendAudioSRTPEnc, gSendAudioSRTPCaps, gSendAudioQueue, gSendAudioSink,
        NULL);

    // linking elements for video
    gst_element_link_many(
        gSendVideoSrc, gSendVideoScale, gSendVideoResCaps, gSendVideoH264Enc, gSendVideoRTPH264Pay, gSendVideoSRTPEnc, gSendVideoSRTPCaps, gSendVideoQueue, gSendVideoSink,
        NULL);

    // linking elements for audio
    gst_element_link_many(
        gSendAudioSrc, gSendAudioConv, gSendAudioResample, gSendAudioOpusenc, gSendAudioRTPOpusPay, gSendAudioSRTPEnc, gSendAudioSRTPCaps, gSendAudioQueue, gSendAudioSink,
        NULL);

    // set up laptop camera
    g_object_set(gSendVideoSrc, "device-index", 0, NULL);  // 0은 첫 번째 카메라를 나타냅니다.

    // Set the video resolution using capsfilter
    gCaps = gst_caps_new_simple(
        "video/x-raw",
        "width", G_TYPE_INT, 1280,
        "height", G_TYPE_INT, 720,
        "framerate", GST_TYPE_FRACTION, 30, 1,
        NULL);
    g_object_set(gSendVideoResCaps, "caps", gCaps, NULL);
    gst_caps_unref(gCaps);

    // Set the tune attribute of the x264enc element to "zerolatency"
    g_object_set(gSendVideoH264Enc, "tune", H_264_TUNE_ZEROLATENCY, NULL);

    // Set the SRTP encryption key for video
    static guint8 data[30];
    memset(data, 0, sizeof(data));
    static guint size = sizeof(data);
    GstBuffer* videoKeyBuffer = gst_buffer_new_wrapped(data, size);

    g_object_set(gSendVideoSRTPEnc, "key", videoKeyBuffer, NULL);
    g_object_set(gSendVideoSRTPEnc, "rtp-auth", 2, NULL);
    g_object_set(gSendVideoSRTPEnc, "rtp-cipher", 1, NULL);
    g_object_set(gSendVideoSRTPEnc, "rtcp-auth", 2, NULL);
    g_object_set(gSendVideoSRTPEnc, "rtcp-cipher", 1, NULL);

    // Set RTP for video
    gCaps = gst_caps_from_string("application/x-srtp, payload=(int)96, ssrc=(uint)112233, roc=(uint)0");
    g_object_set(gSendVideoSRTPCaps, "caps", gCaps, NULL);
    gst_caps_unref(gCaps);

    // Set the receiving IP and port for video
    g_object_set(gSendVideoSink, "host", "127.0.0.1", "port", 5001, NULL);

    // generic (2049)               – Generic audio
    // voice (2048)                 – Voice
    // restricted-lowdelay (2051)   – Restricted low delay
    // Set the opusenc element's audio-type attribute to "restricted-lowdelay"
    g_object_set(gSendAudioOpusenc, "audio-type", 2051, NULL);

    // Set RTP for audio
    gSinkpad = gst_element_get_static_pad(gSendAudioRTPOpusPay, "src");
    gCaps = gst_caps_from_string("application/x-rtp, media=(string)audio, payload=(int)96");
    gst_pad_set_caps(gSinkpad, gCaps);
    gst_caps_unref(gCaps);
    gst_object_unref(gSinkpad);

    // Set the SRTP encryption key for audio
    GstBuffer* audioKeyBuffer = gst_buffer_new_wrapped(data, size);

    g_object_set(gSendAudioSRTPEnc, "key", audioKeyBuffer, NULL);
    g_object_set(gSendAudioSRTPEnc, "rtp-auth", 2, NULL);
    g_object_set(gSendAudioSRTPEnc, "rtp-cipher", 1, NULL);
    g_object_set(gSendAudioSRTPEnc, "rtcp-auth", 2, NULL);
    g_object_set(gSendAudioSRTPEnc, "rtcp-cipher", 1, NULL);

    // Set RTP for video
    gCaps = gst_caps_from_string("application/x-srtp, payload=(int)96, ssrc=(uint)112233, roc=(uint)0");
    g_object_set(gSendAudioSRTPCaps, "caps", gCaps, NULL);
    gst_caps_unref(gCaps);

    // Set the receiving IP and port for audio
    g_object_set(gSendAudioSink, "host", "127.0.0.1", "port", 5002, NULL);

    // Pipeline execution
    GstStateChangeReturn ret = gst_element_set_state(gSendPipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        g_printerr("Sender : Unable to start the pipeline.\n");
        gst_object_unref(gSendPipeline);
        return -1;
    }

    g_printerr("Sender : Run sender main loop!\n");

    // Start the main loop
    gSenderLoop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(gSenderLoop);

    g_printerr("Sender : Stop sender main loop!\n");

    // Release the pipeline
    gst_element_set_state(gSendPipeline, GST_STATE_NULL);
    gst_object_unref(gSendPipeline);

    return 0;
}

void quiteSenderMain()
{
    g_main_loop_quit(gSenderLoop);
}