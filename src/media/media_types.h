#ifndef MEDIA_MEDIA_TYPES_H_
#define MEDIA_MEDIA_TYPES_H_

#include <map>

namespace media {

enum class VideoPresetLevel {
	kVideoPresetOff = 0,
	kVideoPreset1 = 1,
	kVideoPreset2 = 2,
	kVideoPreset3 = 3,
	kVideoPreset4 = 4,
	kVideoPreset5 = 5,
};

struct VideoPresetType {
	VideoPresetType():
		level(VideoPresetLevel::kVideoPresetOff), width(0), height(0), bitrate(0) {}
	VideoPresetType(VideoPresetLevel plevel, int pwidth, int pheight, int pbitrate):
		level(plevel), width(pwidth), height(pheight), bitrate(pbitrate) {}

	VideoPresetLevel level;
	int width;
	int height;
	int bitrate; // kbps
};

extern const std::map<VideoPresetLevel, VideoPresetType> kVideoPresets;

} // namespace media

#endif