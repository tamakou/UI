 
#include "ThProcLibMainSys.h"
#include "ThStringUtils.h"
 
inline
bool _check_type_is_output(int procStatus)
{
    bool bRet = false;
    if (procStatus >= (ThPCCoreLib::eProcStatus_Xreal)) {
        bRet = true;
    }
    else {
        bRet = false;
    }
    return bRet;
}
int CThProcMainSys::get_datalist_in(ThProcLib_PatientInfo* out_info, int type, int no)
{
    if (m_PcCoreLib == nullptr) return  TH_LIB_EXIT_FAILURE;
    ThDcmMgrLib::DcmStudyList* study_list = m_PcCoreLib->getStudyList();
    bool bFoundData = false;
    ThDcmMgrLib::UDICOMStudy_Def outStudy;
    //
    if (study_list != nullptr) {
        int chk_no = -1;
        if (ThLibQueryList_OutputData == type) {
            for (int i = 0; i < study_list->getSize(); i++) {

                if (study_list->getStudy(i, outStudy)) {
                    if (_check_type_is_output(outStudy.m_procStatus)) {
                        chk_no++;
                    }
                    if (chk_no == no) {
                        bFoundData = true;
                        break;
                    };
                }
            }
        }
        else {
            if (study_list->getStudy(no, outStudy)) {
                bFoundData = true;
            };
        }
        study_list->Destroy();  
    }
    memset(out_info, 0, sizeof(ThProcLib_PatientInfo));
    if (!bFoundData)  return TH_LIB_EXIT_FAILURE;
    //PatientName
    _copy_wstr(out_info->PatientName, outStudy.m_patientName);
   //PatientID
    _copy_wstr(out_info ->PatientID, outStudy.m_patientID);
    //Birthday
    _copy_wstr(out_info->Birthday, outStudy.m_patientBirthDate);
    //Age
    out_info->Age = outStudy.m_patientAge;
    //Gender
    _copy_wstr(out_info->Gender, outStudy.m_patientSex);
    
    //StudyDate ;
    _copy_wstr(out_info->StudyDate, outStudy.m_studyDate);
    //StudyDateTime ;
    _copy_wstr(out_info->StudyTime, outStudy.m_studyTime);
    //SliceThickness
    out_info->SliceThickness = 1.0f;
    //CTMode
  //  wcscpy(out_info->CTMode, L"”x–ì");
    //ProcessStatus
     
    out_info->ProcessStatus = ThLibCmdProc_No_Roi;
    if ((outStudy.m_procStatus&ThPCCoreLib::eProcStatus_Roi)!=0) {
        out_info->ProcessStatus = ThLibCmdProc_Roi;
    }
    //UpdateDate
    _copy_wstr(out_info->UpdateDate, L"2025/02/21");
    //UpdateTime
    _copy_wstr(out_info->UpdateTime, L"12:11:22");
    //StudyPattern
    out_info->StudyPattern = 0;
    //StudDescription
    _copy_wstr(out_info->StudDescription, outStudy.m_studyDescription);
    //StudyUID
    _copy_wstr(out_info->StudyUID, ThStringUtils::_convertStr(outStudy.m_studyInstanceUID).c_str());
    //OutputType
    out_info->OutputType = ThLibCmdOutput_Unknown;
    {
        bool bXrealOn = (outStudy.m_procStatus & ThPCCoreLib::eProcStatus_Xreal) != 0 ;
        bool biPhoneOn = (outStudy.m_procStatus & ThPCCoreLib::eProcStatus_iPhone) != 0;
        if (bXrealOn&& biPhoneOn) {
            out_info->OutputType = ThLibCmdOutput_Xreal_iPhone;
        }
        else if (bXrealOn){
            out_info->OutputType = ThLibCmdOutput_Xreal;
        }else if (biPhoneOn) {
            out_info->OutputType = ThLibCmdOutput_iPhone;
        }
    }
    //check_dig
    out_info->CheckDig = 1234; //check bit, always  1234;
    return TH_LIB_EXIT_SUCCESS;
}
 
int CThProcMainSys::get_datalist_size_in(int type)
{
    int ret_num = 0;
    if (m_PcCoreLib == nullptr) return ret_num;
    ThDcmMgrLib::DcmStudyList* study_list = m_PcCoreLib->getStudyList();
    
    if (study_list != nullptr) {
#if 1
        if (ThLibQueryList_OutputData == type) {
            for (int i = 0; i < study_list->getSize(); i++) {
                ThDcmMgrLib::UDICOMStudy_Def outStudy;
                if (study_list->getStudy(i, outStudy)) {
                    if (_check_type_is_output(outStudy.m_procStatus)) {
                        ret_num++;
                    }
                };
            }
        }
        else {
#else
        {
#endif
            ret_num = study_list->getSize();
            study_list->Destroy();
        }
    }
    return ret_num;
}
//
#if 0 //to remove
unsigned long g_output_list_count = 5;
int th_lib_get_output_list_size_in(int type)
{
    return g_output_list_count;
}
int th_lib_get_output_list_in(ThProcLib_PatientInfo* out_info, int no)
{
  //  th_lib_get_datalist_in(out_info, no);
    out_info->OutputType = ThLibCmdOutput_Xreal;
    if (no % 2 == 0) {
        out_info->OutputType = ThLibCmdOutput_Xreal;
    }
    else {
        out_info->OutputType = ThLibCmdOutput_Xreal_iPhone;
    }
    return 0;
}
#endif