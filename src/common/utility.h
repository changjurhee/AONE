#ifndef COMMON_UTILITY_H_
#define COMMON_UTILITY_H_

#include <string>

namespace util {

// 192.168.3.10 is trasformed to
// 13100 for audio
// 13101 for video
//
// For Example,
// 192.168.3.10
//   a.  b.c. d
// 
// 1/3/10/0
// 1: just a prefix value
// 3: 1 digit from c
// 10: last 2 digits from d
// 0 or 1: 0 if audio. Otherwise 1, video.
std::string MakePortFromIp(std::string ip, bool audio);

}

#endif