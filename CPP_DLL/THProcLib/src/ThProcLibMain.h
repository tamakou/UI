#ifndef TH_PROC_LIB_MAIN_H
#define TH_PROC_LIB_MAIN_H

#include "ThProcLibLogger.h"
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

class CThProcMain : public CThProcLogger
{
public:
	enum eProcStatus {
		eProcStatus_Unknown = 0,
		eProcStatus_VolLoading,
		eProcStatus_VolLoaded,
	};
#if 0
	class _TraceLog {
	public:
		_TraceLog(const std::string& str, CThProcMain* owner,bool enable );
		virtual ~_TraceLog(void);
		CThProcMain* m_owner;
		std::wstring m_str;
		bool m_enable;
	};
#endif
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
	virtual int th_lib_get_vol_info(const ThProcLib_Cmd* cmd, ThProcLib_Vol3DData* outVol);
	///#43_rendering_command
	virtual int th_lib_render_GC(const ThProcLib_Cmd* cmd, ThProcLib_RenderGC* outGC);
	virtual int th_lib_exe_render(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, ThProcLib_Image2DData* outImage);

protected:
	virtual int get_new_data_ID(void) = 0;
	virtual int exe_cmd_import(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) = 0;
//#205_Cancel_import_data
	virtual int exe_cmd_async_cancel(const ThProcLib_Cmd* cmd) = 0;
	//#210_delete_imported_data
	virtual int exe_cmd_delete_import_data(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) = 0;

	virtual int check_cmd_async(int cmdID,int cmdUID,int chkNo=0) = 0;
	virtual int exe_cmd_AiSegmentator(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) = 0;
	virtual int exe_cmd_load_vol(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) = 0;
	//#103_load3DModel_command
	virtual int exe_cmd_load_3DModle(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) = 0;
	//#137_check_image_server
	virtual int exe_cmd_check_image_server(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) = 0;

	//#32_ThLibCmd_Output_as_async
	virtual int exe_cmd_add_output(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) = 0;
	virtual int exe_cmd_delete_output(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) = 0;
	virtual int delete_output_in(void);//#208_delete_output_file
	virtual int exe_get_image_data(const ThProcLib_Cmd* cmd, unsigned char* DataBuffer, int BufferSize) = 0;
	//#97_get_mesh_folder
	virtual int exe_get_path(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) = 0;
	virtual int exe_cmd_vol_render(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, ThProcLib_Image2DData* outImage);
	//#123_request_next_render_cmd_from_DLL
	//added : int &next_render_cmds
	virtual int exe_cmd_Op_render(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param,int &next_render_cmds);
	virtual int exe_cmd_ROI_Pen_change(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, int& next_render_cmds);
	virtual int exe_cmd_ROI_Pen_setting(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, int& next_render_cmds);
	virtual int exe_cmd_ROI_final(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, int& next_render_cmds);
	virtual int exe_cmd_ROI_Clear(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, int& next_render_cmds);
	virtual int exe_cmd_proc_mouseEvent(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, int& next_render_cmds);
	 
	virtual int finish_cmd_async(int cmdID, int cmdUID) = 0;
	//#29_command_added
	//virtual int proc_CT_Marker(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) = 0;
	//#168_changed_CT-Marker_to_th_lib_exe_render
	virtual int proc_CT_Marker(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, int& next_render_cmds) = 0;
	virtual int proc_ROI(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) = 0;
	virtual int proc_Output(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret) = 0;
	virtual int release_Render(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret);
	int exe_cmd_async(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret = nullptr, ThProcLib_Vol3DData* outVol=nullptr) ;
	virtual bool cancel_import_CDROM(ThProcLib_CmdStatus *cmdSts) {return true;}
	virtual int proc_load_vol_in(void) ;
	virtual void setupDisplay3Dmodel(int DispParts) {};
	virtual void getLastError(ThProcLib_CmdStatus* status);
	//#120_cleanup_outImage_for_th_lib_exe_render
	void setupNullOutImage(ThProcLib_Image2DData* outImage);
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
	void LogMessage(const wchar_t* fmt, ...) const override;
	std::wstring m_logFileName;

	ThProcLib_Env m_lib_env;
	int m_import_CDROM_timeout_sec;//Sec
	int m_AiSeg_timeout_sec;//Sec
	int m_cmd_uid_count;
	int m_last_cmdID;
	int m_last_cmdUID;
	int m_lib_mode;
	//
	eSeriesType m_curSeriesType;//remember the SeriesType
	//
	ThProcLib_PatientInfo m_new_data;
	//
	std::map<int, std::unique_ptr<ThProcLib_CmdStatus>> m_cmd_uid_map;
	//
	ExeThreader m_thread_cmd_async;
	//
	ThProcLib_Cmd m_current_cmd;
	int m_current_volID;
	//
	std::map<int, ThProcLib_Vol3DData> m_Vol3DList;
	int m_Vol3D_gen_number;
	std::map<int, ThProcLib_Image2DData> m_Image2DList;
	int m_Image2D_gen_number;
	std::map<int, ThProcLib_RenderGC> m_RenderGCList;
	int m_RenderGC_gen_number;
	//
	int m_Roi_Pen_Status;
	int m_Roi_Pen_Width;
	//
	int m_log_level;
	//
	eProcStatus m_procStatus;
	//#167_last_error
	std::wstring m_last_error_code;
	std::wstring m_last_error_msg;
};
template<class T>
class CThreadProc : public CThProcIf
{
public:
	CThreadProc(T* pMain, int cmdID, int cmd_uid) :
		m_pMain(pMain), m_cmd_uid(cmd_uid)
		, m_cmdID(cmdID)
		, m_bCancelFlag(false)//#205_Cancel_import_data_added_cancel_flag
	{

	}
	//#205_Cancel_import_data_added_cancel_flag
	void cancelProc(void) override
	{
		m_bCancelFlag = true;
	}
protected:
	int m_cmdID;
	T* m_pMain;
	int m_cmd_uid;
	bool m_bCancelFlag;
};

#define _copy_wstr( dest_buff,src_str)  wcscpy_s((dest_buff),sizeof(dest_buff)/sizeof(wchar_t),(src_str));
#define _copy_str( dest_buff,src_str)  strncpy_s((dest_buff),sizeof(dest_buff) ,(src_str),sizeof(dest_buff));

#define Trace_Entry _TraceLog enter_log(__FUNCTION__, this, true);
#define Trace_Entry_L1 _TraceLog enter_log(__FUNCTION__, this, m_log_level>=1);
#define Trace_Entry_L2 _TraceLog enter_log(__FUNCTION__, this, m_log_level>=2);
#endif //TH_PROC_LIB_MAIN_H