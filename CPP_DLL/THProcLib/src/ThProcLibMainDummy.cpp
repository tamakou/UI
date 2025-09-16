 
#include "ThProcLibMainDummy.h"
#include "IThPCCoreLibDummy.h"
#include "ThStringUtils.h"
#include "../rapidjson/document.h"
#include "../rapidjson/prettywriter.h"
#include <thread>
#include <chrono>
#include "MultiThExe.h"
std::wstring replaceAll(std::wstring str, const std::wstring& from, const std::wstring& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::wstring::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Move past the replaced substring
	}
	return str;
}
std::wstring makeStudyFolder(const std::wstring& date, const std::wstring& time, const std::string& studyUID) 
{
	std::wstring study_folder = date + L"_" + time + L"_" + ThStringUtils::_convertStr(studyUID);
	//study_folder.replace(study_folder.begin(), study_folder.end(), L'/', L' ');  
	study_folder = replaceAll(study_folder, L"/", L"");
	study_folder = replaceAll(study_folder, L":", L"");
	return study_folder;
}


class CMyUpdateImportProc : public CThreadProc< CThProcMainDummy>
{
public:
	friend CThProcMainDummy;
	CMyUpdateImportProc(CThProcMainDummy *pMain,int cmd_uid):
		CThreadProc< CThProcMainDummy>(pMain, ThLibCmd_Exe_ImportData,cmd_uid)
	{

	}
	int doProc(int th_id) override
	{
		for (int i = 0; i < 200; i++) {
			::_sleep(100);
//			_cmd_uid_map[m_cmd_uid]++;
			if(m_pMain->check_cmd_async(m_cmdID, m_cmd_uid)== TH_LIB_EXIT_SUCCESS) break;
		}
		return 0;
	};
protected:
 
};
class CMyUpdateAiSegProc : public CThreadProc< CThProcMainDummy>
{
public:
	friend CThProcMainDummy;
	CMyUpdateAiSegProc(CThProcMainDummy* pMain, int cmd_uid, int timeout) :
		CThreadProc< CThProcMainDummy>(pMain, ThLibCmd_Exe_AISegmentator, cmd_uid)
		, m_timeout_sec(timeout)
	{

	}
	int doProc(int th_id) override
	{
		int base_time = 200;//mSec
		int toop_n = m_timeout_sec * 1000.0 / base_time;
		int time_count = 0;
		for (int i = 0; i < toop_n; i++) {
			if (m_pMain->check_cmd_async(ThLibCmd_Exe_AISegmentator, m_cmd_uid) == TH_LIB_EXIT_SUCCESS)  {
				break;
			}
			else {
				std::this_thread::sleep_for(std::chrono::milliseconds(base_time));
			}
		}
		return 0;
	};
	 
protected:
	 
	int m_timeout_sec;
};
class CMyLoadVolProc : public CThreadProc< CThProcMainDummy>
{
public:
	friend CThProcMainDummy;
	CMyLoadVolProc(CThProcMainDummy* pMain, int cmd_uid) :
		CThreadProc< CThProcMainDummy>(pMain, ThLibCmd_Render_Vol3D, cmd_uid)
	{

	}
	int doProc(int th_id) override
	{
		m_pMain->proc_load_vol_in();
		for (int i = 0; i < 200; i++) {
			::_sleep(20);
			//			_cmd_uid_map[m_cmd_uid]++;
			if (m_pMain->check_cmd_async(m_cmdID, m_cmd_uid) == TH_LIB_EXIT_SUCCESS) break;
		}
		
		return 0;
	};
protected:
 
};
//#32_ThLibCmd_Output_as_async
class CMyUpdateOutputProc : public CThreadProc< CThProcMainDummy>
{
public:
	friend CThProcMainDummy;
	CMyUpdateOutputProc(CThProcMainDummy* pMain, int cmdID,int cmd_uid) :
		CThreadProc< CThProcMainDummy>(pMain, cmdID, cmd_uid)
	{

	}
	int doProc(int th_id) override
	{
		for (int i = 0; i < 200; i++) {
			::_sleep(20);
			//			_cmd_uid_map[m_cmd_uid]++;
			if (m_pMain->check_cmd_async(m_cmdID, m_cmd_uid) == TH_LIB_EXIT_SUCCESS) break;
		}
		return 0;
	};
protected:
	 
};
static
int _cntCmdParam2CoreLibParam(int cmd_intParam)
{
	int out_param = 0;
	switch (cmd_intParam) {
	case ThLibCmdOutput_Xreal:
		out_param = ThPCCoreLibDummy::eProcStatus_Xreal;
		break;
	case ThLibCmdOutput_iPhone:
		out_param = ThPCCoreLibDummy::eProcStatus_iPhone;
		break;
	case ThLibCmdOutput_Xreal_iPhone:
		out_param = ThPCCoreLibDummy::eProcStatus_Xreal | ThPCCoreLibDummy::eProcStatus_iPhone;
		break;
	}
	return out_param;
}
CThProcMainDummy::CThProcMainDummy()
{

}
CThProcMainDummy::~CThProcMainDummy()
{

}
extern void init_data_list_DB();
int CThProcMainDummy::th_lib_init(int mode)
{
	CThProcMain::th_lib_init(mode);
	init_data_list_DB();
	LogMessage(L" CThProcMainDummy::th_lib_init\n");

	return TH_LIB_EXIT_SUCCESS;
}
int CThProcMainDummy::th_lib_setup_env(const ThProcLib_Env* env)
{
	return CThProcMain::th_lib_setup_env(env);
}
int CThProcMainDummy::th_lib_get_env(ThProcLib_Env* env)
{
	int sts= CThProcMain::th_lib_get_env(env);
	env->ThProLibDummy = 1;
	_copy_wstr(env->ImportFolder, L"D:\\"); //#181_added_import_path
	//#191_Model_Version
	_copy_wstr(env->ModelVers,
		L"ThDcm2Mesh: Ver1.0.8.2 \n"
		L"ThProcLib: Ver1.0.4.3 \n"
		L"ThPCCoreLib: Ver1.0.6.3 \n"
		L"ThDcmMgrLib: Ver1.0.5.3 \n"
		L"ImageServer: Ver4.0.1.0 "
	);
	return sts;
}

int CThProcMainDummy::th_lib_exe_cmd(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)
{
	int iRetSts = CThProcMain::th_lib_exe_cmd(cmd, cmd_ret);
	return iRetSts;
}

int CThProcMainDummy::exe_cmd_import(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)
{

	CThProcMain::exe_cmd_async(cmd, cmd_ret);

	m_thread_cmd_async.m_proc_hdr.reset(new CMyUpdateImportProc(this,cmd_ret->cmdUID));
	m_thread_cmd_async.m_theExe.reset(new CMultiThExe(1, m_thread_cmd_async.m_proc_hdr.get()));

	return TH_LIB_EXIT_SUCCESS;
}
int CThProcMainDummy::exe_cmd_async_cancel(const ThProcLib_Cmd* cmd)
{
	return CThProcMain::exe_cmd_async_cancel(cmd);
}

//#210_delete_imported_data
int CThProcMainDummy::exe_cmd_delete_import_data(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)
{
	 
	return TH_LIB_EXIT_SUCCESS;
}
int CThProcMainDummy::th_lib_get_image_info(const ThProcLib_Cmd* cmd, ThProcLib_DicomImageInfo* image_info)
{
 
	wcscpy(image_info->StudyUID, cmd->strParam1);
	image_info->sizeX = 512;
	image_info->sizeY = 512;
	image_info->sizeZ = 512;
	image_info->WindowWidth = 1000;
	image_info->WindowCenter = 200;
	image_info->CheckDig = 2345; //check bit, always  2345;

	return TH_LIB_EXIT_SUCCESS;
}
 

int CThProcMainDummy::exe_get_image_data(const ThProcLib_Cmd* cmd, unsigned char* intDataBuffer, int BufferSize)
{
	RenderImgDummy* pImage= th_lib_get_image_dummy(cmd->intParam);
	if (pImage == nullptr) return TH_LIB_EXIT_FAILURE;

	unsigned char* pbuffer = (unsigned char*)intDataBuffer;
	for (int i = 0; i < BufferSize; i++) {
		pbuffer[i] = pImage->m_image_buffer[i];
	}
	return TH_LIB_EXIT_SUCCESS;
}
 
int CThProcMainDummy::th_lib_get_status(int cmdUID, ThProcLib_CmdStatus* status)
{

	int iSts = CThProcMain::th_lib_get_status(cmdUID, status);
	return iSts;
	 
}
extern int add_new_data(ThProcLib_PatientInfo& out);
int CThProcMainDummy::get_new_data_ID(void)
{
	return add_new_data(m_new_data);
}
int CThProcMainDummy::th_lib_close_cmd(int cmdUID)
{
	return CThProcMain::th_lib_close_cmd(cmdUID);
}
int CThProcMainDummy::check_cmd_async(int cmdID, int cmdUID, int chkNo)
{
	if (cmdUID <= ThLibCmd_Unknown) {
		return TH_LIB_EXIT_FAILURE;
	}
	int ret_val = 1;//in progress
	for (auto it = m_cmd_uid_map.begin(); it != m_cmd_uid_map.end(); it++) {
		if (it->first == cmdUID) {
			//check finish at first
			if ((it->second->cmdProgress + 1) >= 100) {
				finish_cmd_async(cmdID, cmdUID);
			}
			it->second->cmdProgress++;
			if (it->second->cmdProgress >= 100) {
				ret_val = TH_LIB_EXIT_SUCCESS;
				it->second->dataID = this->m_current_volID;
			}
			break;
		}
	}
	 
	return ret_val;
}

int th_lib_get_datalist_in(ThProcLib_PatientInfo* out_info, int no);
int th_lib_get_output_list_in(ThProcLib_PatientInfo* out_info, int no);

int CThProcMainDummy::th_lib_get_datalist(ThProcLib_PatientInfo* out_info, int type, int no)
{
//	LogMessage(L"th_lib_get_datalist type:%d , no:%d\n", type, no);
	if (type == ThLibQueryList_CTData) {
		return th_lib_get_datalist_in(out_info, no);
	}
	else {
		return th_lib_get_output_list_in(out_info, no);
	}

}

int th_lib_get_datalist_size_in(int type);
int th_lib_get_output_list_size_in(int type);
int CThProcMainDummy::th_lib_get_datalist_size(int type)
{
	LogMessage(L"get_datalist_size type:%d \n", type);
	if (type == ThLibQueryList_CTData) {
		return th_lib_get_datalist_size_in(type);
	}
	else {
		return th_lib_get_output_list_size_in(type);
	}
}
int CThProcMainDummy::exe_cmd_AiSegmentator(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)
{
	std::string study_uid = ThStringUtils::_convertStr(cmd->strParam1);

	CThProcMain::exe_cmd_async(cmd, cmd_ret);
 
	m_thread_cmd_async.m_proc_hdr.reset(new CMyUpdateAiSegProc(this, cmd_ret->cmdUID, m_AiSeg_timeout_sec));
	m_thread_cmd_async.m_theExe.reset(new CMultiThExe(1, m_thread_cmd_async.m_proc_hdr.get()));

	return TH_LIB_EXIT_SUCCESS;
}
extern ThPCCoreLibDummy::UDICOMStudy_Def* th_lib_get_datalist_in_DB(std::wstring& study_uid);
int CThProcMainDummy::finish_cmd_async(int cmdID, int cmdUID)
{
	switch (cmdID) {
	case ThLibCmd_Exe_ImportData:
		{
			for (auto it = m_cmd_uid_map.begin(); it != m_cmd_uid_map.end(); it++) {
				if (it->first == cmdUID) {
					int id = get_new_data_ID();
					it->second->dataID = id;
					_copy_wstr(it->second->strParam1, this->m_new_data.StudyUID);
				}
			}
		}
		break;
	case ThLibCmd_Exe_AISegmentator:
		{
			std::wstring study_uid =  m_current_cmd.strParam1;
			ThPCCoreLibDummy::UDICOMStudy_Def* pdata= th_lib_get_datalist_in_DB(study_uid);
			if (pdata != nullptr) {
				pdata->m_procStatus = ThPCCoreLibDummy::eProcStatus_AiSeg;
			}
		}
		break;
	case ThLibCmd_Output_Add: //#32_ThLibCmd_Output_as_async
	{
		for (auto it = m_cmd_uid_map.begin(); it != m_cmd_uid_map.end(); it++) {
			if (it->first == cmdUID) {
				 
				std::wstring study_uid = m_current_cmd.strParam1;
				ThPCCoreLibDummy::UDICOMStudy_Def* org_data = th_lib_get_datalist_in_DB(study_uid);
				if (org_data == nullptr) {
					return TH_LIB_EXIT_FAILURE;
				}
				int procSts = org_data->m_procStatus;

				int CoreLibParam = _cntCmdParam2CoreLibParam(m_current_cmd.intParam);
				if ((CoreLibParam & ThPCCoreLibDummy::eProcStatus_Xreal) != 0 ||
					(CoreLibParam & ThPCCoreLibDummy::eProcStatus_iPhone) != 0
					) {
					procSts = procSts | CoreLibParam;
					org_data->m_procStatus = procSts;
				}
 
			}
		}
	}
	break;
	case ThLibCmd_Output_Remove: //#32_ThLibCmd_Output_as_async
	{
		for (auto it = m_cmd_uid_map.begin(); it != m_cmd_uid_map.end(); it++) {
			if (it->first == cmdUID) {

				std::wstring study_uid = m_current_cmd.strParam1;
				ThPCCoreLibDummy::UDICOMStudy_Def* org_data = th_lib_get_datalist_in_DB(study_uid);
				if (org_data == nullptr) {
					return TH_LIB_EXIT_FAILURE;
				}
				int procSts = org_data->m_procStatus;

				int CoreLibParam = _cntCmdParam2CoreLibParam(m_current_cmd.intParam);
				if ((CoreLibParam & ThPCCoreLibDummy::eProcStatus_Xreal) != 0 ||
					(CoreLibParam & ThPCCoreLibDummy::eProcStatus_iPhone) != 0
					) {
					procSts = procSts & (~CoreLibParam);
					org_data->m_procStatus = procSts;
				}

			}
		}
	}
	break;
	case ThLibCmd_Render_Vol3D:
	{
		 //ref: m_current_volID
	}
	break;
	}
	return TH_LIB_EXIT_SUCCESS;
}
 
//#168_changed_CT-Marker_to_th_lib_exe_render
int CThProcMainDummy::proc_CT_Marker(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, int& next_render_cmds)
{
	std::wstring study_uid = m_current_cmd.strParam1;

	ThPCCoreLibDummy::UDICOMStudy_Def* org_data = th_lib_get_datalist_in_DB(study_uid);
	if (org_data == nullptr) {
		return TH_LIB_EXIT_FAILURE;
	}
	int procSts = org_data->m_procStatus;

	switch (cmd->cmdID) {
	case ThLibCmd_CT_Marker_Clear:
	{
		procSts = procSts & (~ThPCCoreLibDummy::eProcStatus_CTMarker);
		org_data->m_procStatus = procSts;
	}
	break;
	case ThLibCmd_CT_Marker_Add:
		//not here
		// ref:
		//ThLibMouseEvent_CT_Marker_2D:
		//ThLibMouseEvent_CT_Marker_3D:
		break;
	case ThLibCmd_CT_Marker_Final:
	{
		procSts = procSts | ThPCCoreLibDummy::eProcStatus_CTMarker;
		org_data->m_procStatus = procSts;
	}
	break;
	}
	next_render_cmds |= ThLibRRCmd_2D_1;
	next_render_cmds |= ThLibRRCmd_3D;
	return TH_LIB_EXIT_SUCCESS;
}

int CThProcMainDummy::proc_ROI(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)
{
	std::wstring study_uid = m_current_cmd.strParam1;
	ThPCCoreLibDummy::UDICOMStudy_Def* org_data = th_lib_get_datalist_in_DB(study_uid);
	if (org_data == nullptr) {
		return TH_LIB_EXIT_FAILURE;
	}
	int procSts = org_data->m_procStatus;

	switch (cmd->cmdID) {
	case ThLibCmd_ROI_Clear:
	{
		procSts = procSts & (~ThPCCoreLibDummy::eProcStatus_Roi);
		org_data->m_procStatus = procSts;
	}
	break;
	case ThLibCmd_ROI_Add:
	case ThLibCmd_ROI_Erase:
	case ThLibCmd_ROI_Final:
	{
		procSts = procSts | ThPCCoreLibDummy::eProcStatus_Roi;
		org_data->m_procStatus = procSts;
	}
	break;
	}
	return TH_LIB_EXIT_SUCCESS;
}

int CThProcMainDummy::proc_Output(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)
{
	std::wstring study_uid = m_current_cmd.strParam1;
	ThPCCoreLibDummy::UDICOMStudy_Def* org_data = th_lib_get_datalist_in_DB(study_uid);
	if (org_data == nullptr) {
		return TH_LIB_EXIT_FAILURE;
	}
	int procSts = org_data->m_procStatus;

	switch (cmd->cmdID) {
	case ThLibCmd_Output_Remove:			//40
	{
#if 0
		int CoreLibParam = _cntCmdParam2CoreLibParam(cmd->intParam);
		if ((CoreLibParam & ThPCCoreLibDummy::eProcStatus_Xreal) != 0 ||
			(CoreLibParam & ThPCCoreLibDummy::eProcStatus_iPhone) != 0
			) {
			procSts = procSts & (~CoreLibParam);
			org_data->m_procStatus = procSts;
		}
#else
		//#32_ThLibCmd_Output_Remove
		exe_cmd_delete_output(cmd, cmd_ret);
#endif
	}
	break;
	case ThLibCmd_Output_Add:
	{
#if 0
		int CoreLibParam = _cntCmdParam2CoreLibParam(cmd->intParam);
		if ((CoreLibParam & ThPCCoreLibDummy::eProcStatus_Xreal) != 0 ||
			(CoreLibParam & ThPCCoreLibDummy::eProcStatus_iPhone) != 0
			) {
			procSts = procSts | CoreLibParam;
			org_data->m_procStatus = procSts;
		}
#else
		//#32_ThLibCmd_Output_Remove
		exe_cmd_add_output(cmd, cmd_ret);
#endif
	}
	break;
	}

	return TH_LIB_EXIT_SUCCESS;
}
//#32_ThLibCmd_Output_Remove
int CThProcMainDummy::exe_cmd_add_output(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)
{
	CThProcMain::exe_cmd_async(cmd, cmd_ret);

	m_thread_cmd_async.m_proc_hdr.reset(new CMyUpdateOutputProc(this, ThLibCmd_Output_Add, cmd_ret->cmdUID));
	m_thread_cmd_async.m_theExe.reset(new CMultiThExe(1, m_thread_cmd_async.m_proc_hdr.get()));

	return TH_LIB_EXIT_SUCCESS;
}
int CThProcMainDummy::exe_cmd_delete_output(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)
{
	CThProcMain::exe_cmd_async(cmd, cmd_ret);

	m_thread_cmd_async.m_proc_hdr.reset(new CMyUpdateOutputProc(this, ThLibCmd_Output_Remove, cmd_ret->cmdUID));
	m_thread_cmd_async.m_theExe.reset(new CMultiThExe(1, m_thread_cmd_async.m_proc_hdr.get()));

	return TH_LIB_EXIT_SUCCESS;
}


int CThProcMainDummy::th_lib_exe_render(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, ThProcLib_Image2DData* outImage)
{
	int sts = CThProcMain::th_lib_exe_render(cmd, param, outImage);
	if (sts != TH_LIB_EXIT_SUCCESS) return sts;

	if (cmd->cmdID == ThLibCmd_Render_Vol3D) {
		sts = th_lib_render_vol_dummy(cmd, param, outImage);
	}

	return sts;
}
int CThProcMainDummy::exe_cmd_load_vol(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)
{
	CThProcMain::exe_cmd_async(cmd, cmd_ret);
	m_thread_cmd_async.m_proc_hdr.reset(new CMyLoadVolProc(this, cmd_ret->cmdUID));
	m_thread_cmd_async.m_theExe.reset(new CMultiThExe(1, m_thread_cmd_async.m_proc_hdr.get()));
	
	return TH_LIB_EXIT_SUCCESS;
}
//#103_load3DModel_command
int CThProcMainDummy::exe_cmd_load_3DModle(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)
{
	return TH_LIB_EXIT_SUCCESS;
}
//#137_check_image_server
int CThProcMainDummy::exe_cmd_check_image_server(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)
{
	cmd_ret->intParam = 1;//success;
	return TH_LIB_EXIT_SUCCESS;
}
int CThProcMainDummy::th_lib_get_vol_info(const ThProcLib_Cmd* cmd, ThProcLib_Vol3DData* outVol)
{
	int volID = cmd->intParam;
	ThProcLib_Vol3DData *pVol = get_vol3D_ID(volID);
	if (pVol == nullptr) {
		return TH_LIB_EXIT_FAILURE;
	}
	*outVol = *pVol;
	return TH_LIB_EXIT_SUCCESS;
}
int CThProcMainDummy::proc_load_vol_in(void)
{
	return CThProcMain::proc_load_vol_in();
}
int CThProcMainDummy::exe_get_path(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)
{
	std::wstring study_uid = m_current_cmd.strParam1;
	ThPCCoreLibDummy::UDICOMStudy_Def* pdata = th_lib_get_datalist_in_DB(study_uid);

	std::wstring study_folder = makeStudyFolder(pdata->m_studyDate, pdata->m_studyTime, ThStringUtils::_convertStr(study_uid));
	 
	 
	switch (cmd->cmdID) {
	case ThLibCmd_Get_StudyPath:
		 
		break;
	case ThLibCmd_Get_MeshPath:
		study_folder = std::wstring(L"C:/ZeusHome/CacheData/MeshData/")+ study_folder;
		break;
	default:
		return  TH_LIB_EXIT_FAILURE;
		break;
	}
	_copy_wstr(cmd_ret->strParam1, study_folder.c_str());
	 
	return TH_LIB_EXIT_SUCCESS;
}
