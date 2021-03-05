#include "SystemUtil.h"

#include <Windows.h>
#include "GlobalContext.h"
#include <chrono>


std::string SystemUtil::string_to_utf8(const std::string & str) {
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t * pwBuf = new wchar_t[nwLen + 1];
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char * pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}

std::string SystemUtil::utf8_to_string(const std::string & str) {
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

	wchar_t * pwBuf = new wchar_t[nwLen + 1];
	memset(pwBuf, 0, nwLen * 2 + 2);

	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char * pBuf = new char[nLen + 1];
	memset(pBuf, 0, nLen + 1);

	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr = pBuf;

	delete[]pBuf;
	delete[]pwBuf;

	pBuf = NULL;
	pwBuf = NULL;

	return retStr;
}


//获得dll所在路径
std::string SystemUtil::get_module_path()
{
	char szBuff[MAX_PATH] = { 0 };
	::GetModuleFileName(NULL, szBuff, MAX_PATH);
	std::string path{ szBuff };
	auto found = path.find_last_of('\\');
	path = path.substr(0, found+1);
	return path;
}

std::string SystemUtil::get_date()
{
	auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	struct tm t; 
	localtime_s(&t, &tt);
	char date[11] = { 0 };
	snprintf(date, sizeof(date),"%d-%02d-%02d",(int)t.tm_year + 1900, (int)t.tm_mon + 1, (int)t.tm_mday);
	return std::string(date);
}


std::string& SystemUtil::replace_all(std::string& src, const std::string& old_value, const std::string& new_value) {
	// 每次重新定位起始位置，防止上轮替换后的字符串形成新的old_value
	for (std::string::size_type pos(0); pos != std::string::npos; pos += new_value.length()) {
		if ((pos = src.find(old_value, pos)) != std::string::npos) {
			src.replace(pos, old_value.length(), new_value);
		}
		else break;
	}
	return src;
}
