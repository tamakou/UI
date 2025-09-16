 
#include "ThProcLibMain.h"
#include "ThStringUtils.h"
#include "../rapidjson/document.h"
#include "../rapidjson/prettywriter.h"

#include "MultiThExe.h"
#include "windows.h"
#include "time.h"
#include <map>
#include <memory>
#include <filesystem>
namespace fs = std::filesystem;

 
CThProcLogger::_TraceLog::_TraceLog(const std::string& str, CThProcLogger* owner, bool enable):
		m_owner(owner)
		,m_enable(enable)
	{
		if (m_enable) {
			m_str = ThStringUtils::_convertStr(str);
			m_owner = owner;
			m_owner->LogMessage((m_str + L"--- Enter \n").c_str());
		}
	}
CThProcLogger::_TraceLog::~_TraceLog(void) {
		if (m_enable) {
			m_owner->LogMessage((m_str + L"--- Leave \n").c_str());
		}
	}

CThProcMain::CThProcMain() 
{
	m_curSeriesType = ThLibSeriesType_Unknown;
	m_log_level = 0;
	m_Vol3D_gen_number=m_Image2D_gen_number=m_RenderGC_gen_number=0;
	m_procStatus = eProcStatus_Unknown;
}
CThProcMain::~CThProcMain()
{

}
#include <time.h>
int CThProcMain::th_lib_init(int mode)
{
	m_lib_mode = mode;
	m_cmd_uid_count = 1;
	m_last_cmdID = 0;
	m_last_cmdUID = 0;
	//
	_copy_wstr(m_lib_env.HomeFolder, L"C:/ZeusHome");
	_copy_wstr(m_lib_env.OutputFolder, L"C:/ZeusHome/MeshData");
	_copy_wstr(m_lib_env.DcmRawData, L"C:/ZeusHome/DcmRawData");
	m_lib_env.LogLevel = 0;
	m_import_CDROM_timeout_sec = 8 * 60;
	m_AiSeg_timeout_sec = 10 * 60;
 	bool result = fs::create_directories(m_lib_env.HomeFolder);
	std::wstring log_folder = std::wstring(m_lib_env.HomeFolder) + L"/log";
	result = fs::create_directories(log_folder);

	m_logFileName = log_folder +L"/CppDll.log";
	{
		time_t result = time(NULL);
		wchar_t time_str[64];
		_wctime_s(time_str, sizeof(time_str) / sizeof(wchar_t), &result);
		LogMessage(L">>>=== th_lib_init mode[%d] : %s \n ", mode, time_str);
	}
	LogMessage(L" import_CDROM_timeout_sec %d Sec.\n", m_import_CDROM_timeout_sec);
	LogMessage(L" AiSeg_timeout_sec %d Sec.\n", m_AiSeg_timeout_sec);

	return TH_LIB_EXIT_SUCCESS;
}
void CThProcMain::resetCmdOutput(ThProcLib_CmdOutput* cmd_ret)
{
	cmd_ret->cmdType = 0; 
	cmd_ret->cmdID = -1;   
	cmd_ret->cmdUID = 0;   
	cmd_ret->cdmExtDataSize = 0;  
	cmd_ret->strParam1[0] = 0;
	cmd_ret->strParam2[0] = 0;
	cmd_ret->intParam = 0;
	cmd_ret->floatParam = 0.0f;
}

int CThProcMain::th_lib_setup_env(const ThProcLib_Env* env)
{
	_copy_wstr(m_lib_env.HomeFolder, env->HomeFolder);
	_copy_wstr(m_lib_env.OutputFolder, env->OutputFolder);
	_copy_wstr(m_lib_env.DcmRawData, env->DcmRawData);
	m_lib_env.LogLevel = env->LogLevel;
	return TH_LIB_EXIT_SUCCESS;
}
int CThProcMain::th_lib_get_env(ThProcLib_Env* env)
{
	memset(env, 0, sizeof(ThProcLib_Env));
	_copy_wstr(env->HomeFolder, m_lib_env.HomeFolder);
	_copy_wstr(env->OutputFolder, m_lib_env.OutputFolder);
	_copy_wstr(env->DcmRawData, m_lib_env.DcmRawData);
	env->LogLevel = m_lib_env.LogLevel;
	env->ThProLibDummy = 0;
	return TH_LIB_EXIT_SUCCESS;
}
void CThProcMain::selfTest(void)
{

}

int CThProcMain::th_lib_exe_cmd(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)
{
	memset(cmd_ret, 0, sizeof(ThProcLib_CmdOutput));
	if ((cmd->cmdID > ThLibCmd_Render_Release_Start) &&
		(cmd->cmdID < ThLibCmd_Render_Release_End)
		) {
		return  release_Render(cmd, cmd_ret);
	}
	LogMessage(L"th_lib_exe_cmd cmdID:%d entry\n", cmd->cmdID);
	LogMessage(L"   --[%s] \n", get_cmd_name(cmd->cmdID).c_str());
	int iRetSts = TH_LIB_EXIT_FAILURE;
	memcpy(&m_current_cmd, cmd, sizeof(ThProcLib_Cmd));
	switch (cmd->cmdID) {
	case ThLibCmd_Exe_ImportData:
		iRetSts = exe_cmd_import(cmd, cmd_ret);
		break;
	case ThLibCmd_Exe_AsyncCmd_Cancel://#205_Cancel_import_data
		iRetSts = exe_cmd_async_cancel(cmd);
		break;
	case ThLibCmd_Exe_AISegmentator:
		iRetSts = exe_cmd_AiSegmentator(cmd, cmd_ret);
		break;
#if 0
	//#168_changed_CT-Marker_to_th_lib_exe_render
	case ThLibCmd_CT_Marker_Clear:
	case ThLibCmd_CT_Marker_Add:			 
	case ThLibCmd_CT_Marker_Final:
		break;
#endif
	case ThLibCmd_ROI_Clear:
	case ThLibCmd_ROI_Add:
	case ThLibCmd_ROI_Erase:
	case ThLibCmd_ROI_Final:
		iRetSts = proc_ROI(cmd, cmd_ret);
		break;
	case ThLibCmd_Exe_DeleteData:
		//#210_delete_imported_data
		iRetSts = exe_cmd_delete_import_data(cmd, cmd_ret);
		break;
	case ThLibCmd_Output_Remove:			//40
	case ThLibCmd_Output_Add:
		iRetSts = proc_Output(cmd, cmd_ret);
		break;
	case ThLibCmd_Render_Load_Vol3D:
		m_procStatus = eProcStatus_VolLoading;
		iRetSts = exe_cmd_load_vol(cmd, cmd_ret);
		break;
	case ThLibCmd_Render_Load_3DModel://#103_load3DModel_command
		iRetSts = exe_cmd_load_3DModle(cmd, cmd_ret);
		break;
	case ThLibCmd_Get_StudyPath:
	case ThLibCmd_Get_MeshPath:
	//#97_get_mesh_folder
		iRetSts = exe_get_path(cmd, cmd_ret);
		break;
	case ThLibCmd_Check_Image_Server:
		//#137_check_image_server
		iRetSts = exe_cmd_check_image_server(cmd, cmd_ret);
		break;
	}
	LogMessage(L"th_lib_exe_cmd cmdID:%d leave ->cmdUID:%d,cmdType:%d \n", cmd->cmdID, cmd_ret->cmdUID, cmd_ret->cmdType);
	return iRetSts;
}
//#205_Cancel_import_data
int CThProcMain::exe_cmd_async_cancel(const ThProcLib_Cmd* cmd)
{
	int cmdUID = cmd->intParam;
	if (cmdUID == ThLibCmd_Unknown) {
		 
		return TH_LIB_EXIT_SUCCESS;
	}
 
	for (auto it = m_cmd_uid_map.begin(); it != m_cmd_uid_map.end(); it++) {
		if (it->first == cmdUID) {
			it->second->cmdStatus = ThLibCmdStatus_Failed;
			switch (it->second->cmdID) {
			case ThLibCmd_Exe_ImportData:
				_copy_wstr(it->second->errMessage, L"Import Data is Canceled");
				if (!cancel_import_CDROM(it->second.get())) {
					return TH_LIB_EXIT_FAILURE;
				};
				break;
			};
			break;
		}
	}
	if (m_thread_cmd_async.m_proc_hdr) {//#205_Cancel_import_data_added_cancel_flag
		m_thread_cmd_async.m_proc_hdr->cancelProc();
	}
	return TH_LIB_EXIT_SUCCESS;
}
int CThProcMain::th_lib_get_status(int cmdUID, ThProcLib_CmdStatus* status)
{
	status->dataID = 0;
	if (cmdUID == ThLibCmd_Unknown) {
		status->cmdStatus = -1;
		//#167_last_error
		getLastError(status);
		return TH_LIB_EXIT_SUCCESS;
	}
	status->cmdID = m_last_cmdID;

	int cmd_progress = 0;
	bool b_found = false;
	std::wstring strParam1;
	std::wstring errorStr;
	int new_data_id;
	int cmd_status = ThLibCmdStatus_Progress;
	for (auto it = m_cmd_uid_map.begin(); it != m_cmd_uid_map.end(); it++) {
		if (it->first == cmdUID) {
			cmd_progress = it->second->cmdProgress;
			strParam1 = it->second->strParam1;
			new_data_id = it->second->dataID;
			cmd_status = it->second->cmdStatus;
			if (cmd_status == ThLibCmdStatus_Failed) {
				errorStr = it->second->errMessage;
			}
			b_found = true;
		}
	}
	if (b_found) {
		status->cmdProgress = cmd_progress;
		_copy_wstr(status->strParam1, strParam1.c_str());
		status->dataID = new_data_id;
		status->cmdStatus = cmd_status;
	}
	if (status->cmdProgress >= 100) {
		status->cmdStatus = ThLibCmdStatus_Succeeded;
		//new dataID
		//status->dataID = g_datalist_count++;
		if (status->cmdID == ThLibCmd_Exe_ImportData) {
		//	status->dataID = get_new_data_ID();
		}
		LogMessage(L"th_lib_get_status : Succeeded \n");

	}
	else {
		status->cmdStatus = ThLibCmdStatus_Progress;
	}
	if (cmd_status == ThLibCmdStatus_Failed) {
		status->cmdStatus = ThLibCmdStatus_Failed;
		_copy_wstr(status->errMessage, errorStr.c_str());
	}
	return TH_LIB_EXIT_SUCCESS;
}
int CThProcMain::th_lib_close_cmd(int cmdUID)
{
	LogMessage(L"th_lib_close_cmd cmdUID:%d \n", cmdUID);
	if (cmdUID <= ThLibCmd_Unknown) {
		return TH_LIB_EXIT_FAILURE;
	}

	for (auto it = m_cmd_uid_map.begin(); it != m_cmd_uid_map.end(); it++) {
		if (it->first == cmdUID) {
			m_cmd_uid_map.erase(it);
			break;
		}
	}
	return TH_LIB_EXIT_SUCCESS;
}
//
int CThProcMain::exe_cmd_async(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret,ThProcLib_Vol3DData* outVol)
{
	if (cmd_ret != nullptr) {
		CThProcMain::resetCmdOutput(cmd_ret);
	
		cmd_ret->cmdID = cmd->cmdID;
		cmd_ret->cmdType = ThLibCmdType_Async;
		cmd_ret->cmdUID = generateCmdUId();
	}
	if (m_thread_cmd_async.m_theExe != nullptr) {
		m_thread_cmd_async.m_theExe->wait();
	}

	m_cmd_uid_map[cmd_ret->cmdUID] = std::unique_ptr<ThProcLib_CmdStatus>(new ThProcLib_CmdStatus);
	ThProcLib_CmdStatus* p_cmd_status = m_cmd_uid_map[cmd_ret->cmdUID].get();
	p_cmd_status->cmdID = cmd->cmdID;
	p_cmd_status->cmdProgress = 0;
	p_cmd_status->cmdStatus = ThLibCmdStatus_Progress;
	p_cmd_status->dataID = 0;
	p_cmd_status->strParam1[0] = 0;
	p_cmd_status->errMessage[0] = 0;
	m_last_cmdUID = cmd_ret->cmdUID;
	m_last_cmdID = cmd->cmdID;

	return TH_LIB_EXIT_SUCCESS;
}

void CThProcMain::LogMessage(const wchar_t* fmt, ...) const
{
	try
	{
		va_list args;
		va_start(args, fmt);
		bool bFileOutputed = false;
		if (m_logFileName.size() > 1) {
			 
			FILE *_log_file = nullptr;
			_wfopen_s(&_log_file, m_logFileName.c_str(), L"at+");
			if (_log_file != nullptr) {
				wchar_t  time_str[128];
				{
					struct tm when;
					time_t tme = time(NULL);
					localtime_s(&when, &tme);
				//	when = *localtime(&tme);
					int year = when.tm_year + 1900;
					int mon  = when.tm_mon + 1;
					int day  = when.tm_mday;
					int hour = when.tm_hour;
					int min  = when.tm_min;
					int sec  = when.tm_sec;
					swprintf(time_str, 128, L"%04d_%02d_%02d %2d:%02d:%02d",
						year, mon, day,
						hour, min, sec);
				}
				fwprintf(_log_file, L"%s ", time_str);
				bFileOutputed = true;
				vfwprintf_s(_log_file, fmt, args);
				fclose(_log_file);
			}
		}
		if (!bFileOutputed) {
			vwprintf(fmt, args);
		}

		va_end(args);
	}
	catch (...)
	{

	}
}
std::wstring CThProcMain::get_cmd_name(int cmdID)
{
	std::wstring strCmd = L" ";
	switch (cmdID) {
	case ThLibCmd_Unknown:
		break;
	case ThLibCmd_Exe_ImportData:
		strCmd = L"Import Data"; break;
	case ThLibCmd_Exe_OutputData:
		strCmd = L"OutputData"; break;
	case ThLibCmd_Exe_DeleteData:
		strCmd = L"DeleteData"; break;
	case ThLibCmd_Exe_AISegmentator:
		strCmd = L"AISegmentator"; break;
	case ThLibCmd_Exe_USB_Output:
		strCmd = L"USB_Output"; break;
		//-----------
	case ThLibCmd_Query_CTSlice:// 10,
	case ThLibCmd_Query_CTMarker2D:	//11
	case ThLibCmd_Query_CTMarker3D:	//12
	case ThLibCmd_Query_CTRoi2D:		//13
	case ThLibCmd_Query_CTRoi3D:		//14
		break;
			//-----------
	case ThLibCmd_CT_Marker_Clear:	//20
		strCmd = L"Marker_Clear"; break;
	case ThLibCmd_CT_Marker_Add:			//21
		strCmd = L"Marker_Add"; break;
	case ThLibCmd_CT_Marker_Final:			//2
		strCmd = L"Marker_Final"; break;
		//-----------
	case ThLibCmd_ROI_Clear:			//30
		strCmd = L"ROI_Clear"; break;
	case ThLibCmd_ROI_Add:				//31
		strCmd = L"ROI_Add"; break;
	case ThLibCmd_ROI_Erase:			//32
		strCmd = L"ROI_Erase"; break;
	case ThLibCmd_ROI_Final:			//33
		strCmd = L"ROI_Final"; break;
		//----------
	case ThLibCmd_Output_Remove:			//40
		strCmd = L"Output_Remove"; break;
	case ThLibCmd_Output_Add:			//41
		strCmd = L"Output_Add"; break;
		//-----------
	case ThLibCmd_Get_StudyPath:// 50,
	case ThLibCmd_Get_DicomImageInfo: //51         // th_lib_get_image_infoÇ…éwíËÅA/ThProcLib_DicomImageInfo
	case ThLibCmd_Query_Setting:// 99,
		//
	case ThLibCmd_Render_Load_Vol3D:	//125
		strCmd = L"Load_Vol3D"; break;
	case ThLibCmd_Render_Load_3DModel://126   //#103_load3DModel_command
		strCmd = L"Load_3DModel"; break;
		break;
	}
	return strCmd;
}
int  CThProcMain::generate_vol3D_ID(void)
{
	int new_id = m_Vol3D_gen_number++;
	if (new_id < 0) {
		m_Vol3D_gen_number = 1;
		new_id = 0;
	}
	return new_id;
}
int  CThProcMain::generate_image2D_ID(void)
{
	int new_id = m_Image2D_gen_number++;
	if (new_id < 0) {
		m_Image2D_gen_number = 1;
		new_id = 0;
	}
	return new_id;
}
int  CThProcMain::generate_renderGC_ID(void)
{
	int new_id = m_RenderGC_gen_number++;
	if (new_id < 0) {
		m_RenderGC_gen_number = 1;
		new_id = 0;
	}
	return new_id;
}
void CThProcMain::release_vol3D_ID(int ID)
{
	bool bFound = false;
	for (auto it = m_Vol3DList.begin(); it != m_Vol3DList.end(); it++) {
		if (ID == it->second.volID) {
			bFound = true;
			break;
		}
	}
	if (bFound)
	m_Vol3DList.erase(ID);
}
void CThProcMain::release_image2D_ID(int ID)
{
	bool bFound = false;
	for (auto it = m_Image2DList.begin(); it != m_Image2DList.end(); it++) {
		if (ID == it->second.imageID) {
			bFound = true;
			break;
		}
	}
	if(bFound)
	m_Image2DList.erase(ID);
}
void CThProcMain::release_renderGC_ID(int ID)
{
	bool bFound = false;
	for (auto it = m_RenderGCList.begin(); it != m_RenderGCList.end(); it++) {
		if (ID == it->second.render_gc_id) {
			bFound = true;
			break;
		}
	}
	if (bFound)
		m_RenderGCList.erase(ID);
}
ThProcLib_Vol3DData* CThProcMain::get_vol3D_ID(int ID)
{
	ThProcLib_Vol3DData *retPtr = nullptr;
	for (auto it = m_Vol3DList.begin(); it != m_Vol3DList.end(); it++) {
		if (ID == it->second.volID) {
			retPtr = &(it->second);
			break;
		}
	}
	return retPtr;
}
ThProcLib_Image2DData* CThProcMain::get_image2D_ID(int ID)
{
	ThProcLib_Image2DData* retPtr = nullptr;
	for (auto it = m_Image2DList.begin(); it != m_Image2DList.end(); it++) {
		if (ID == it->second.imageID) {
			retPtr = &(it->second);
			break;
		}
	}
	return retPtr;
}
ThProcLib_RenderGC* CThProcMain::get_renderGC_ID(int ID)
{
	ThProcLib_RenderGC* retPtr = nullptr;
	for (auto it = m_RenderGCList.begin(); it != m_RenderGCList.end(); it++) {
		if (ID == it->second.render_gc_id) {
			retPtr = &(it->second);
			break;
		}
	}
	return retPtr;
}
///#43_rendering_command
int CThProcMain::th_lib_get_vol_info(const ThProcLib_Cmd* cmd, ThProcLib_Vol3DData* outVol)
{
	return 0;
}
int CThProcMain::proc_load_vol_in(void)
{
	m_Roi_Pen_Status = ThLibCmd_ROI_Add;//default is Pen-add as same as UI 
	// ThLibCmd_Unknown;
	m_Roi_Pen_Width = 2;

	const ThProcLib_Cmd* cmd = &m_current_cmd;
	ThProcLib_Vol3DData new_vol;
	new_vol.volID = generate_vol3D_ID();
	
	new_vol.sizeX = 512;
	new_vol.sizeY = 512;
	new_vol.sizeZ = 360;
	new_vol.ptichX = 0.2;
	new_vol.ptichY = 0.2;
	new_vol.ptichZ = 1.0f;
	_copy_wstr(new_vol.StudyUID ,cmd->strParam1);
	this->m_Vol3DList[new_vol.volID] = new_vol;
	m_current_volID = new_vol.volID;
	m_procStatus = eProcStatus_VolLoaded;
	return TH_LIB_EXIT_SUCCESS;
}
//#208_delete_output_file
int CThProcMain::delete_output_in(void)
{
	return TH_LIB_EXIT_SUCCESS;
}
int CThProcMain::th_lib_render_GC(const ThProcLib_Cmd* cmd, ThProcLib_RenderGC* outGC)
{
	if (cmd->cmdID == ThLibCmd_Render_Create_GC) {
		ThProcLib_RenderGC new_render_GC;
		memset(&new_render_GC, 0, sizeof(ThProcLib_RenderGC));
		int newID = generate_renderGC_ID();
		new_render_GC.render_gc_id = newID;
		m_RenderGCList[newID] = new_render_GC;
		*outGC = new_render_GC;
	}
	else {
		int gc_id =cmd->intParam;
		ThProcLib_RenderGC* p_gc = CThProcMain::get_renderGC_ID(gc_id);
		if (p_gc != nullptr) {
	//		*outGC = *p_gc;
		}
	}
	return TH_LIB_EXIT_SUCCESS;
}
//#120_cleanup_outImage_for_th_lib_exe_render
void CThProcMain::setupNullOutImage(ThProcLib_Image2DData* outImage)
{
	ThProcLib_Image2DData new_image;
	memset(&new_image, 0, sizeof(ThProcLib_Image2DData));
#if 0
	int newID = generate_image2D_ID();
	new_image.imageID = newID;
	m_Image2DList[newID] = new_image;
#else
	new_image.imageID = -1;
#endif
	* outImage = new_image;
}
int CThProcMain::th_lib_exe_render(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, ThProcLib_Image2DData* outImage)
{
	int ret_sts = TH_LIB_EXIT_FAILURE;
	setupNullOutImage(outImage);//#120_cleanup_outImage_for_th_lib_exe_render
	outImage->next_render_cmds = ThLibRRCmd_Unknown;
	switch (cmd->cmdID) {
	case ThLibCmd_Render_Vol3D:
		ret_sts = exe_cmd_vol_render(cmd, param, outImage);
		break;
	case ThLibCmd_Render_Op:
	 	ret_sts = exe_cmd_Op_render(cmd, param, outImage->next_render_cmds);
		break;
	case ThLibCmd_ROI_Add:
		ret_sts = exe_cmd_ROI_Pen_change(cmd, param, outImage->next_render_cmds);
		break;
	case ThLibCmd_ROI_Erase:
		ret_sts = exe_cmd_ROI_Pen_change(cmd, param, outImage->next_render_cmds);
		break;
	case ThLibCmd_ROI_SetPen:
		ret_sts = exe_cmd_ROI_Pen_setting(cmd, param, outImage->next_render_cmds);
		break;
	case ThLibCmd_ROI_Final:
		ret_sts = exe_cmd_ROI_final(cmd, param, outImage->next_render_cmds);
		break;
	case ThLibCmd_ROI_Clear:
		ret_sts = exe_cmd_ROI_Clear(cmd, param, outImage->next_render_cmds);
		break;
		//
	case ThLibCmd_CT_Marker_Clear:
	case ThLibCmd_CT_Marker_Add:
	case ThLibCmd_CT_Marker_Final:
		//#168_changed_CT-Marker_to_th_lib_exe_render
		ret_sts = proc_CT_Marker(cmd, param, outImage->next_render_cmds);
		break;
		 
	}
	return ret_sts;
}
int CThProcMain::exe_cmd_vol_render(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, ThProcLib_Image2DData* outImage)
{
	int volID = cmd->intParam;
	if (get_vol3D_ID(volID) == 0) {
		return TH_LIB_EXIT_FAILURE;
	}
	ThProcLib_Image2DData new_image;
	int newID = generate_image2D_ID();
	new_image.imageID = newID;
	m_Image2DList[newID] = new_image;
	*outImage = new_image;

	return TH_LIB_EXIT_SUCCESS;
}
int CThProcMain::exe_cmd_Op_render(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, int& next_render_cmds)
{
	int volID = cmd->intParam;
	if (get_vol3D_ID(volID) == 0) {
		return TH_LIB_EXIT_FAILURE;
	}
 
	switch (param->render_cmd_major)
	{
	case ThLibRenderCmd_Setup://give-up, 2025/08/13
							  //changed to ThLibRenderCmd_Setup_Series/ThLibRenderCmd_Setup_3D_Obj
	{
		;
		if (param->display_parts != 0) {
			this->setupDisplay3Dmodel(param->display_parts);
		}
		else {
			m_curSeriesType = (eSeriesType)param->series_type;
		}
		//#123_request_next_render_cmd_from_DLL
		next_render_cmds |= ThLibRRCmd_2D_1;
		next_render_cmds |= ThLibRRCmd_3D;
	}
	break;
	case ThLibRenderCmd_Setup_Series:
	{//#160_changed_ThLibRenderCmd_Setup
		 
		m_curSeriesType = (eSeriesType)param->series_type;
		 
		//#123_request_next_render_cmd_from_DLL
		next_render_cmds |= ThLibRRCmd_2D_1;
		next_render_cmds |= ThLibRRCmd_3D;
	}
	break;
	case ThLibRenderCmd_Setup_3D_Obj:
	{//#160_changed_ThLibRenderCmd_Setup
		 
		this->setupDisplay3Dmodel(param->display_parts);
		 
		//#123_request_next_render_cmd_from_DLL
		next_render_cmds |= ThLibRRCmd_2D_1;
		next_render_cmds |= ThLibRRCmd_3D;
	}
	break;
	case ThLibRenderCmd_Change_Case:
		next_render_cmds |= ThLibRRCmd_2D_1;
		next_render_cmds |= ThLibRRCmd_3D;
		break;
	case ThLibRenderCmd_ProcMouse:
		exe_cmd_proc_mouseEvent(cmd, param, next_render_cmds);
	break;
	}
	return TH_LIB_EXIT_SUCCESS;
}

int CThProcMain::release_Render(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)
{
	switch (cmd->cmdID) {
	case ThLibCmd_Release_Vol3D:
		release_vol3D_ID(cmd->intParam);
		break;
	case ThLibCmd_Release_Image2D:
		release_image2D_ID(cmd->intParam);
		
		break;
	case ThLibCmd_Release_GC:
		release_renderGC_ID(cmd->intParam);
		break;
	}
	 
	return TH_LIB_EXIT_SUCCESS;
}
int CThProcMain::th_lib_get_data(const ThProcLib_Cmd* cmd, unsigned char* DataBuffer, int BufferSize)
{
	int ret_sts = TH_LIB_EXIT_FAILURE;
	switch (cmd->cmdID) {
	case ThLibCmd_Render_Image2D:
		ret_sts = exe_get_image_data( cmd,  DataBuffer,  BufferSize) ;
		break;
	}
	return ret_sts;
}
int CThProcMain::exe_cmd_ROI_Pen_change(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, int& next_render_cmds)
{
	m_Roi_Pen_Status = cmd->cmdID;
	return TH_LIB_EXIT_SUCCESS;
}
int CThProcMain::exe_cmd_ROI_Pen_setting(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, int& next_render_cmds)
{
	m_Roi_Pen_Width = cmd->intParam;
	return TH_LIB_EXIT_SUCCESS;
}
int CThProcMain::exe_cmd_ROI_final(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, int& next_render_cmds)
{
	 
	return TH_LIB_EXIT_SUCCESS;
}
int CThProcMain::exe_cmd_ROI_Clear(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, int& next_render_cmds)
{

	return TH_LIB_EXIT_SUCCESS;
}
int CThProcMain::exe_cmd_proc_mouseEvent(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, int& next_render_cmds)
{
	switch (param->mouse_type)
	{
	case ThLibMouseEvent_ROI_Pen:
		LogMessage(L"ROI_pen: [%d,%d]-%d,%d \n", 
			param->target_sizeX, param->target_sizeY,
			param->mouse_posX, param->mouse_posY);
		//#123_request_next_render_cmd_from_DLL
		next_render_cmds |= ThLibRRCmd_2D_1;
		next_render_cmds |= ThLibRRCmd_3D;
		break;
	case ThLibMouseEvent_ROI_Eraser:
		LogMessage(L"ROI_Eraser: [%d,%d]-%d,%d \n", 
			param->target_sizeX, param->target_sizeY,
			param->mouse_posX, param->mouse_posY);
		//#123_request_next_render_cmd_from_DLL
		next_render_cmds |= ThLibRRCmd_2D_1;
		next_render_cmds |= ThLibRRCmd_3D;
		break;
	}
	return TH_LIB_EXIT_SUCCESS;
}
void CThProcMain::getLastError(ThProcLib_CmdStatus* status)
{
	//#167_last_error
	_copy_wstr(status->strParam1, m_last_error_code.c_str());
	_copy_wstr(status->errMessage, m_last_error_msg.c_str());
	m_last_error_code.clear();
	m_last_error_msg.clear();
	
}