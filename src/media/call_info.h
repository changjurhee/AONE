#pragma once
#include <string>
struct ContactInfo {
	std::string cid;
	std::string dest_ip;
	int dest_video_port;
	int dest_audio_port;
	int org_video_port;
	int org_audio_port;
};

struct OperatingInfo {
	std::string video_codec;
	std::string audio_codec;
	std::string encryption_alg;
	std::string encryption_key;
};