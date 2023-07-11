// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H

// 여기에 미리 컴파일하려는 헤더 추가
#include "framework.h"
#include <memory>
#include <string>
#include <sstream>
#include <string_view>
#include <chrono>
#include <iomanip>

using time_point = std::chrono::system_clock::time_point;
static std::string serializeTimePoint(const time_point& time, const std::string& format)
{
	std::time_t tt = std::chrono::system_clock::to_time_t(time);
	std::tm tm;
	localtime_s(&tm, &tt);
	//gmtime_s(&tm, &tt);
	std::stringstream ss;
	ss << std::put_time(&tm, format.c_str());
	return ss.str();
}

static std::pair<std::string, std::string> GetDateTime(long long dataAndTime, long long duration) {
	std::chrono::system_clock::time_point input = std::chrono::system_clock::now();
	auto now1 = std::chrono::time_point_cast<std::chrono::seconds>(input);
	using sys_seconds = decltype(now1);

	sys_seconds dt1{ std::chrono::seconds{dataAndTime} };
	sys_seconds dt2{ std::chrono::seconds{dataAndTime + duration} };

	std::pair<std::string, std::string> res;
	res.first = serializeTimePoint(dt1, "%Y-%m-%d %H:%M:%S");
	res.second = serializeTimePoint(dt2, "%Y-%m-%d %H:%M:%S");

	return res;
}



#endif //PCH_H
