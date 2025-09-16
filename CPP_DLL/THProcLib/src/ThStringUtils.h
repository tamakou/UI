//  
//
#pragma once
 

#include <string>
 
#include <memory>
namespace ThStringUtils
{
	std::string _convertStr(const std::wstring& str);
	std::wstring _convertStr(const std::string& str);
	std::string _convertStrUtf8(const std::wstring& str);
	std::wstring _convertStrUtf8(const std::string& str);
};