 
#include "ThProcLibMain.h"
#include "IThPCCoreLibDummy.h"
#include "ThStringUtils.h"
#ifdef _USE_JSON_FORMAT
#include "../rapidjson/document.h"
#include "../rapidjson/prettywriter.h"
#include "ThStringUtils.h"

std::string _Utf16toUtf8(const std::wstring& w_str) {
    typedef rapidjson::GenericStringStream<rapidjson::UTF16<> > StringStreamW;
    StringStreamW source(w_str.c_str());
    rapidjson::GenericStringBuffer<rapidjson::UTF8<> > target;
    bool hasError = false;
    while (source.Peek() != '\0')
        if (!rapidjson::Transcoder<rapidjson::UTF16<>, rapidjson::UTF8<> >::Transcode(source, target)) {
            hasError = true;
            break;
        }
    std::string ret_str;
    if (!hasError) {
        ret_str = target.GetString();
    }
    return ret_str;
}
std::wstring _Utf8toUtf16(const std::string& str) {
    typedef rapidjson::GenericStringStream<rapidjson::UTF8<> > StringStreamW;
    StringStreamW source(str.c_str());
    rapidjson::GenericStringBuffer<rapidjson::UTF16<> > target;
    bool hasError = false;
    while (source.Peek() != '\0')
        if (!rapidjson::Transcoder<rapidjson::UTF8<>, rapidjson::UTF16<> >::Transcode(source, target)) {
            hasError = true;
            break;
        }
    std::wstring ret_str;
    if (!hasError) {
        ret_str = target.GetString();
    }
    return ret_str;
}
 
//#define WRITE_STR  _write_val_utf8
#define WRITE_STR _write_val_utf16
class DataItem {
public:
    DataItem(void) {}
    virtual ~DataItem() {};

    DataItem& operator=(const DataItem& rhs) {
        PatientName     = rhs.PatientName;
        PatientID       = rhs.PatientID;
        Birthday        = rhs.Birthday;
        Gender          = rhs.Gender;
        StudyDateTime   = rhs.StudyDateTime;
        SliceThickness  = rhs.SliceThickness;
        CTMode          = rhs.CTMode;
        ProcessStatus   = rhs.ProcessStatus;
        UpdateDateTime  = rhs.UpdateDateTime;
        StudDescription = rhs.StudDescription;
        return *this;
    }
    std::wstring PatientName;
    std::wstring PatientID;
    std::wstring Birthday;
    std::wstring Gender;
    std::wstring StudyDateTime;
    std::wstring SliceThickness;
    std::wstring CTMode;
    std::wstring ProcessStatus;
    std::wstring UpdateDateTime;
    std::wstring StudDescription;
    template <typename Writer> void _write_val_utf8(Writer& writer_hd,const std::wstring& str) const
    {
        std::string utf8_str = _Utf16toUtf8(str);
        writer_hd.String(utf8_str.c_str(), (rapidjson::SizeType)utf8_str.length());
    }
    template <typename Writer> void _write_val_utf16(Writer& writer_hd, const std::wstring& str) const
    {
        writer_hd.String(str.c_str(), (rapidjson::SizeType)str.length());
    }
    template <typename Writer>
    void Serialize(Writer& writer) const {
        writer.StartObject();
        
        // This base class just write out name-value pairs, without wrapping within an object.
        writer.String(L"PatientName");
        WRITE_STR(writer, PatientName);
        //
        writer.String(L"PatientID");
        WRITE_STR(writer, PatientID);
        //
        writer.String(L"Birthday");
        WRITE_STR(writer, Birthday);
        //
        writer.String(L"Gender");
        WRITE_STR(writer, Gender);
        //
        writer.String(L"StudyDateTime");
        WRITE_STR(writer, StudyDateTime);
        //
        writer.String(L"SliceThickness");
        WRITE_STR(writer, SliceThickness);
        //
        writer.String(L"CTMode");
        WRITE_STR(writer, CTMode);
        //
        writer.String(L"ProcessStatus");
        WRITE_STR(writer, ProcessStatus);
        //
        writer.String(L"UpdateDateTime");
        WRITE_STR(writer, UpdateDateTime);
        //
        writer.String(L"StudDescription");
        WRITE_STR(writer, StudDescription);
 
        writer.EndObject();
    }
protected:
private:
    
};


int th_lib_get_datalist(wchar_t* pListBuffer, int paramBufferSize)
{
 
    std::vector<DataItem> data_list;

    DataItem item;
    item.PatientName = L"thoracentes test1";
    item.PatientID = L"12349001";
    item.Birthday = L"2000/01/02";
    item.Gender = L"男";
    item.StudyDateTime = L"2025/05/11 12:01:20";
    item.SliceThickness = L"1.0mm";
    item.CTMode = L"肺野";
    item.ProcessStatus = L"②症例選択";
    item.UpdateDateTime = L"2025/06/11 13:11:21";;
    item.StudDescription = L"comment test";
    data_list.push_back(item);
    //
    item.PatientName = L"テスト 太郎";
    item.PatientID = L"12349002";
    item.Birthday = L"2000/01/02";
    item.Gender = L"男";
    data_list.push_back(item);

    ///
    typedef rapidjson::GenericStringBuffer<rapidjson::UTF16<> > StringBufferUtf16;

    //  rapidjson::StringBuffer sb;
      //rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    StringBufferUtf16  sb;
    rapidjson::PrettyWriter<StringBufferUtf16, rapidjson::UTF16<>, rapidjson::UTF16<>> writer(sb);

    writer.StartArray();
    for (auto dataItr = data_list.begin(); dataItr != data_list.end(); ++dataItr)
        dataItr->Serialize(writer);
    writer.EndArray();

    auto json_str = sb.GetString();

    wcscpy(pListBuffer, json_str);


 
    return 0;
}
#endif 

#if 1
int th_lib_setup_datalist_in(ThProcLib_PatientInfo* out_info, int no)
{
    return 0;
}
int th_lib_cnv_datalist_from_DB(ThProcLib_PatientInfo* out_info, ThPCCoreLibDummy::UDICOMStudy_Def* DB_info)
{
    bool bIsFemal = false;
    
    //PatientName
    _copy_wstr(out_info->PatientName, DB_info->m_patientName);
   //PatientID
    _copy_wstr(out_info->PatientID, DB_info->m_patientID);
    //Birthday
    _copy_wstr(out_info->Birthday, DB_info->m_patientBirthDate);
    //Age
    out_info->Age = DB_info->m_patientAge;
    //Gender
    _copy_wstr(out_info->Gender, DB_info->m_patientSex);// bIsFemal ? L"女" : L"男");
    
    //StudyDate ;
    _copy_wstr(out_info->StudyDate, DB_info->m_studyDate);
    //StudyDateTime ;
    _copy_wstr(out_info->StudyTime, DB_info->m_studyTime);
    //SliceThickness
    out_info->SliceThickness = 1.0f;
   
    out_info->ProcessStatus = ThLibCmdProc_No_Roi;
    if ((DB_info->m_procStatus & ThPCCoreLibDummy::eProcStatus_Roi) != 0) {
        out_info->ProcessStatus = ThLibCmdProc_Roi;
    }
    //UpdateDate
    _copy_wstr(out_info->UpdateDate, L"2025/02/21");
    //UpdateTime
    _copy_wstr(out_info->UpdateTime, L"12:11:22");
    //StudyPattern
    out_info->StudyPattern = 0;
    //StudDescription
    _copy_wstr(out_info->StudDescription, DB_info->m_studyDescription);
    //StudyUID
    _copy_wstr(out_info->StudyUID, ThStringUtils::_convertStr(DB_info->m_studyInstanceUID).c_str());
    //OutputType
    out_info->OutputType = ThLibCmdOutput_Unknown;
    {
        bool bXrealOn = (DB_info->m_procStatus & ThPCCoreLibDummy::eProcStatus_Xreal) != 0;
        bool biPhoneOn = (DB_info->m_procStatus & ThPCCoreLibDummy::eProcStatus_iPhone) != 0;
        if (bXrealOn && biPhoneOn) {
            out_info->OutputType = ThLibCmdOutput_Xreal_iPhone;
        }
        else if (bXrealOn) {
            out_info->OutputType = ThLibCmdOutput_Xreal;
        }
        else if (biPhoneOn) {
            out_info->OutputType = ThLibCmdOutput_iPhone;
        }
    }

    //UpdateDate
    wcscpy(out_info->UpdateDate, L"2025/02/21");
    //UpdateTime
    wcscpy(out_info->UpdateTime, L"12:11:22");
    //StudyPattern
    out_info->StudyPattern = 0;
    //StudDescription
    wcscpy(out_info->StudDescription, DB_info->m_studyDescription);
    //StudyUID
    _copy_wstr(out_info->StudyUID, ThStringUtils::_convertStr(DB_info->m_studyInstanceUID).c_str());
     
    //check_dig
    out_info->CheckDig = 1234; //check bit, always  1234;
    return  TH_LIB_EXIT_SUCCESS;
}
#else
#endif
int th_lib_setup_datalist_in_DB(ThPCCoreLibDummy::UDICOMStudy_Def* out_info, int no)
{
    bool bIsFemal = false;
    if (no % 3 == 0)  bIsFemal = true;
    //PatientName
    std::wstring name_base = L"テスト　";
    name_base += bIsFemal ? L"花子" : L"太郎";
    std::wstring name = name_base + std::to_wstring(no + 1);

    _copy_wstr(out_info->m_patientName, name.c_str());
    //PatientID
    std::wstring id = std::to_wstring(12340 + no + 1);
    _copy_wstr(out_info->m_patientID, id.c_str());
    //Birthday
    _copy_wstr(out_info->m_patientBirthDate, L"2011/01/01");
    //Age
    out_info->m_patientAge  = 23;
    //Gender
    _copy_wstr(out_info->m_patientSex, bIsFemal ? L"女" : L"男");

    //StudyDate ;
    _copy_wstr(out_info->m_studyDate, L"2021/01/11");
    //StudyDateTime ;
    _copy_wstr(out_info->m_studyTime, L"12:01:23");
    //
    out_info->m_procStatus = ThPCCoreLibDummy::eProcStatus_Init;
#if 0
    //SliceThickness
//    out_info-> SliceThickness = 1.0f;
    //CTMode
  //  wcscpy(out_info->CTMode, L"肺野");
    //ProcessStatus
    int proc_sts = 0;
    {
        if (no % 3 == 0) {
            proc_sts = 1;
        }
    }
    out_info->ProcessStatus = proc_sts;

    //UpdateDate
    wcscpy(out_info->UpdateDate, L"2025/02/21");
    //UpdateTime
    wcscpy(out_info->UpdateTime, L"12:11:22");
    //StudyPattern
    out_info->StudyPattern = 0;
#endif

    //StudDescription
    _copy_wstr(out_info->m_studyDescription, L"test comment");
    //StudyUID
    std::string studyUID = "290.33.2000.3849.333." + std::to_string(no);
    _copy_str(out_info->m_studyInstanceUID, studyUID.c_str());
    
    return  TH_LIB_EXIT_SUCCESS;
}
 

const unsigned long g_datalist_count = 50;
//std::map<std::wstring, ThProcLib_PatientInfo*> g_data_list;
std::map<std::wstring, ThPCCoreLibDummy::UDICOMStudy_Def*> g_data_list;//do it like real DB.

void init_data_list_DB()
{
    for (int i = 0; i < g_datalist_count; i++) {
        ThPCCoreLibDummy::UDICOMStudy_Def* new_item = new ThPCCoreLibDummy::UDICOMStudy_Def;
        th_lib_setup_datalist_in_DB(new_item,i);
        g_data_list[ThStringUtils::_convertStr(new_item->m_studyInstanceUID)] = new_item;
    }
}
int add_new_data(ThProcLib_PatientInfo &out) {
    int data_no = g_data_list.size();
    ThPCCoreLibDummy::UDICOMStudy_Def* new_item = new ThPCCoreLibDummy::UDICOMStudy_Def;
    
    th_lib_setup_datalist_in_DB(new_item, data_no);
    th_lib_cnv_datalist_from_DB(&out, new_item);

    return data_no;
}
#if 1
ThPCCoreLibDummy::UDICOMStudy_Def* th_lib_get_datalist_in_DB(std::wstring& study_uid) {
    ThPCCoreLibDummy::UDICOMStudy_Def* ret = nullptr;
    for (auto it = g_data_list.begin(); it != g_data_list.end(); it++) {
        if (study_uid ==  ThStringUtils::_convertStr(it->second->m_studyInstanceUID)) {
            ret = it->second;
            break;
       }
    }
    return ret;
}
#endif
int th_lib_get_datalist_size_in(int type)
{
    return g_data_list.size();
}
int th_lib_get_datalist_in(ThProcLib_PatientInfo* out_info, int no)
{
    int ret_no = 0;
    for (auto it = g_data_list.begin(); it != g_data_list.end(); it++) {
        if (ret_no++ == no) {
            ThProcLib_PatientInfo out_temp;
            th_lib_cnv_datalist_from_DB(&out_temp, it->second);
            memcpy(out_info,&out_temp,sizeof(ThProcLib_PatientInfo));
            break;
        }
    }
    return TH_LIB_EXIT_SUCCESS;
}
//
inline
bool _check_type_is_output(int procStatus)
{
    bool bRet = false;
    if (procStatus >= (ThPCCoreLibDummy::eProcStatus_Xreal)) {
        bRet = true;
    }
    else {
        bRet = false;
    }
    return bRet;
}
int th_lib_get_output_list_size_in(int type)
{
    int count = 0;
    for (auto it = g_data_list.begin(); it != g_data_list.end(); it++) {
        if (_check_type_is_output(it->second->m_procStatus)) {
            count++;
        }
    }
    return count;
}
int th_lib_get_output_list_in(ThProcLib_PatientInfo* out_info, int no)
{
    int sel_no = -1;
    int org_no = 0;
    for (auto it = g_data_list.begin(); it != g_data_list.end(); it++) {
        if (_check_type_is_output(it->second->m_procStatus)) {
            sel_no++;
        }
        if (no == sel_no) {
            th_lib_get_datalist_in(out_info, org_no);
            break;
        }
        org_no++;
    }
 
    return TH_LIB_EXIT_SUCCESS;
}
