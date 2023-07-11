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

#include "common/logger.h"

using tstring = std::basic_string<TCHAR>;
using tstring_view = std::basic_string_view<TCHAR>;
using tstringstream = std::basic_stringstream<TCHAR>;

#define UM_VAD_STATE           (WM_USER+101)

#pragma warning(push)
#pragma warning(disable:4840)
template<class ... TypeTs>
static std::wstring FormatString(const std::wstring_view& rsFormat, TypeTs ... Variables)
{
	CStringW sMessage;
	sMessage.Format(rsFormat.data(), Variables...);

	return std::wstring(sMessage);
}

template<class ... TypeTs>
static std::string FormatString(const std::string_view& rsFormat, TypeTs ... Variables)
{
	CStringA sMessage;
	sMessage.Format(rsFormat.data(), Variables...);

	return std::string(sMessage);
}
#pragma warning(pop)

struct stOleToDate {
	long long dataAndTime;
	long long duration;
};

struct stDateToOle {
	COleDateTime m_Date;
	COleDateTime m_StartTime;
	COleDateTime m_endTime;
};

constexpr long long RESOLUTION_TIME = 10000000;

using time_point = std::chrono::system_clock::time_point;
static std::string serializeTimePoint(const time_point& time, const std::string& format)
{
	std::time_t tt = std::chrono::system_clock::to_time_t(time);
	std::tm tm;
	gmtime_s(&tm, &tt);
	std::stringstream ss;
	ss << std::put_time(&tm, format.c_str());
	return ss.str();
}

static stOleToDate SetDateTime(COleDateTime m_Date, COleDateTime m_StartTime, COleDateTime m_endTime) {
	std::tm tm_start = {};
	std::tm tm_end = {};
	std::string st_time = FormatString("%d %d %d %d %d %d", m_Date.GetYear(), m_Date.GetMonth(), m_Date.GetDay(), m_StartTime.GetHour(), m_StartTime.GetMinute(), m_StartTime.GetSecond());
	std::string ed_time = FormatString("%d %d %d %d %d %d", m_Date.GetYear(), m_Date.GetMonth(), m_Date.GetDay(), m_endTime.GetHour(), m_endTime.GetMinute(), m_endTime.GetSecond());

	std::stringstream ss_s(st_time);
	std::stringstream ss_e(ed_time);
	ss_s >> std::get_time(&tm_start, "%Y %m %d %H %M %S");
	ss_e >> std::get_time(&tm_end, "%Y %m %d %H %M %S");
	auto tp_s = std::chrono::system_clock::from_time_t(std::mktime(&tm_start));
	auto tp_e = std::chrono::system_clock::from_time_t(std::mktime(&tm_end));
	
	auto now1 = std::chrono::time_point_cast<std::chrono::seconds>(tp_s);
	auto now2 = std::chrono::time_point_cast<std::chrono::seconds>(tp_e);

	auto integral_duration1 = now1.time_since_epoch().count();
	auto integral_duration2 = now2.time_since_epoch().count();
	
	stOleToDate res;
	res.dataAndTime = integral_duration1;
	res.duration = integral_duration2 - integral_duration1;

	return res;
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

enum class KResponse : INT_PTR {
	CONNECT_COMPLETE, 
	CONNECT_FAILED,
	LOGIN_COMPLETE,
	LOGIN_FAILED, 
	SIGNIN_COMPLETE, 
	SIGNIN_CANCELED,

	CREATE_USER,
	UPDATE_USER,
	RESET_PASSWORD,
	START,
};

struct userInfo {
	tstring email;
	tstring password;
	tstring server_ip_num;
};

#endif //PCH_H
