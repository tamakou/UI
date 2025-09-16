#ifndef TH_PROC_LIB_DLL_LOADER_H
#define TH_PROC_LIB_DLL_LOADER_H

#include "windows.h"
#include "../include/IThProcLibData.h"
typedef int (*th_lib_init)(int mode);
typedef void (*th_lib_close)(void);
typedef int (*th_lib_setup_env)(const ThProcLib_Env* inEnv);
typedef int (*th_lib_get_env)(ThProcLib_Env* outEnv);
typedef int (*th_lib_get_data_size)(int type);
typedef int (*th_lib_get_datalist)(ThProcLib_PatientInfo* out_info, int type, int no);
typedef int (*th_lib_exe_cmd)(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret);
typedef int (*th_lib_get_image_info)(const ThProcLib_Cmd* cmd, ThProcLib_DicomImageInfo* image_info);
typedef int (*th_lib_get_data)(const ThProcLib_Cmd* cmd, unsigned char* DataBuffer, int BufferSize);

typedef int (*th_lib_get_status)(int cmdUID, ThProcLib_CmdStatus* status);
typedef int (*th_lib_close_cmd)(int cmdUID);

typedef int (*th_lib_get_vol_info)(const ThProcLib_Cmd* cmd, ThProcLib_Vol3DData* outVol);
typedef int (*th_lib_render_GC)(const ThProcLib_Cmd* cmd, ThProcLib_RenderGC* outGC);
typedef int (*th_lib_exe_render)(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, ThProcLib_Image2DData* outImage);
 
class CThProcLibDllLoader
{
public:
	
	CThProcLibDllLoader();
	virtual ~CThProcLibDllLoader();
	bool init(void);
	th_lib_init _dll_th_lib_init;
	th_lib_close _dll_th_lib_close;
	th_lib_setup_env _dll_th_lib_setup_env;
	th_lib_get_env _dll_th_lib_get_env;
	th_lib_get_data_size _dll_th_lib_get_data_size;
	th_lib_get_datalist _dll_th_lib_get_datalist;
	th_lib_exe_cmd _dll_th_lib_exe_cmd;
	th_lib_get_image_info _dll_th_lib_get_image_info;
	th_lib_get_data _dll_th_lib_get_data;
	th_lib_get_status _dll_th_lib_get_status;
	th_lib_close_cmd _dll_th_lib_close_cmd;
	//
	th_lib_get_vol_info _dll_th_lib_get_vol_info;
	th_lib_render_GC _dll_th_lib_render_GC;
	th_lib_exe_render  _dll_th_lib_exe_render;

protected:
	HMODULE m_dll_hander;
	
};
#endif //TH_PROC_LIB_DLL_LOADER_H