#ifndef TH_PROC_LIB_ERROR_CODE_H
#define TH_PROC_LIB_ERROR_CODE_H
#include <string>
namespace ThProcLibErrorCode {
	enum ThDllErrorCode {
		ThDllErr_Dll_Unknown_Error = 1000,
		ThDllErr_InvalidVolID ,
		ThDllErr_InvalidImportData,
		ThDllErr_LoadVolFailed,
		ThDllErr_No2SeriesData,
		ThDllErr_DeleteStudy,
		ThDllErr_ImportDataCanceled,
		ThDllErr_AiSegmentation,
		ThDllErr_OutputMesh,
		ThDllErr_ImportData,
		ThDllErr_Unknown_Error,
		ThDllErr_InvalidRenderImage,
	};
	std::wstring getErrCode(int code);
	std::wstring getErrStr(int code);
}
#endif //TH_PROC_LIB_ERROR_CODE_H