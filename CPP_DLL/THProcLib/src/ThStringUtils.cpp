// MainProcImp.cpp
//
#include "stdafx.h"
#include "ThStringUtils.h"
 
#include<vector>
#include<string>
#include <locale> 
#include<codecvt> 
#include <locale.h>
#include <string>
#include <iostream>
#include <windows.h>
std::string ThStringUtils::_convertStr(const std::wstring& str)
{
#if 1
	std::string str_temp;
	// wstring → MultiByte
	int iBufferSize = ::WideCharToMultiByte(CP_ACP, 0, str.c_str()
		, -1, (char*)NULL, 0, NULL, NULL);

	// バッファの取得
	CHAR* cpBufMultiByte = new CHAR[iBufferSize+1];

	// wstring → MultiByte
	::WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, cpBufMultiByte
		, iBufferSize, NULL, NULL);

	// stringの生成
	std::string oRet(cpBufMultiByte, cpBufMultiByte + iBufferSize - 1);

	// バッファの破棄
	delete[] cpBufMultiByte;

	// 変換結果を返す
	return(oRet);
 
#else
	//setup converter
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;

	//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
	std::string converted_str = converter.to_bytes(str);
	return converted_str;
#endif
}
std::wstring ThStringUtils::_convertStr(const std::string& str)
{
#if 1
	int iBufferSize = ::MultiByteToWideChar(CP_ACP, 0, str.c_str()
		, -1, (wchar_t*)NULL, 0);

	// バッファの取得
	wchar_t* wpBufWString = (wchar_t*)new wchar_t[iBufferSize+1];

	// MultiByte → wstring
	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, wpBufWString
		, iBufferSize);

	// wstringの生成
	std::wstring oRet(wpBufWString, wpBufWString + iBufferSize - 1);
	std::wstring str_temp = wpBufWString;
	// バッファの破棄
	delete[] wpBufWString;
	return str_temp;
#else
	//setup converter
	using convert_type = std::codecvt_utf8_utf16<wchar_t>;
	std::wstring_convert<convert_type> converter;
	 
	std::wstring converted_str = converter.from_bytes(str);

	return converted_str;
#endif
}

std::string ThStringUtils::_convertStrUtf8(const std::wstring& str)
{
#if 1
	std::string str_temp;
	// wstring → MultiByte
	int iBufferSize = ::WideCharToMultiByte(CP_UTF8, 0, str.c_str()
		, -1, (char*)NULL, 0, NULL, NULL);

	// バッファの取得
	CHAR* cpBufMultiByte = new CHAR[iBufferSize + 1];

	// wstring → MultiByte
	::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, cpBufMultiByte
		, iBufferSize, NULL, NULL);

	// stringの生成
	std::string oRet(cpBufMultiByte, cpBufMultiByte + iBufferSize - 1);

	// バッファの破棄
	delete[] cpBufMultiByte;

	// 変換結果を返す
	return(oRet);

#else
	//setup converter
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;

	//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
	std::string converted_str = converter.to_bytes(str);
	return converted_str;
#endif
}
std::wstring ThStringUtils::_convertStrUtf8(const std::string& str)
{
#if 1
	int iBufferSize = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str()
		, -1, (wchar_t*)NULL, 0);

	// バッファの取得
	wchar_t* wpBufWString = (wchar_t*)new wchar_t[iBufferSize + 1];

	// MultiByte → wstring
	::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wpBufWString
		, iBufferSize);

	// wstringの生成
	std::wstring oRet(wpBufWString, wpBufWString + iBufferSize - 1);
	std::wstring str_temp = wpBufWString;
	// バッファの破棄
	delete[] wpBufWString;
	return str_temp;
#else
	//setup converter
	using convert_type = std::codecvt_utf8_utf16<wchar_t>;
	std::wstring_convert<convert_type> converter;

	std::wstring converted_str = converter.from_bytes(str);

	return converted_str;
#endif
}
