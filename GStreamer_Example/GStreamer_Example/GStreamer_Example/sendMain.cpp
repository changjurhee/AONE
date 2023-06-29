#include <gst/gst.h>
#include "sendMain.h"

// Create GStreamer loop
GMainLoop* gSenderLoop;

// Create GStreamer pipline
GstElement* gSendPipeline;

// Create GStreamer pipline elements for video
GstElement* gSendVideoSrc;
GstElement* gSendVideoScale;
GstElement* gSendVideoCapsFilter;
GstElement* gSendVideoEnc;
GstElement* gSendVideoPay;
GstElement* gSendVideoSink;

// Create GStreamer pipline elements for audio
GstElement* gSendAudioSrc;
GstElement* gSendAudioConv;
GstElement* gSendAudioResample;
GstElement* gSendAudioOpusenc;
GstElement* gSendAudioPay;
GstElement* gSendAudioSink;

// level 요소의 메시지 처리 콜백 함수
static gboolean handle_level_message(GstBus* bus, GstMessage* message, gpointer user_data) {
    if (GST_MESSAGE_TYPE(message) == GST_MESSAGE_ELEMENT) {
        const GstStructure* structure = gst_message_get_structure(message);
        if (gst_structure_has_name(structure, "level")) {
            // 볼륨 레벨 정보 가져오기
            const GValue* rms_value = gst_structure_get_value(structure, "rms");
            const GValue* peak_value = gst_structure_get_value(structure, "peak");

            if (rms_value != NULL && peak_value != NULL) {
                if (G_VALUE_HOLDS_DOUBLE(rms_value) && G_VALUE_HOLDS_DOUBLE(peak_value)) {
                    gdouble rms = g_value_get_double(rms_value);
                    gdouble peak = g_value_get_double(peak_value);

                    // 볼륨 레벨 출력
                    g_print("RMS level: %f, Peak level: %f\n", rms, peak);
                }
                else {
                    g_print("Invalid value type for rms or peak\n");
                }
            }
        }
    }

    return TRUE;
}


void changeVideoResolution(int width, int height)
{
    // Pipeline stop(?)
    GstStateChangeReturn ret = gst_element_set_state(gSendPipeline, GST_STATE_PAUSED);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        g_printerr("Sender : Unable to start the pipeline.\n");
        gst_object_unref(gSendPipeline);
    }

    // Set the video resolution using capsfilter
    GstCaps* caps = gst_caps_new_simple("video/x-raw",
        "width", G_TYPE_INT, width,
        "height", G_TYPE_INT, height,
        "framerate", GST_TYPE_FRACTION, 30, 1,
        NULL);

    g_object_set(gSendVideoCapsFilter, "caps", caps, NULL);
    gst_caps_unref(caps);

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
    gSendVideoScale = gst_element_factory_make("videoscale", "gSendVideoScale");
    gSendVideoCapsFilter = gst_element_factory_make("capsfilter", "videoCapsFilter");
    gSendVideoEnc = gst_element_factory_make("x264enc", "videoEnc");
    gSendVideoPay = gst_element_factory_make("rtph264pay", "videoPay");
    gSendVideoSink = gst_element_factory_make("udpsink", "videoSink");

    // Create GStreamer pipline elements for audio
    gSendAudioSrc = gst_element_factory_make("autoaudiosrc", "audioSrc");
    gSendAudioConv = gst_element_factory_make("audioconvert", "audioConv");
    gSendAudioResample = gst_element_factory_make("audioresample", "audioResample");
    gSendAudioOpusenc = gst_element_factory_make("opusenc", "audioOpusenc");
    gSendAudioPay = gst_element_factory_make("rtpopuspay", "audioPay");
    gSendAudioSink = gst_element_factory_make("udpsink", "audioSink");

    // Add element to pipeline
    gst_bin_add_many(GST_BIN(gSendPipeline), gSendVideoSrc, gSendVideoScale, gSendVideoCapsFilter, gSendVideoEnc, gSendVideoPay, gSendVideoSink,
        gSendAudioSrc, gSendAudioConv, gSendAudioResample, gSendAudioOpusenc, gSendAudioPay, gSendAudioSink, NULL);

    // linking elements for video
    gst_element_link_many(gSendVideoSrc, gSendVideoScale, gSendVideoCapsFilter, gSendVideoEnc, gSendVideoPay, gSendVideoSink, NULL);

    // linking elements for audio
    gst_element_link_many(gSendAudioSrc, gSendAudioConv, gSendAudioResample, gSendAudioOpusenc, gSendAudioPay, gSendAudioSink, NULL);

    // Set the receiving IP and port for video
    g_object_set(gSendVideoSink, "host", "127.0.0.1", "port", 5001, NULL);

    // Set the receiving IP and port for audio
    g_object_set(gSendAudioSink, "host", "127.0.0.1", "port", 5002, NULL);

    // set up laptop camera
    g_object_set(gSendVideoSrc, "device-index", 0, NULL);  // 0은 첫 번째 카메라를 나타냅니다.

    // Set the tune attribute of the x264enc element to "zerolatency"
    g_object_set(gSendVideoEnc, "tune", 0x00000004, NULL);

    // generic (2049)               – Generic audio
    // voice (2048)                 – Voice
    // restricted-lowdelay (2051)   – Restricted low delay
    // Set the opusenc element's audio-type attribute to "restricted-lowdelay"
    g_object_set(gSendAudioOpusenc, "audio-type", 2051, NULL);

    // Set the video resolution using capsfilter
    GstCaps* caps = gst_caps_new_simple("video/x-raw",
        "width", G_TYPE_INT, 1280,
        "height", G_TYPE_INT, 720,
        "framerate", GST_TYPE_FRACTION, 30, 1,
        NULL);
    g_object_set(gSendVideoCapsFilter, "caps", caps, NULL);
    gst_caps_unref(caps);

    // level 요소에서 발생하는 메시지 처리를 위한 버스 연결
    GstBus* bus = gst_element_get_bus(gSendPipeline);
    gst_bus_add_watch(bus, (GstBusFunc)handle_level_message, NULL);

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