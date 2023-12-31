#pragma once
#include <string>
struct ContactInfo {
	std::string cid;
	std::string client_name;
	std::string my_ip;
	std::string dest_ip;
	int dest_video_port;
	int dest_audio_port;
	int org_video_port;
	int org_audio_port;
	bool new_client;
	bool dummy_client;
};

struct OperatingInfo {
	std::string video_codec;
	std::string audio_codec;
	std::string encryption_alg;
	std::string encryption_key;
};