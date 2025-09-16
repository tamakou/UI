#ifndef TH_PROC_LIB_MAIN_H
#define TH_PROC_LIB_MAIN_H

#include "../include/IThProcLib.h"
#include "MultiThExe.h"
#include <vector>
#include <map>
#include <string>
#include <memory>

struct ExeThreader
{
	std::unique_ptr<CThProcIf> m_proc_hdr;
	std::unique_ptr <CMultiThExe> m_theExe;
};

class CThProcMain
{
public:
	CThProcMain();
	virtual ~CThProcMain();
	void selfTest(void);
	/// <summary>
	/// common inteface
	/// </summary>

	virtual int th_lib_init(int mode);
	virtual int th_lib_setup_env(const ThProcLib_Env* env);
	virtual int th_lib_get_env(ThProcLib_Env* env);
	virtual int th_lib_get_datalist_size(int type) = 0;
	virtual int th_lib_get_datalist(ThProcLib_PatientInfo* out_info, int type, int no) = 0;
	virtual int th_lib_exe_cmd(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) = 0;
	virtual int th_lib_get_image_info(const ThProcLib_Cmd* cmd, ThProcLib_DicomImageInfo* image_info) = 0;
	virtual int th_lib_get_data(const ThProcLib_Cmd* cmd, unsigned char* DataBuffer, int BufferSize) ;
	virtual int th_lib_get_status(int cmdUID, ThProcLib_CmdStatus* status) ;
	virtual int th_lib_close_cmd(int cmdUID) = 0;
	virtual int th_lib_load_vol(const ThProcLib_Cmd* cmd, ThProcLib_Vol3DData* outVol);
	///#43_rendering_command
	virtual int th_lib_render_GC(const ThProcLib_Cmd* cmd, ThProcLib_RenderGC* outGC);
	virtual int th_lib_exe_render(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, ThProcLib_Image2DData* outImage);

protected:
	virtual int get_new_data_ID(void) = 0;
	virtual int exe_cmd_import(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) = 0;
	virtual int check_cmd_async(int cmdID,int cmdUID,int chkNo=0) = 0;
	virtual int exe_cmd_AiSegmentator(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) = 0;
	//#32_ThLibCmd_Output_as_async
	virtual int exe_cmd_add_output(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) = 0;
	virtual int exe_cmd_delete_output(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) = 0;
	virtual int exe_get_image_data(const ThProcLib_Cmd* cmd, unsigned char* DataBuffer, int BufferSize) = 0;
	virtual int exe_cmd_vol_render(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, ThProcLib_Image2DData* outImage);
	virtual int finish_cmd_async(int cmdID, int cmdUID) = 0;
	//#29_command_added
	virtual int proc_CT_Marker(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) = 0;
	virtual int proc_ROI(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) = 0;
	virtual int proc_Output(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) = 0;
	virtual int release_Render(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret);
	int exe_cmd_async(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) ;
	//
	int  generate_vol3D_ID(void);
	int  generate_image2D_ID(void);
	int  generate_renderGC_ID(void);
	void release_vol3D_ID(int ID);
	void release_image2D_ID(int ID);
	void release_renderGC_ID(int ID);
	ThProcLib_Vol3DData *get_vol3D_ID(int ID);
	ThProcLib_Image2DData* get_image2D_ID(int ID);
	ThProcLib_RenderGC* get_renderGC_ID(int ID);

	std::wstring get_cmd_name(int cmdID);
	int generateCmdUId(void) {
		int retUID = m_cmd_uid_count++;
		if (retUID <1) retUID = 1;
		return retUID;
	}
	void resetCmdOutput(ThProcLib_CmdOutput* cmd_ret);
	void LogMessage(const wchar_t* fmt, ...);
	std::wstring m_logFileName;

	ThProcLib_Env m_lib_env;
	int m_import_CDROM_timeout_sec;//Sec
	int m_AiSeg_timeout_sec;//Sec
	int m_cmd_uid_count;
	int m_last_cmdID;
	int m_last_cmdUID;
	int m_lib_mode;
	//
	ThProcLib_PatientInfo m_new_data;
	//
	std::map<int, std::unique_ptr<ThProcLib_CmdStatus>> m_cmd_uid_map;
	//
	ExeThreader m_thread_cmd_async;
	//
	ThProcLib_Cmd m_current_cmd;
	//
	std::map<int, ThProcLib_Vol3DData> m_Vol3DList;
	int m_Vol3D_gen_number;
	std::map<int, ThProcLib_Image2DData> m_Image2DList;
	int m_Image2D_gen_number;
	std::map<int, ThProcLib_RenderGC> m_RenderGCList;
	int m_RenderGC_gen_number;
};
#define _copy_wstr( dest_buff,src_str)  wcscpy_s((dest_buff),sizeof(dest_buff)/sizeof(wchar_t),(src_str));
#define _copy_str( dest_buff,src_str)  strncpy_s((dest_buff),sizeof(dest_buff) ,(src_str),sizeof(dest_buff));

#endif //TH_PROC_LIB_MAIN_H