#include "media_types.h"

namespace media {

const std::map<VideoPresetLevel, VideoPresetType> kVideoPresets = {
	// Please see https://github.com/changjurhee/AONE/wiki/Experiment-%232#expected-outcomes
	// {VideoPresetLevel, {VideoPresetLevel, width, height, bitrate}}
	{VideoPresetLevel::kVideoPresetOff, {VideoPresetLevel::kVideoPresetOff, 0,0,0}},
	{VideoPresetLevel::kVideoPreset1, {VideoPresetLevel::kVideoPreset1, 320, 240, 50 * 1024}},
	{VideoPresetLevel::kVideoPreset2, {VideoPresetLevel::kVideoPreset2, 320, 240, 120 * 1024}},
	{VideoPresetLevel::kVideoPreset3, {VideoPresetLevel::kVideoPreset3, 480, 360, 250 * 1024}},
	{VideoPresetLevel::kVideoPreset4, {VideoPresetLevel::kVideoPreset4, 640, 480, 500 * 1024}},
	{VideoPresetLevel::kVideoPreset5, {VideoPresetLevel::kVideoPreset5, 640, 480, 1024 * 1024}}
};

}