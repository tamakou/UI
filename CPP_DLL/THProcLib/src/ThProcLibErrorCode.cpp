 
#include "ThProcLibErrorCode.h"
#include <map>
using namespace ThProcLibErrorCode;
std::map<int, std::wstring> _error_code_table{
	
	{ThDllErr_Dll_Unknown_Error, L"Dll Unknown Error" },
	{ThDllErr_InvalidVolID, L"Invalid VolID" },
	{ThDllErr_InvalidImportData, L"Invalid Data" },
	{ThDllErr_LoadVolFailed,L"Load Vol. is Failed"},
	{ThDllErr_No2SeriesData,L"Not Completed Haiya/Juukaku"},
	{ThDllErr_DeleteStudy,L"Delete Study Error"},
	{ThDllErr_ImportDataCanceled,L"ImportData is Canceled"},
	{ThDllErr_AiSegmentation,L"AI Segmentation Error"},
	{ThDllErr_OutputMesh,L"Output Mesh Error"},
	{ThDllErr_ImportData,L"ImportData Error"},
	{ThDllErr_InvalidRenderImage,L"Invalid Render Image"},
	
	 
};
std::wstring ThProcLibErrorCode::getErrCode(int code)
{
	wchar_t _str_buffer[64];
	swprintf(_str_buffer, 64, L"-%04d", code);
	return  _str_buffer;
}
std::wstring ThProcLibErrorCode::getErrStr(int code)
{
	std::wstring str;
	for (auto it : _error_code_table) {
		if (it.first == code) {
			str = it.second;
			break;
		}
	}
	return str;
 }
   