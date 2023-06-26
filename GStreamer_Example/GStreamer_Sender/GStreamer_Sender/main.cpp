#include <gst/gst.h>

int main(int argc, char* argv[]) {
    // GStreamer 초기화
    gst_init(&argc, &argv);

    // GStreamer 파이프라인 생성
    GstElement* pipeline = gst_pipeline_new("sender_pipeline");
    GstElement* src = gst_element_factory_make("mfvideosrc", "src");
    GstElement* enc = gst_element_factory_make("x264enc", "enc");
    GstElement* pay = gst_element_factory_make("rtph264pay", "pay");
    GstElement* capsfilter = gst_element_factory_make("capsfilter", "filter");
    GstElement* sink = gst_element_factory_make("udpsink", "sink");

    // 파이프라인에 요소 추가
    gst_bin_add_many(GST_BIN(pipeline), src, enc, pay, capsfilter, sink, NULL);

    // 요소 간 링크 설정
    gst_element_link_many(src, enc, pay, capsfilter, sink, NULL); 

    // 송신 주소 설정 (IP 주소는 127.0.0.1로 고정, 포트 번호 5001)
    g_object_set(sink, "host", "127.0.0.1", "port", 5001, NULL);

    // RTP 형식(caps) 설정
    GstCaps* caps = gst_caps_from_string("application/x-rtp, media=(string)video, payload=(int)96");
    g_object_set(G_OBJECT(capsfilter), "caps", caps, NULL);
    gst_caps_unref(caps);

    // 노트북 카메라 설정
    g_object_set(src, "device-index", 0, NULL);  // 0은 첫 번째 카메라를 나타냅니다.

    // x264enc 요소의 tune 속성을 "zerolatency"로 설정
    g_object_set(enc, "tune", 0x00000004, NULL);

    // 파이프라인 실행
    GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Unable to start the pipeline.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // 메인 루프 실행
    GstBus* bus = gst_element_get_bus(pipeline);
    GstMessage* msg;
    while ((msg = gst_bus_poll(bus, GST_MESSAGE_ANY, GST_CLOCK_TIME_NONE))) {
        GError* err;
        gchar* debug_info;
        switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_ERROR:
            gst_message_parse_error(msg, &err, &debug_info);
            g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
            g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
            g_error_free(err);
            g_free(debug_info);
            break;
        case GST_MESSAGE_EOS:
            g_print("End-Of-Stream reached.\n");
            break;
        case GST_MESSAGE_STATE_CHANGED: {
            GstState old_state, new_state, pending_state;
            gst_message_parse_state_changed(msg, &old_state, &new_state, &pending_state);
            g_print("State changed from %s to %s\n", gst_element_state_get_name(old_state), gst_element_state_get_name(new_state));
            break;
        }
        case GST_MESSAGE_BUFFERING: {
            gint percent = 0;
            gst_message_parse_buffering(msg, &percent);
            g_print("Buffering %d%%\n", percent);
            break;
        }
        default:
            break;
        }
        gst_message_unref(msg);
    }

    // 파이프라인 정지 및 해제
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    return 0;
}
