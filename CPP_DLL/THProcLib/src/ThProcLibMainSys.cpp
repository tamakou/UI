 
#include "ThProcLibMainSys.h"
#include "ThProcRenderImp.h"
#include "ThProcLibErrorCode.h"
#include "ThStringUtils.h"
#include <thread>
#include <chrono>
#include <map>
#include <memory>
#ifdef UseDummyData
#include "ThProcLibRenderDummy.h"
#endif
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

/// <summary>
/// 
/// </summary>
 
class CMyUpdateImportProc  : public CThreadProc< CThProcMainSys>
{
public:
	CMyUpdateImportProc(CThProcMainSys *pMain,int cmd_uid,int timeout):
		CThreadProc< CThProcMainSys>(pMain, ThLibCmd_Exe_ImportData, cmd_uid)
		, m_timeout_sec(timeout)
	{

	}
	int doProc(int th_id) override
	{
		int base_time = 500;//mSec
		int toop_n = (int)(m_timeout_sec * 1000.0 / base_time);
		 
		for (int i = 0; i < toop_n; i++) {
			if (m_bCancelFlag) break;//#205_Cancel_import_data_added_cancel_flag
			if (m_pMain->check_cmd_async(ThLibCmd_Exe_ImportData,m_cmd_uid,i) == ThLibCmdStatus_Progress) {
				std::this_thread::sleep_for(std::chrono::milliseconds(base_time));
			}else {
				break;
			}
		}
		return 0;
	};
protected:
	int m_timeout_sec;
};
class CMyUpdateAiSegProc : public CThreadProc< CThProcMainSys>
{
public:
	CMyUpdateAiSegProc(CThProcMainSys* pMain,int cmdID, int cmd_uid, int timeout) :
		CThreadProc< CThProcMainSys>(pMain, cmdID, cmd_uid)
		, m_timeout_sec(timeout)
	{

	}
	int doProc(int th_id) override
	{
		int base_time = 500;//mSec
		int toop_n = (int)(m_timeout_sec * 1000.0 / base_time);

		for (int i = 0; i < toop_n; i++) {
			if (m_bCancelFlag) break;//#205_Cancel_import_data_added_cancel_flag
			if (m_pMain->check_cmd_async(m_cmdID, m_cmd_uid,i) == ThLibCmdStatus_Progress) {
				std::this_thread::sleep_for(std::chrono::milliseconds(base_time));
			}
			else {
				break;
			}
		}
		return 0;
	};
protected:
 
	int m_timeout_sec;
};
class CMyLoadVolProc : public CThreadProc< CThProcMainSys>
{
public:
	friend CThProcMainSys;
	CMyLoadVolProc(CThProcMainSys* pMain, int cmd_uid) :
		CThreadProc< CThProcMainSys>(pMain, ThLibCmd_Render_Load_Vol3D, cmd_uid)
	{

	}
	int doProc(int th_id) override
	{
		//#154_Image_server_not_working
		int sts = m_pMain->proc_load_vol_in();
	 
		for (int i = 0; i < 200; i++) {
			if (m_bCancelFlag) break;//#205_Cancel_import_data_added_cancel_flag
			::_sleep(20);
			//			_cmd_uid_map[m_cmd_uid]++;
			if (m_pMain->check_cmd_async(m_cmdID, m_cmd_uid) == TH_LIB_EXIT_SUCCESS) break;
		}
		 
		return 0;
	};
protected:

};
//#32_ThLibCmd_Output_as_async
class CMyUpdateOutputProc : public CThreadProc< CThProcMainSys>
{
public:
	friend CThProcMainSys;
	CMyUpdateOutputProc(CThProcMainSys* pMain, int cmdID, int cmd_uid) :
		CThreadProc< CThProcMainSys>(pMain, cmdID, cmd_uid)
	{

	}
	int doProc(int th_id) override
	{
		//#208_delete_output_file
		int sts = m_pMain->delete_output_in();
		for (int i = 0; i < 200; i++) {
			if (m_bCancelFlag) break;//#205_Cancel_import_data_added_cancel_flag
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
		out_param = ThPCCoreLib::eProcStatus_Xreal;
		break;
	case ThLibCmdOutput_iPhone:
		out_param = ThPCCoreLib::eProcStatus_iPhone;
		break;
	case ThLibCmdOutput_Xreal_iPhone:
		out_param = ThPCCoreLib::eProcStatus_Xreal | ThPCCoreLib::eProcStatus_iPhone;
		break;
	}
	return out_param;
}
CThProcMainSys::CThProcMainSys()
{
	//#127_shared_Haiya_Juukaku
	m_renderGrpParam = new CThProcRenderGrpParam;
	m_renderGrpParam->m_pRoi_Pen_Status_shared = &m_Roi_Pen_Status;
}
CThProcMainSys::~CThProcMainSys()
{
	clearRenders();
	delete m_renderGrpParam;//#127_shared_Haiya_Juukaku
}
 
void CThProcMainSys::clearRenders(void)
{
	for (auto it : m_renders) {
		if (it.second != nullptr) delete it.second;
	}
	m_renders.clear();
}
CThProcRender* CThProcMainSys::findRender(eSeriesType type) const
{
	CThProcRender* ret_p = nullptr;
	for (auto it : m_renders) {
		if (it.first == type) {
			ret_p = it.second;
			break;
		}
	}
	return ret_p;
}
int CThProcMainSys::th_lib_init(int mode)
{
	CThProcMain::th_lib_init(mode);

	Trace_Entry;

	m_PcCoreLib.reset(ThPCCoreLib::CThPCCoreLib::createThPCCoreLibIf());
	m_PcCoreLib->setProcEvent(&m_PcCoreLibEvent);
	m_PcCoreLib->setupHomeFolder(m_lib_env.HomeFolder);
	LogMessage(L" CThProcMainSys::th_lib_init ");
	if (!m_PcCoreLib->init()) {
		return TH_LIB_EXIT_FAILURE;
	};
	ThPCCoreLib::ThPCCoreLibIf::ThPCCoreLibConfig config;
	m_PcCoreLib->getConfig(config);
	LogMessage(L" config: \n ");
	LogMessage(L"       3DModelAlpha %d  \n ",config.m_3DModelAlpha);
 
	std::string  cscnt_log_folder = ThStringUtils::_convertStr(config.m_home_folder)+"Log";
	CThProcRenderImp::setupSMAPILibLog(cscnt_log_folder, 0);
	 
	clearRenders();
	//#127_shared_Haiya_Juukaku
	CThProcRenderImp *pRender_Haiya = new CThProcRenderImp(m_PcCoreLib.get(), ThLibSeriesType_Haiya, m_renderGrpParam);
	pRender_Haiya->setRotation3DXYDeg(config.m_DegRotation3D);
	m_renders[ThLibSeriesType_Haiya] = pRender_Haiya;
	m_masterSeriesType = ThLibSeriesType_Haiya;
	if (!m_renders[ThLibSeriesType_Haiya]->init()) {
		LogMessage(L"m_renders[ThLibSeriesType_Haiya]->init error \n");
	//#154_Image_server_not_working
	//	return TH_LIB_EXIT_FAILURE;
	}
	if (this->m_log_level > 1) {
		m_renders[ThLibSeriesType_Haiya]->setupLogger(this);
	}
#if 1
//#127_shared_Haiya_Juukaku
	CThProcRenderImp* pRender_Juukaku = new CThProcRenderImp(m_PcCoreLib.get(), ThLibSeriesType_Juukaku, m_renderGrpParam);
	pRender_Juukaku->setRotation3DXYDeg(config.m_DegRotation3D);
	m_renders[ThLibSeriesType_Juukaku] = pRender_Juukaku;
	m_renders[ThLibSeriesType_Juukaku]->setupShareClientNvrAsMaster(m_renders[ThLibSeriesType_Haiya]);
	if (!m_renders[ThLibSeriesType_Juukaku]->init()) {
		LogMessage(L"m_renders[ThLibSeriesType_Juukaku]->init error \n");
	//#154_Image_server_not_working
	//	return TH_LIB_EXIT_FAILURE;
	}
	if (this->m_log_level > 1) {
		m_renders[ThLibSeriesType_Juukaku]->setupLogger(this);
	}
#endif
//#127_shared_Haiya_Juukaku
	m_renders[ThLibSeriesType_Juukaku]->setupPairing((CThProcRenderImp*)(m_renders[ThLibSeriesType_Haiya]));
	m_renders[ThLibSeriesType_Haiya]->setupPairing((CThProcRenderImp*)(m_renders[ThLibSeriesType_Juukaku]));
	
	//#161_display_thoracentes_needle
	for (auto it : m_renders) {
		it.second->setupNeedleDisplay(config.m_NeedleOn, config.m_NeedleLength, config.m_NeedleR);
	}

	LogMessage(L" OK\n");
	return TH_LIB_EXIT_SUCCESS;
}
int CThProcMainSys::th_lib_setup_env(const ThProcLib_Env* env)
{
	int sts = CThProcMain::th_lib_setup_env(env);
	return sts;
}
int CThProcMainSys::th_lib_get_env(ThProcLib_Env* env)
{
	int sts  = CThProcMain::th_lib_get_env(env);
	ThPCCoreLib::ThPCCoreLibIf::ThPCCoreLibConfig config;
	m_PcCoreLib->getConfig(config);
	_copy_wstr(env->ImportFolder, config.m_CD_Drive); //#181_added_import_path
	//#191_Model_Version
	_copy_wstr(env->ModelVers,
		L"ThDcm2Mesh: Ver1.0.8.2 \n"
		L"ThProcLib: Ver1.0.4.3 \n"
		L"ThPCCoreLib: Ver1.0.6.3 \n"
		L"ThDcmMgrLib: Ver1.0.5.3 \n"
		L"ImageServer: Ver4.0.1.0 "
	)
	return sts;
}

int CThProcMainSys::th_lib_exe_cmd(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)
{
	Trace_Entry_L2;
	int iRetSts = CThProcMain::th_lib_exe_cmd(cmd, cmd_ret);
	return iRetSts;
}
 
int CThProcMainSys::th_lib_get_image_info(const ThProcLib_Cmd* cmd, ThProcLib_DicomImageInfo* image_info)
{
	Trace_Entry_L1;
	std::string studyUID = ThStringUtils::_convertStr(cmd->strParam1);
	eSeriesType series_type = (eSeriesType)cmd->intParam;
	CThProcRender*  render_p = findRender(series_type);
	if (render_p == nullptr) {
		LogMessage(L"invalid series_type :%d \n", series_type);
		TH_LIB_EXIT_FAILURE;
	}
	ThProcLib_DicomImageInfo dicom_infor;
	render_p->get_dicom_info(studyUID,&dicom_infor);
	*image_info = dicom_infor;
	_copy_wstr(image_info->StudyUID, cmd->strParam1);
	image_info->CheckDig = 2345; //check bit, always  2345;
#if 0
	_copy_wstr(image_info->StudyUID, cmd->strParam1);
	image_info->sizeX = 512;
	image_info->sizeY = 512;
	image_info->sizeZ = 512;
	image_info->WindowWidth = 1000;
	image_info->WindowCenter = 200;
	image_info->CheckDig = 2345; //check bit, always  2345;
#endif
	return TH_LIB_EXIT_SUCCESS;
}
int CThProcMainSys::th_lib_get_data(const ThProcLib_Cmd* cmd, unsigned char* intDataBuffer, int BufferSize)
{
	Trace_Entry_L2;
	CThProcMain::th_lib_get_data(cmd, intDataBuffer, BufferSize);
	return TH_LIB_EXIT_SUCCESS;
}
 
int CThProcMainSys::th_lib_get_status(int cmdUID, ThProcLib_CmdStatus* status)
{
	Trace_Entry_L2;
	int iSts = CThProcMain::th_lib_get_status(cmdUID, status);
	return iSts;
}
int CThProcMainSys::th_lib_close_cmd(int cmdUID)
{
	Trace_Entry_L2;
	return CThProcMain::th_lib_close_cmd( cmdUID);
}

int CThProcMainSys::th_lib_get_datalist_size(int type)
{
	Trace_Entry_L1;
	LogMessage(L"get_datalist_size type:%d \n", type);
	return CThProcMainSys::get_datalist_size_in(type);
}

int CThProcMainSys::th_lib_get_datalist(ThProcLib_PatientInfo* out_info, int type, int no)
{
	Trace_Entry_L2;
	//LogMessage(L"th_lib_get_datalist type:%d , no:%d\n", type,no);
	return CThProcMainSys::get_datalist_in(out_info, type,  no);
	 
}
int CThProcMainSys::check_cmd_async(int cmdID, int cmdUID, int chkNo)
{
	Trace_Entry_L2;
	if (m_PcCoreLib == nullptr) return  TH_LIB_EXIT_FAILURE;
	if (cmdUID <= ThLibCmd_Unknown) {
		return TH_LIB_EXIT_FAILURE;
	}
	bool bLogFlag = (chkNo % 20) == 1;
	//#205_Cancel_import_data_added_cancel_flag
	//cmd is closed
	int ret_val = m_cmd_uid_map.size() < 1 ? TH_LIB_EXIT_FAILURE: TH_LIB_EXIT_IN_PROGRESS;//in progress
	bool bProcFinished = false;
	for (auto it = m_cmd_uid_map.begin(); it != m_cmd_uid_map.end(); it++) {
		if (it->first != cmdUID) {
			continue;
		}
		if ((cmdID == ThLibCmd_Output_Remove)  
				) { //Žb’è 
			int prog_step = 5;
			//check finish at first
			if ((it->second->cmdProgress + prog_step) >= 100) {
				finish_cmd_async(cmdID, cmdUID);
			}
			it->second->cmdProgress = it->second->cmdProgress+ prog_step;
			if (it->second->cmdProgress >= 100) {
				LogMessage(L"cmdProgress >=100 \n");
				it->second->cmdStatus =  ThLibCmdStatus_Succeeded  ;
				ret_val = TH_LIB_EXIT_SUCCESS;//for finish 	
			}
			break;
		}
		int completed;
		float progress;
		ThPCCoreLib::eCoreLibThreadType type = ThPCCoreLib::eCoreLibThread_Unknown;
		switch (cmdID) {
		case ThLibCmd_Exe_ImportData:
			type = ThPCCoreLib::eCoreLibThread_CDImport;
			break;
		case ThLibCmd_Exe_AISegmentator:
			type = ThPCCoreLib::eCoreLibThread_AiSegWatcher;
			break;
		case ThLibCmd_Output_Add:
			type = ThPCCoreLib::eCoreLibThread_AiSegWatcher;
			break;
		case ThLibCmd_Render_Vol3D:
		case ThLibCmd_Render_Load_Vol3D:
			{//not yet, ref: CThProcMainDummy::check_cmd_async
				bProcFinished = true;
					 
				int completed1, completed2;
				float progress1, progress2;
				bool bExeSts1 = m_renders[ThLibSeriesType_Haiya]->getProgress(completed1, progress1);
				bool bExeSts2 = m_renders[ThLibSeriesType_Juukaku]->getProgress(completed2, progress2);
				bool bExeSts = bExeSts1 && bExeSts2;
				if ((completed1 == 1) && (completed2 == 1)) {
					LogMessage(L"load volume getProgress completed \n");
					if (bExeSts) {
						finish_cmd_async(cmdID, cmdUID);
						it->second->dataID = m_current_volID;//return volID to ThProcLib_CmdStatus 2025/07/18
					}
					else {
						release_vol3D_ID(m_current_volID);
					}
					it->second->cmdProgress = 100;
					it->second->cmdStatus = bExeSts ? ThLibCmdStatus_Succeeded : ThLibCmdStatus_Failed;
					m_procStatus = eProcStatus_VolLoaded;
					ret_val = TH_LIB_EXIT_SUCCESS;//for finish 	 
				}
				else {
					it->second->cmdProgress = (int)(progress * 100.0f);
				}
					 
			}
			break;
		}
		if (bLogFlag) {
			LogMessage(L" check_cmd_async [%d] - %d \n", cmdID,chkNo);
		}
		if (bProcFinished) {
			//skip
		}
		else {
			bool bExeSts = m_PcCoreLib->getProgress(type, completed, progress);
			if (completed == 1) {
				LogMessage(L"m_PcCoreLib->getProgress completed \n");
				if (bExeSts) {//#28_dll_error_code
					finish_cmd_async(cmdID, cmdUID);
				}
				it->second->cmdProgress = 100;
				//#28_dll_error_code
				it->second->cmdStatus = bExeSts ? ThLibCmdStatus_Succeeded : ThLibCmdStatus_Failed;
				ret_val = TH_LIB_EXIT_SUCCESS;//for finish 	 
				if(it->second->cmdStatus== ThLibCmdStatus_Failed){//setup last error
				//#216_setup_last_error
					int error_code = ThProcLibErrorCode::ThDllErr_Dll_Unknown_Error;
					switch (cmdID) {
					case ThLibCmd_Exe_ImportData:
						error_code = ThProcLibErrorCode::ThDllErr_ImportData;
						break;
					case ThLibCmd_Exe_AISegmentator:
						error_code = ThProcLibErrorCode::ThDllErr_AiSegmentation;
						
						break;
					case ThLibCmd_Output_Add:
						error_code = ThProcLibErrorCode::ThDllErr_OutputMesh;
						break;
					}
					setupLastError(error_code);

				}
			}
			else {
				it->second->cmdProgress = (int)(progress * 100.0f);
			}
		}
		break;
		 
		//check cancel here
#pragma message("*** check cancel here ***")
		// to stop it
		// ret_val = TH_LIB_EXIT_SUCCESS;  
	}
	return ret_val;
}

 
int CThProcMainSys::exe_cmd_import(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)
{
	Trace_Entry_L1;
	if (m_PcCoreLib == nullptr) return  TH_LIB_EXIT_FAILURE;

	CThProcMain::exe_cmd_async(cmd, cmd_ret);
	const wchar_t* dcm_folder = nullptr;//default CT-ROM
	m_PcCoreLib->importCDROMData(dcm_folder, true/*async*/);

	m_thread_cmd_async.m_proc_hdr.reset(new CMyUpdateImportProc(this,cmd_ret->cmdUID,this->m_import_CDROM_timeout_sec));
	m_thread_cmd_async.m_theExe.reset(new CMultiThExe(1, m_thread_cmd_async.m_proc_hdr.get()));

	return TH_LIB_EXIT_SUCCESS;
}
int CThProcMainSys::exe_cmd_async_cancel(const ThProcLib_Cmd* cmd)
{
	Trace_Entry_L1;
	if (m_PcCoreLib == nullptr) return  TH_LIB_EXIT_FAILURE;
	
	int nSts = CThProcMain::exe_cmd_async_cancel(cmd);
	return TH_LIB_EXIT_SUCCESS;
}
//#205_Cancel_import_data
bool CThProcMainSys::cancel_import_CDROM(ThProcLib_CmdStatus* cmdSts)
{
	if (m_PcCoreLib == nullptr) return false;

	//for th_lib_get_status(-1,ThProcLib_CmdStatus* status)
	setupLastError(ThProcLibErrorCode::ThDllErr_ImportDataCanceled);

	std::wstring errStr = ThProcLibErrorCode::getErrCode(ThProcLibErrorCode::ThDllErr_ImportDataCanceled);
	errStr = std::wstring(L"[")+ errStr+std::wstring(L"] ")+ ThProcLibErrorCode::getErrStr(ThProcLibErrorCode::ThDllErr_ImportDataCanceled);

	_copy_wstr(cmdSts->errMessage, errStr.c_str());
	bool bSts = m_PcCoreLib->cancelCmd(ThPCCoreLib::eCoreLibThread_CDImport);
	return bSts;
};

//#210_delete_imported_data
int CThProcMainSys::exe_cmd_delete_import_data(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)
{
	Trace_Entry_L1;
	if (m_PcCoreLib == nullptr) return  TH_LIB_EXIT_FAILURE;
	std::string study_uid = ThStringUtils::_convertStr(m_current_cmd.strParam1);
	ThDcmMgrLib::UDICOMStudy_Def org_data;
	if (!m_PcCoreLib->getStudyData(study_uid.c_str(), org_data)) {
		setupLastError(ThProcLibErrorCode::ThDllErr_InvalidImportData);
		return TH_LIB_EXIT_FAILURE;
	};
	//#210_delete_imported_data
	std::vector<std::wstring> delete_paths;
	ThDcmMgrLib::UDICOMFolder_Def data_folder;
	//series_haiya
	bool bIsHaiya = true;
	data_folder.m_dicom_folder[0] = 0;
	if (m_PcCoreLib->getDicomFolder(study_uid.c_str(), data_folder, bIsHaiya)) {
		delete_paths.push_back(data_folder.m_dicom_folder);
	}
	//series_juukaku
	bIsHaiya = false;
	data_folder.m_dicom_folder[0] = 0;
	if (m_PcCoreLib->getDicomFolder(study_uid.c_str(), data_folder, bIsHaiya)) {
		delete_paths.push_back(data_folder.m_dicom_folder);
	}
	//StlFolder
	data_folder.m_dicom_folder[0] = 0;
	if (m_PcCoreLib->getStlFolder(study_uid.c_str(), data_folder))
	{
		delete_paths.push_back(data_folder.m_dicom_folder);
	};
	//CTMarkerFolder
	data_folder.m_dicom_folder[0] = 0;
	if (m_PcCoreLib->getCTMarkerFolder(study_uid.c_str(), data_folder))
	{
		delete_paths.push_back(data_folder.m_dicom_folder);
	};
	//remove folders
	for (auto it : delete_paths)
	{
		std::wstring folder_name = it;
		if (folder_name.empty()) {
			continue;
		}
		fs::path full_path = folder_name;
		if (fs::exists(full_path)) {
			try {
				std::uintmax_t count = fs::remove_all(full_path);
			}
			catch (const fs::filesystem_error& e) {
				;
			}
		}
	}
	//remove dicom study folder
	{
		for (auto it : delete_paths) {
			if (it.find(L"DicomData") != std::wstring::npos){
				fs::path full_path = it;
				//try to remove all folder, if no other device sub-folder
				fs::path parent = full_path.parent_path();
				if (fs::exists(parent)) {
					if (fs::is_empty(parent)) {
						std::wstring parent_str = parent.wstring();

						if (parent_str.find(ThStringUtils::_convertStr(study_uid)) == std::wstring::npos) {
							continue;
						}
						try {
							std::uintmax_t count = fs::remove_all(parent);
						}
						catch (const fs::filesystem_error& e) {
							continue;
						}
						break;

					}
				}

			}
		}
	}
	//delete DB
	{
		if (!m_PcCoreLib->deleteStudy(study_uid.c_str())) {
			setupLastError(ThProcLibErrorCode::ThDllErr_DeleteStudy);
			return TH_LIB_EXIT_FAILURE;
		}
	}
	return TH_LIB_EXIT_SUCCESS;
}
//#208_delete_output_file
int CThProcMainSys::delete_output_in(void)
{
	Trace_Entry_L1;
	if (m_PcCoreLib == nullptr) return  TH_LIB_EXIT_FAILURE;
	std::string study_uid = ThStringUtils::_convertStr(m_current_cmd.strParam1);
	ThDcmMgrLib::UDICOMStudy_Def org_data;
	if (!m_PcCoreLib->getStudyData(study_uid.c_str(), org_data)) {
		setupLastError(ThProcLibErrorCode::ThDllErr_InvalidImportData);
		return TH_LIB_EXIT_FAILURE;
	};
	ThDcmMgrLib::UDICOMFolder_Def data_folder;
 
//	for (auto ti : std::vector< ThPCCoreLib::eProcStatus>(ThPCCoreLib::eProcStatus_iPhone, ThPCCoreLib::eProcStatus_Xreal))
	{

		std::wstring folder_name = get_mesh_folder_from_cmd(&m_current_cmd);
		 
		if (folder_name.empty()){
			return TH_LIB_EXIT_FAILURE;
		}
		
		fs::path full_path = folder_name;
		if (fs::exists(full_path)) {

			try {
				std::uintmax_t count = fs::remove_all(full_path);
			}
			catch (const fs::filesystem_error& e) {
				return TH_LIB_EXIT_FAILURE;
			}
		}
		//try to remove all folder, if no other device sub-folder
		fs::path parent = full_path.parent_path();
		if (fs::exists(parent)) {
			if (fs::is_empty(parent)) {
				std::wstring parent_str = parent.wstring();
				{
					if (parent_str.find(ThStringUtils::_convertStr(study_uid)) == std::wstring::npos) {
						return TH_LIB_EXIT_FAILURE;;
					}
					try {
						std::uintmax_t count = fs::remove_all(parent);
					}
					catch (const fs::filesystem_error& e) {
						return TH_LIB_EXIT_FAILURE;
					}
				}
			}
		}
	}
	
	return TH_LIB_EXIT_SUCCESS;
}
int CThProcMainSys::get_new_data_ID(void)
{
	Trace_Entry_L2;
	int nRetID = -1;
	if (m_PcCoreLib == nullptr) return  nRetID;
	
	std::string current_study_uid = m_PcCoreLib->getCurrentStudyUID();
	ThDcmMgrLib::DcmStudyList *study_list = m_PcCoreLib->getStudyList();
	if (study_list != nullptr) {
		for (int i = 0; i < study_list->getSize(); i++) {
			ThDcmMgrLib::UDICOMStudy_Def outStudy;
			if (study_list->getStudy(i, outStudy)) {
				if (current_study_uid == outStudy.m_studyInstanceUID) {
					nRetID = i;
					_copy_wstr(m_new_data.StudyUID, ThStringUtils::_convertStr(outStudy.m_studyInstanceUID).c_str());
					break;
				}
			}
		}
		study_list->Destroy();
	}
	return nRetID;
}
int CThProcMainSys::exe_cmd_AiSegmentator(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)
{
	Trace_Entry_L1;
	if (m_PcCoreLib == nullptr) return  TH_LIB_EXIT_FAILURE;
	std::string study_uid = ThStringUtils::_convertStr(cmd->strParam1);

	CThProcMain::exe_cmd_async(cmd, cmd_ret);
	m_PcCoreLib->runAISegmentator(study_uid.c_str(), true/*async*/);

	m_thread_cmd_async.m_proc_hdr.reset(new CMyUpdateAiSegProc(this, cmd_ret->cmdID,cmd_ret->cmdUID, m_AiSeg_timeout_sec));
	m_thread_cmd_async.m_theExe.reset(new CMultiThExe(1, m_thread_cmd_async.m_proc_hdr.get()));

	return TH_LIB_EXIT_SUCCESS;
}
 
int CThProcMainSys::finish_cmd_async(int cmdID, int cmdUID)
{
	Trace_Entry_L2;
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
		LogMessage(L"finish ThLibCmd_Exe_AISegmentator  \n");
		std::string study_uid = ThStringUtils::_convertStr(m_current_cmd.strParam1);
		int procSts =	ThPCCoreLib::eProcStatus_AiSeg
		//				| ThPCCoreLib::eProcStatus_Roi
		//				| ThPCCoreLib::eProcStatus_Xreal
			;
		m_PcCoreLib->changeProcStatus(study_uid.c_str(), procSts);
 
	}
	break;
	case ThLibCmd_Output_Add: //#32_ThLibCmd_Output_as_async
	{
		LogMessage(L"finish ThLibCmd_Output_Add  \n");
			//#50_output_model.json
			//Žb’è here
		output_model_file(&m_current_cmd);
		std::string study_uid = ThStringUtils::_convertStr(m_current_cmd.strParam1);
		ThDcmMgrLib::UDICOMStudy_Def org_data;
		if (!m_PcCoreLib->getStudyData(study_uid.c_str(), org_data)) {
			return TH_LIB_EXIT_FAILURE;
		};
		int procSts = org_data.m_procStatus;

		int CoreLibParam = _cntCmdParam2CoreLibParam(m_current_cmd.intParam);
		if ((CoreLibParam & ThPCCoreLib::eProcStatus_Xreal) != 0 ||
			(CoreLibParam & ThPCCoreLib::eProcStatus_iPhone) != 0
			) {
			procSts = procSts | CoreLibParam;
			if (!m_PcCoreLib->changeProcStatus(study_uid.c_str(), procSts)) {
				return TH_LIB_EXIT_FAILURE;
			};
		}
		else {
			return TH_LIB_EXIT_FAILURE;
		}
	}
	break;
	case ThLibCmd_Output_Remove: //#32_ThLibCmd_Output_as_async
	{
		std::string study_uid = ThStringUtils::_convertStr(m_current_cmd.strParam1);
		ThDcmMgrLib::UDICOMStudy_Def org_data;
		if (!m_PcCoreLib->getStudyData(study_uid.c_str(), org_data)) {
			return TH_LIB_EXIT_FAILURE;
		};
		int procSts = org_data.m_procStatus;
		int CoreLibParam = _cntCmdParam2CoreLibParam(m_current_cmd.intParam);
		if ((CoreLibParam & ThPCCoreLib::eProcStatus_Xreal) != 0 ||
			(CoreLibParam & ThPCCoreLib::eProcStatus_iPhone) != 0
			) {
			procSts = procSts & (~CoreLibParam);
			if (!m_PcCoreLib->changeProcStatus(study_uid.c_str(), procSts)) {
				return TH_LIB_EXIT_FAILURE;
			};
		}
		else {
			return TH_LIB_EXIT_FAILURE;
		}
	}
	break;
	case ThLibCmd_Render_Load_Vol3D:
	{
		return create_vol_info();
	}
	break;
	}
	return TH_LIB_EXIT_SUCCESS;
}
//#168_changed_CT-Marker_to_th_lib_exe_render
int CThProcMainSys::proc_CT_Marker(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, int& next_render_cmds)
{
	Trace_Entry_L2;

	ThDcmMgrLib::UDICOMStudy_Def org_data;
#if 0
	std::string study_uid = ThStringUtils::_convertStr(m_current_cmd.strParam1);
#else
	//#125_insert_CT_Marker
	//	int volID = cmd->intParam;
		//#140_use_m_current_volID
	int volID = m_current_volID;
	ThProcLib_Vol3DData* pVol = get_vol3D_ID(volID);
	if (pVol == nullptr) {
		return TH_LIB_EXIT_FAILURE;
	}
	std::string study_uid = ThStringUtils::_convertStr(pVol->StudyUID);
#endif
	if (!m_PcCoreLib->getStudyData(study_uid.c_str(), org_data)) {
		return TH_LIB_EXIT_FAILURE;
	};
	int procSts = org_data.m_procStatus;

	//#127_shared_Haiya_Juukaku

	CThProcRender* pRender = m_renders[this->m_masterSeriesType];

	switch (cmd->cmdID) {
	case ThLibCmd_CT_Marker_Clear:
	{
		procSts = procSts & (~ThPCCoreLib::eProcStatus_CTMarker);
		if (!m_PcCoreLib->changeProcStatus(study_uid.c_str(), procSts)) {
			return TH_LIB_EXIT_FAILURE;
		};
		pRender->clearCTMarker();
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
		procSts = procSts | ThPCCoreLib::eProcStatus_CTMarker;
		if (!m_PcCoreLib->changeProcStatus(study_uid.c_str(), procSts)) {
			return TH_LIB_EXIT_FAILURE;
		};

		if (!pRender->saveCTMarker()) {
			return TH_LIB_EXIT_FAILURE;
		};

	}
	break;
	}
	next_render_cmds |= ThLibRRCmd_2D_1;
	next_render_cmds |= ThLibRRCmd_3D;
	return TH_LIB_EXIT_SUCCESS;
}

int CThProcMainSys::proc_ROI(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)
{
	Trace_Entry_L2;
	std::string study_uid = ThStringUtils::_convertStr(m_current_cmd.strParam1);
	ThDcmMgrLib::UDICOMStudy_Def org_data;
	if (!m_PcCoreLib->getStudyData(study_uid.c_str(), org_data)) {
		return TH_LIB_EXIT_FAILURE;
	};
	int procSts = org_data.m_procStatus;

	switch (cmd->cmdID) {
	case ThLibCmd_ROI_Clear:
		{
			procSts = procSts & (~ThPCCoreLib::eProcStatus_Roi);
			if (!m_PcCoreLib->changeProcStatus(study_uid.c_str(), procSts)) {
				return TH_LIB_EXIT_FAILURE;
			};
		}
	break;
	case ThLibCmd_ROI_Add:
	case ThLibCmd_ROI_Erase:
	case ThLibCmd_ROI_Final:
		{
			procSts = procSts | ThPCCoreLib::eProcStatus_Roi;
			if (!m_PcCoreLib->changeProcStatus(study_uid.c_str(), procSts)) {
				return TH_LIB_EXIT_FAILURE;
			};
		}
		break;
	}
	return TH_LIB_EXIT_SUCCESS;
}

int CThProcMainSys::proc_Output(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)
{
	Trace_Entry_L2;

	std::string study_uid = ThStringUtils::_convertStr(m_current_cmd.strParam1);
	ThDcmMgrLib::UDICOMStudy_Def org_data;
	if (!m_PcCoreLib->getStudyData(study_uid.c_str(), org_data)) {
		return TH_LIB_EXIT_FAILURE;
	};
	int procSts = org_data.m_procStatus;

	int ret_sts = TH_LIB_EXIT_FAILURE;
	switch (cmd->cmdID) {
	case ThLibCmd_Output_Remove:			//40
		{
#if 0
			int CoreLibParam = _cntCmdParam2CoreLibParam(cmd->intParam);
			if ((CoreLibParam & ThPCCoreLib::eProcStatus_Xreal) != 0 ||
				(CoreLibParam & ThPCCoreLib::eProcStatus_iPhone) != 0
				) {
				procSts = procSts & (~CoreLibParam);
				if (!m_PcCoreLib->changeProcStatus(study_uid.c_str(), procSts)) {
					return TH_LIB_EXIT_FAILURE;
				};
			}
			else {
				return TH_LIB_EXIT_FAILURE;
			}
#else
		//#32_ThLibCmd_Output_Remove
		ret_sts = exe_cmd_delete_output(cmd, cmd_ret);
#endif
		}
		break;
	case ThLibCmd_Output_Add:
		{
#if 0
		int CoreLibParam = _cntCmdParam2CoreLibParam(cmd->intParam);
		if ((CoreLibParam & ThPCCoreLib::eProcStatus_Xreal) != 0 ||
			(CoreLibParam & ThPCCoreLib::eProcStatus_iPhone) != 0
			) {
			procSts = procSts | CoreLibParam;
			if (!m_PcCoreLib->changeProcStatus(study_uid.c_str(), procSts)) {
				return TH_LIB_EXIT_FAILURE;
			};
		}
		else {
			return TH_LIB_EXIT_FAILURE;
		}
#else
		//#32_ThLibCmd_Output_Remove
		ret_sts = exe_cmd_add_output(cmd, cmd_ret);
#endif
		}
		break;
	}

	return ret_sts;
}
//#32_ThLibCmd_Output_as_async
int CThProcMainSys::exe_cmd_add_output(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)
{
	Trace_Entry_L2;
#if 0
	CThProcMain::exe_cmd_async(cmd, cmd_ret);

	m_thread_cmd_async.m_proc_hdr.reset(new CMyUpdateOutputProc(this, ThLibCmd_Output_Add, cmd_ret->cmdUID));
	m_thread_cmd_async.m_theExe.reset(new CMultiThExe(1, m_thread_cmd_async.m_proc_hdr.get()));
#else
	if (m_PcCoreLib == nullptr) return  TH_LIB_EXIT_FAILURE;
	std::string study_uid = ThStringUtils::_convertStr(cmd->strParam1);

	CThProcMain::exe_cmd_async(cmd, cmd_ret);
	bool glbOnly = cmd->intParam == ThLibCmdOutput_Xreal;
	m_PcCoreLib->outputMesh(study_uid.c_str(), glbOnly,true/*async*/);
 
	m_thread_cmd_async.m_proc_hdr.reset(new CMyUpdateAiSegProc(this, ThLibCmd_Output_Add,cmd_ret->cmdUID, m_AiSeg_timeout_sec/3));
	m_thread_cmd_async.m_theExe.reset(new CMultiThExe(1, m_thread_cmd_async.m_proc_hdr.get()));
 
#endif
	return TH_LIB_EXIT_SUCCESS;
}
int CThProcMainSys::exe_cmd_delete_output(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)
{
	Trace_Entry_L2;
	CThProcMain::exe_cmd_async(cmd, cmd_ret);

	m_thread_cmd_async.m_proc_hdr.reset(new CMyUpdateOutputProc(this, ThLibCmd_Output_Remove, cmd_ret->cmdUID));
	m_thread_cmd_async.m_theExe.reset(new CMultiThExe(1, m_thread_cmd_async.m_proc_hdr.get()));

	return TH_LIB_EXIT_SUCCESS;
}

int CThProcMainSys::exe_get_image_data(const ThProcLib_Cmd* cmd, unsigned char* RGBA_DataBuffer, int BufferSize)
{
	Trace_Entry_L2;
#ifdef UseDummyData
	RenderImgDummy* pImage = th_lib_get_image_dummy(cmd->intParam);
	if (pImage == nullptr) return TH_LIB_EXIT_FAILURE;

	unsigned char* pbuffer = (unsigned char*)intDataBuffer;
	for (int i = 0; i < BufferSize; i++) {
		pbuffer[i] = pImage->m_image_buffer[i];
	}
#else
	RenderImg* pImage = th_lib_get_image(cmd->intParam);
	if (pImage == nullptr) return TH_LIB_EXIT_FAILURE;
	
	bool bSts = pImage->m_render->copyOutputImage(RGBA_DataBuffer,BufferSize);
	if (!bSts) {
		setupLastError(ThProcLibErrorCode::ThDllErr_InvalidRenderImage);
		TH_LIB_EXIT_FAILURE;
	}
#endif
	return TH_LIB_EXIT_SUCCESS;
}
//#97_get_mesh_folder
int CThProcMainSys::exe_get_path(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)
{
	Trace_Entry_L1;
	if (m_PcCoreLib == nullptr) return  TH_LIB_EXIT_FAILURE;
	std::wstring study_uid = cmd->strParam1;
	
	switch (cmd->cmdID) {
	case ThLibCmd_Get_StudyPath:
	{
		ThDcmMgrLib::UDICOMFolder_Def  folder;
		folder.m_dicom_folder[0] = 0;
		m_PcCoreLib->getStudyDateTimeUIDFolder(ThStringUtils::_convertStr(study_uid).c_str(), folder);
		_copy_wstr(cmd_ret->strParam1, folder.m_dicom_folder);
	}
		break;
	case ThLibCmd_Get_MeshPath:
	{//#132_XREAL_iPhone_Different_subFolder
		ThPCCoreLib::eProcStatus modelType;
		switch (cmd->intParam) {
		case 	ThLibCmdOutput_Xreal:
			modelType = ThPCCoreLib::eProcStatus_Xreal;
			break;
		case ThLibCmdOutput_iPhone:
			modelType = ThPCCoreLib::eProcStatus_iPhone;
		}
		std::wstring path_str = CThProcMainSys::get_mesh_folder_from_cmd(cmd);
		if (path_str.empty()) {
			cmd_ret->strParam1[0] = 0;
		}
		else {
			_copy_wstr(cmd_ret->strParam1, path_str.c_str());
		}
	}
	break;
	default:
		return  TH_LIB_EXIT_FAILURE;
		break;
	}
	
//	strncpy_s((char*)DataBuffer, BufferSize, ThStringUtils::_convertStrUtf8(folder.m_dicom_folder).c_str(), BufferSize);
	return TH_LIB_EXIT_SUCCESS;
}
std::wstring CThProcMainSys::get_mesh_folder_from_cmd(const ThProcLib_Cmd* cmd) const
{
	ThDcmMgrLib::UDICOMFolder_Def  folder;
	folder.m_dicom_folder[0] = 0;
	std::wstring study_uid = cmd->strParam1;
	{//#132_XREAL_iPhone_Different_subFolder
		ThPCCoreLib::eProcStatus modelType;
		switch (cmd->intParam) {
		case 	ThLibCmdOutput_Xreal:
			modelType = ThPCCoreLib::eProcStatus_Xreal;
			break;
		case ThLibCmdOutput_iPhone:
			modelType = ThPCCoreLib::eProcStatus_iPhone;
		}
		m_PcCoreLib->getMeshFolder(ThStringUtils::_convertStr(study_uid).c_str(), modelType, folder);
	}
	return folder.m_dicom_folder;
}
int CThProcMainSys::exe_cmd_load_vol(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)
{
	Trace_Entry_L1;
	CThProcMain::exe_cmd_async(cmd, cmd_ret);
	
	m_thread_cmd_async.m_proc_hdr.reset(new CMyLoadVolProc(this, cmd_ret->cmdUID));
	m_thread_cmd_async.m_theExe.reset(new CMultiThExe(1, m_thread_cmd_async.m_proc_hdr.get()));

	return TH_LIB_EXIT_SUCCESS;
}
//#103_load3DModel_command
int CThProcMainSys::exe_cmd_load_3DModle(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)
{
 //“¯ŠúŽÀs
	Trace_Entry_L1;
	std::string study_uid = ThStringUtils::_convertStr(m_current_cmd.strParam1);
	bool bIsHaiya = true;
	ThDcmMgrLib::DcmSeriesList* series_list = m_PcCoreLib->getSeriesList(study_uid.c_str());
	if (series_list == nullptr) return TH_LIB_EXIT_FAILURE;
	if (series_list->getSize() < 1) return TH_LIB_EXIT_FAILURE;
	bool bLoadSts = false;
 
	for (int i = 0; i < series_list->getSize(); i++) {
		ThDcmMgrLib::UDICOMSeries_Def Series;
		series_list->getSeries(i, Series);
		bool bIsHaiya = Series.m_windowCenter < 0;
		CThProcRender* pRender = nullptr;
		if (bIsHaiya&&(m_masterSeriesType==ThLibSeriesType_Haiya)) {
			pRender = m_renders[ThLibSeriesType_Haiya];
		}
		else if((m_masterSeriesType == ThLibSeriesType_Juukaku)){
			pRender = m_renders[ThLibSeriesType_Juukaku];
		}
		if (pRender == nullptr) continue;
		//#154_Image_server_not_working
		if (!pRender->checkClientNvr()) {
			return TH_LIB_EXIT_FAILURE;
		}
		 
		bLoadSts = pRender->load3DModel(study_uid.c_str(), Series.m_seriesInstanceUID);
		if (!bLoadSts) return TH_LIB_EXIT_FAILURE;
		break;//just do once
	}
 
	return TH_LIB_EXIT_SUCCESS;
 
}
//#137_check_image_server
int CThProcMainSys::exe_cmd_check_image_server(const ThProcLib_Cmd* cmd, ThProcLib_CmdOutput* cmd_ret)
{
	bool bSts = true;
	//#137_check_image_server
	for (auto it : m_renders) {
		if (!it.second->checkClientNvr()) {
			bSts = false;
			break;
		}
	}
	cmd_ret->intParam = bSts ? 1 : 0;
	return TH_LIB_EXIT_SUCCESS;
}
int CThProcMainSys::th_lib_get_vol_info(const ThProcLib_Cmd* cmd, ThProcLib_Vol3DData* outVol)
{
	Trace_Entry_L2;
	int volID = cmd->intParam;
	ThProcLib_Vol3DData* pVol = get_vol3D_ID(volID);
	 
	if (pVol == nullptr) {
		//#167_last_error
		//not net, just test it
		setupLastError(ThProcLibErrorCode::ThDllErr_InvalidVolID);
		return TH_LIB_EXIT_FAILURE;
	}
	*outVol = *pVol;
	return TH_LIB_EXIT_SUCCESS;
}
int CThProcMainSys::th_lib_render_GC(const ThProcLib_Cmd* cmd, ThProcLib_RenderGC* outGC)
{
	Trace_Entry_L2;
	if (CThProcMain::th_lib_render_GC(cmd, outGC) != TH_LIB_EXIT_SUCCESS) return TH_LIB_EXIT_FAILURE;
	switch (cmd->cmdID) {
	case ThLibCmd_Render_Create_GC:
		//done in CThProcMain::th_lib_render_GC
		break;
	case ThLibCmd_Render_Op:
		LogMessage(L"ThLibCmd_Render_Op \n");
		break;
	}

	 
	//int volID = cmd->intParam;
	//ThProcLib_Vol3DData* pVol = get_vol3D_ID(volID);
	//if(pVol==nullptr) return TH_LIB_EXIT_FAILURE;
	 
	return TH_LIB_EXIT_SUCCESS;
}
int CThProcMainSys::th_lib_exe_render(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, ThProcLib_Image2DData* outImage)
{
	Trace_Entry_L2;
	int sts = CThProcMain::th_lib_exe_render(cmd, param, outImage);
	if (sts != TH_LIB_EXIT_SUCCESS) return sts;

	eSeriesType series_type = (eSeriesType)param->series_type;
	CThProcRender* render_p = findRender(series_type);
	if (render_p == nullptr) {
		LogMessage(L"invalid series_type :%d \n", series_type);
		TH_LIB_EXIT_FAILURE;
	}
	if (m_log_level >= 2) {
		LogMessage(L" render_cmd_major: %d\n ", param->render_cmd_major);
	}
	int retSts = TH_LIB_EXIT_SUCCESS;
//	sts = th_lib_render_vol_dummy(cmd, param, outImage);
	switch (param->render_cmd_major) {
	case ThLibRenderCmd_2D:
	{
		//LogMessage(L"renderAxial pan [%.1f,%.1f],zoo: [%.2f] \n", param->panX, param->panY,param->zoom);
		int slice_pos = param->slice_position;
		if (slice_pos < 0) slice_pos = 0;
		if (!render_p->renderAxial(param)) {
			LogMessage(L"renderAxial error\n");
			retSts = TH_LIB_EXIT_FAILURE;
		}
		else {
			outImage->format = ImageFormat_32ARGB;
			outImage->sizeX = param->target_sizeX;
			outImage->sizeY = param->target_sizeY;
			outImage->ptichX = outImage->ptichY = 0.1f;
			int imageID = outImage->imageID;
			m_render_img.insert(std::map<int, RenderImg*>::value_type(outImage->imageID,
				new RenderImg(render_p, outImage)));
			//#115_added_Polygon
			m_curSlicePosition = param->slice_position;
		}

	}
	break;
	case ThLibRenderCmd_3D:
	{
		//LogMessage(L"render3D pan [%.1f,%.1f],zoo: [%.2f] \n", param->panX, param->panY, param->zoom);
		int slice_pos = param->slice_position;
		if (slice_pos < 0) slice_pos = 0;
		if (!render_p->render3D(param)) {
			LogMessage(L"renderAxial error\n");
			retSts = TH_LIB_EXIT_FAILURE;
		}else{
			outImage->format = ImageFormat_32ARGB;
			outImage->sizeX = param->target_sizeX;
			outImage->sizeY = param->target_sizeY;
			outImage->ptichX = outImage->ptichY = 0.1f;
			int imageID = outImage->imageID;
			m_render_img.insert(std::map<int, RenderImg*>::value_type(outImage->imageID,
				new RenderImg(render_p, outImage)));
		}

	}
	break;
	}
	return retSts ;
}
int CThProcMainSys::proc_load_vol_in(void)
{
	Trace_Entry_L1;
	m_Roi_Pen_Width = 2;
//	ThDcmMgrLib::UDICOMFolder_Def dcm_folder;
	std::string study_uid = ThStringUtils::_convertStr(m_current_cmd.strParam1);
	bool bIsHaiya = true;
	ThDcmMgrLib::DcmSeriesList* series_list = m_PcCoreLib->getSeriesList(study_uid.c_str());
	if(series_list == nullptr) return TH_LIB_EXIT_FAILURE;
	if(series_list->getSize()<1) return TH_LIB_EXIT_FAILURE;
	bool bLoadVolSts = false;
	std::map<int, bool> Series_Loaded_Flag;
	Series_Loaded_Flag[ThLibSeriesType_Haiya] = false;
	Series_Loaded_Flag[ThLibSeriesType_Juukaku] = false;

	//#203_clear_exe_status
	for (auto it : m_renders) {
		it.second->clearExeStatus();
	}
	for (int i = 0; i < series_list->getSize(); i++) {
		ThDcmMgrLib::UDICOMSeries_Def Series;
		series_list->getSeries(i,Series);
		bool bIsHaiya = Series.m_windowCenter < 0;
		CThProcRender* pRender = nullptr;
		if (bIsHaiya) {
			pRender = m_renders[ThLibSeriesType_Haiya];
		}
		else {
			pRender = m_renders[ThLibSeriesType_Juukaku];
		}
		//#154_Image_server_not_working
		if (!pRender->checkClientNvr()) {
			return TH_LIB_EXIT_FAILURE;;
		}
		bLoadVolSts = pRender->loadVol(study_uid.c_str(), Series.m_seriesInstanceUID);
		if (!bLoadVolSts) {
			setupLastError(ThProcLibErrorCode::ThDllErr_LoadVolFailed);
			return TH_LIB_EXIT_FAILURE;
		}
		if (bIsHaiya) {
			Series_Loaded_Flag[ThLibSeriesType_Haiya] = true;
		}
		else {
			Series_Loaded_Flag[ThLibSeriesType_Juukaku] = true;
		}
 
		//#100_load_3DModel_and_show_3DModel
		//ˆê•”•\Ž¦‚ÌŽb’è
	 //	pRender->show3DModel(study_uid.c_str(), Series.m_seriesInstanceUID,false, CThProcRender::eModel_all);
	//	pRender->show3DModel(study_uid.c_str(), Series.m_seriesInstanceUID,true, CThProcRender::eModel_lung);
	//	pRender->show3DModel(study_uid.c_str(), Series.m_seriesInstanceUID,true, CThProcRender::eModel_roi_marker);
		
	}

	int Series_Loaded_count = 0;
	for (auto it : Series_Loaded_Flag) {
		if (it.second) {
			Series_Loaded_count++;
		}
		else {
			if (it.first == ThLibSeriesType_Haiya) {
				LogMessage(L"*** No Haiya Vol. data \n");
			}
			else {
				LogMessage(L"*** No Juukaku Vol. data \n");
			}
		}
	}
	if (Series_Loaded_count < 2) {
 
		setupLastError(ThProcLibErrorCode::ThDllErr_No2SeriesData);
		return TH_LIB_EXIT_FAILURE;
	}
	return TH_LIB_EXIT_SUCCESS;
//	return CThProcMain::proc_load_vol_in();
}

int CThProcMainSys::create_vol_info(void)
{
	Trace_Entry_L2;
	const ThProcLib_Vol3DData *pVol3DData_temp = nullptr;
	pVol3DData_temp = m_renders[ThLibSeriesType_Haiya]->getVol3DData();
	if(pVol3DData_temp==nullptr) m_renders[ThLibSeriesType_Juukaku]->getVol3DData();
	if (pVol3DData_temp == nullptr) {
		LogMessage(L"getVol3DData error \n");
		return TH_LIB_EXIT_FAILURE;
	}
	const ThProcLib_Cmd* cmd = &m_current_cmd;
	ThProcLib_Vol3DData new_vol;
	new_vol = *pVol3DData_temp;
	new_vol.volID = generate_vol3D_ID();
 
	_copy_wstr(new_vol.StudyUID, cmd->strParam1);
	this->m_Vol3DList[new_vol.volID] = new_vol;
	m_current_volID = new_vol.volID;

	return TH_LIB_EXIT_SUCCESS;
}
	//#50_output_model.json
int CThProcMainSys::output_model_file(const ThProcLib_Cmd* cmd)
{
	Trace_Entry_L1;
	LogMessage(L"output_model_file [%s] \n", cmd->strParam1);

	std::string study_uid = ThStringUtils::_convertStr(cmd->strParam1);
	ThDcmMgrLib::UDICOMStudy_Def study_data;
	if (!m_PcCoreLib->getStudyData(study_uid.c_str(), study_data)) {
		LogMessage(L"output_model_file  getStudyData error \n");
		return TH_LIB_EXIT_FAILURE;
	};
	//#132_XREAL_iPhone_Different_subFolder
	std::vector<ThPCCoreLib::eProcStatus> modelTypeList;
	switch (cmd->intParam) {
	case 	ThLibCmdOutput_Xreal:
		modelTypeList.push_back(ThPCCoreLib::eProcStatus_Xreal);
		break;
	case ThLibCmdOutput_iPhone:
		modelTypeList.push_back(ThPCCoreLib::eProcStatus_iPhone);
	 
		break;
	case ThLibCmdOutput_Xreal_iPhone:
		modelTypeList.push_back(ThPCCoreLib::eProcStatus_Xreal);
		modelTypeList.push_back(ThPCCoreLib::eProcStatus_iPhone);
		break;
	}
	for (int m_i = 0; m_i < modelTypeList.size(); m_i++) {
		if (!m_PcCoreLib->outputModelFile(study_data.m_studyInstanceUID, modelTypeList[m_i])) {
			LogMessage(L"output_model_file  outputModelFile error \n");
			return TH_LIB_EXIT_FAILURE;
		};
	}
	return TH_LIB_EXIT_SUCCESS;
}

#ifdef UseDummyData
 
//not yet, do dummy here
int CThProcMainSys::th_lib_render_vol_dummy(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, ThProcLib_Image2DData* outImage)
{

	//	switch(param->render_type)
	outImage->format = ImageFormat_32ARGB;
	outImage->sizeX = param->target_sizeX;
	outImage->sizeY = param->target_sizeY;
	outImage->ptichX = outImage->ptichY = 0.1f;
	int imageID = outImage->imageID;
	m_render_img_dummy.insert(std::map<int, RenderImgDummy*>::value_type(outImage->imageID, new RenderImgDummy(outImage)));
	RenderImgDummy* render_img = th_lib_get_image_dummy(imageID);
	if (render_img == nullptr) return TH_LIB_EXIT_FAILURE;
	if (!render_img->creageImage()) return TH_LIB_EXIT_FAILURE;
	ARGB_def* p_ARGB_Img = (ARGB_def*)(render_img->m_image_buffer.get());
	int slice_pos = param->slice_position;
	if (slice_pos < 0) slice_pos = 0;
	for (int y_i = 0; y_i < outImage->sizeY; y_i++) {
		for (int x_i = 0; x_i < outImage->sizeX; x_i++) {
			ARGB_def& pixel = p_ARGB_Img[y_i * outImage->sizeX + x_i];
			float vx = ((x_i + slice_pos) % 125) / 125.0f;
			float vy = 0.5f + 0.5f * (((y_i) % 125) / 125.0f);
			unsigned char v = (unsigned char)(vx * vy * 255);
			pixel.A = 255;
			pixel.R = v;
			pixel.G = v;
			pixel.B = v;
		}
	}
	return TH_LIB_EXIT_SUCCESS;
}


RenderImgDummy* CThProcMainSys::th_lib_get_image_dummy(int imageID)
{
	RenderImgDummy* ret_p = nullptr;
	for (auto it = m_render_img_dummy.begin(); it != m_render_img_dummy.end(); it++) {
		if (it->second->m_p_imge_info.imageID == imageID) {
			ret_p = it->second;
			break;
		}
	}
	return ret_p;
}
#endif

RenderImg* CThProcMainSys::th_lib_get_image(int imageID)
{
	Trace_Entry_L2;
	RenderImg* ret_p = nullptr;
	for (auto it = m_render_img.begin(); it != m_render_img.end(); it++) {
		if (it->second->m_p_imge_info.imageID == imageID) {
			ret_p = it->second;
			break;
		}
	}
	return ret_p;
}
void  CThProcMainSys::th_lib_delete_image(int imageID)
{
	Trace_Entry_L2;
	RenderImg* p = nullptr;
	for (auto it = m_render_img.begin(); it != m_render_img.end(); it++) {
		if (it->second->m_p_imge_info.imageID == imageID) {
			p = it->second;

		}
	}
	if (p) {
		m_render_img.erase(imageID);
		delete p;
	}
	return;
}
void CThProcMainSys::setupDisplay3Dmodel(int DispParts)
{
	Trace_Entry_L2;
	for (auto it : m_renders) {
		if (it.second != nullptr) 
		{
			it.second->setupDisplay3Dmodel(DispParts);
		}

	}
}
int CThProcMainSys::exe_cmd_proc_mouseEvent(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, int& next_render_cmds)
{
	Trace_Entry_L2;
	 
	eSeriesType series_type = (eSeriesType)param->series_type;
	CThProcRender* pRender = findRender(series_type);

	switch (param->mouse_type)
	{
	case ThLibMouseEvent_ROI_Pen:
	{
		LogMessage(L"ROI_pen: [%d,%d]-%d,%d \n",
			param->target_sizeX, param->target_sizeY,
			param->mouse_posX, param->mouse_posY);
		pRender->setpPenWidth(m_Roi_Pen_Width);
		ThProcLib_RenderGC new_param = *param;
		new_param.slice_position = m_curSlicePosition;
		pRender->procMouseEvent_AddPolygon(&new_param);
		//#123_request_next_render_cmd_from_DLL
		next_render_cmds |= ThLibRRCmd_2D_1;
		next_render_cmds |= ThLibRRCmd_3D;
	}
	break;
	case ThLibMouseEvent_ROI_Eraser:
	{
		LogMessage(L"ROI_Eraser: [%d,%d]-%d,%d \n",
			param->target_sizeX, param->target_sizeY,
			param->mouse_posX, param->mouse_posY);
		pRender->setpPenWidth(m_Roi_Pen_Width);
		ThProcLib_RenderGC new_param = *param;
		new_param.slice_position = m_curSlicePosition;
	 	pRender->procMouseEvent_ErasePolygon(&new_param);
		//#123_request_next_render_cmd_from_DLL
		//next_render_cmds |= ThLibRRCmd_2D_1;
		//next_render_cmds |= ThLibRRCmd_3D;
	}
	break;
	case ThLibMouseEvent_LBUTTONDOWN:
		LogMessage(L"LBUTTONDOWN: [%d,%d]-%d,%d \n",
			param->target_sizeX, param->target_sizeY,
			param->mouse_posX, param->mouse_posY);
		if (m_Roi_Pen_Status == ThLibCmd_ROI_Erase) {
			//do as ThLibMouseEvent_ROI_Eraser
			ThProcLib_RenderGC new_param = *param;
			new_param.slice_position = m_curSlicePosition;
			pRender->procMouseEvent_ErasePolygon(&new_param);
		}
	case ThLibMouseEvent_LBUTTONUP:
		LogMessage(L"LBUTTONUP: [%d,%d]-%d,%d \n",
			param->target_sizeX, param->target_sizeY,
			param->mouse_posX, param->mouse_posY);
		
	default:
		LogMessage(L"default: [%d]- [%d,%d]-%d,%d \n",
			param->mouse_type,
			param->target_sizeX, param->target_sizeY,
			param->mouse_posX, param->mouse_posY);
		pRender->procMouseEvent(param, next_render_cmds);
		break;
	}
	
	return TH_LIB_EXIT_SUCCESS;
}
int CThProcMainSys::exe_cmd_ROI_final(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, int& next_render_cmds)
{
	Trace_Entry_L1;
	CThProcRender* pRender;
	bool bIsHaiya = true;// param->series_type == ThLibSeriesType_Haiya;
	if (bIsHaiya) {
		pRender = m_renders[ThLibSeriesType_Haiya];
	}
	else {
		pRender = m_renders[ThLibSeriesType_Juukaku];
	}
	pRender->saveRoiData(param);
	return TH_LIB_EXIT_SUCCESS;
}
int CThProcMainSys::exe_cmd_ROI_Clear(const ThProcLib_Cmd* cmd, const ThProcLib_RenderGC* param, int& next_render_cmds)
{
	Trace_Entry_L1;
	 
	CThProcRender* pRender = m_renders[this->m_masterSeriesType];
	pRender->clearRoiData(param);
	next_render_cmds |= ThLibRRCmd_2D_1;
	next_render_cmds |= ThLibRRCmd_3D;
	return TH_LIB_EXIT_SUCCESS;
}
void CThProcMainSys::dumpLastError(void)
{
	LogMessage(L"dump last Error: [%s]-%s\n", m_last_error_code.c_str(), m_last_error_msg.c_str());
}
void CThProcMainSys::setupLastError(int error)
{
	m_last_error_code = ThProcLibErrorCode::getErrCode(error);
	m_last_error_msg = ThProcLibErrorCode::getErrStr(error);

	dumpLastError();
}