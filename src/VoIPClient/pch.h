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
#include <string_view>

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

enum class KResponse : INT_PTR {
	CONNECT_COMPLETE, 
	CONNECT_FAILED,
	LOGIN_COMPLETE,
	LOGIN_FAILED, 
	SIGNIN_COMPLETE, 
	SIGNIN_CANCELED,
	RESET_PASSWORD_COMPLETE, 
	RESET_PASSWORD_CANCELED, 
	UPDATE_USER_COMPLETE, 
	UPDATE_USER_CANCELED,
	

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
