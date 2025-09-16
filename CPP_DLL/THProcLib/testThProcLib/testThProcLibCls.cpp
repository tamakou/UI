// testThItkLib.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "math.h"

#include "testThProcLibCls.h"
 
void CTestThProcLib::check_cmd_finished(int cmdUID, ThProcLib_CmdStatus &status,int wait_inter)
{
 
    for (int i = 0; i < 800; i++) {

        dll_loader._dll_th_lib_get_status(cmdUID, &status);
        if (status.cmdStatus == -1) {
            dll_loader._dll_th_lib_get_status(-1, &status);
            printf("failed: \n"); 
            break;
        }
        if (status.cmdStatus != 0) {
            ::Sleep(wait_inter);
        }
        else {
            break;
        }
        printf("progress: %d \n", status.cmdProgress);
    }
}
 
CTestThProcLib::CTestThProcLib():m_bInitFlag(false)
{
    tbl_series[0] = ThLibSeriesType_Haiya;
    tbl_series[1] = ThLibSeriesType_Juukaku;
}
CTestThProcLib::~CTestThProcLib()
{
    if (m_bInitFlag)
    {
        dll_loader._dll_th_lib_close();
    }
}
bool CTestThProcLib::initDll(void)
{
    if (!dll_loader.init()) {
        printf("dll_loader init error\n");
        return false;
    }
    dll_loader._dll_th_lib_init(0);
    m_bInitFlag = true;
    return true;
}
 
bool CTestThProcLib::loadVol(const std::wstring& studyUID)
{
    ThProcLib_Cmd exe_cmd;
    ThProcLib_CmdOutput ret_cmd;
    ThProcLib_CmdStatus status;
    //load volume
    exe_cmd.cmdID = ThLibCmd_Render_Load_Vol3D;
    _copy_wstr(exe_cmd.strParam1, studyUID.c_str());
    dll_loader._dll_th_lib_exe_cmd(&exe_cmd, &ret_cmd);
    check_cmd_finished(ret_cmd.cmdUID, status, 1000);
    int volID = status.dataID;
    exe_cmd.intParam = volID;
    dll_loader._dll_th_lib_get_vol_info(&exe_cmd, &m_VolData);

    return true;
}
bool CTestThProcLib::createRenderGC(ThProcLib_RenderGC& renderGC)
{
    ThProcLib_Cmd exe_cmd;
    exe_cmd.cmdID = ThLibCmd_Render_Create_GC;
    exe_cmd.intParam = m_VolData.volID;
    int nSts = dll_loader._dll_th_lib_render_GC(&exe_cmd, &renderGC);
    return nSts == 0;
}
bool CTestThProcLib::render(ThProcLib_RenderGC& renderGC)
{
    ThProcLib_Cmd exe_cmd;
    exe_cmd.cmdID = ThLibCmd_Render_Vol3D;
    exe_cmd.intParam = m_VolData.volID;
    
    int nSts = dll_loader._dll_th_lib_exe_render(&exe_cmd, &renderGC, &m_RenderOutImage);
    if (nSts != 0) {
        printf("render: _dll_th_lib_exe_render error \n"); return false;
    }

    exe_cmd.cmdID = ThLibCmd_Render_Image2D;
    exe_cmd.intParam = m_RenderOutImage.imageID;
    int sizeXY = m_RenderOutImage.sizeX * m_RenderOutImage.sizeY;
    
    aImageBuffer.resize(sizeXY);
    unsigned char* pImageBuff = (unsigned char*)(&(aImageBuffer[0]));

    nSts = dll_loader._dll_th_lib_get_data(&exe_cmd, pImageBuff, sizeXY);
    if (nSts != 0) {
        printf("render: _dll_th_lib_get_data error \n"); return false;
    }
    return true;
}
void CTestThProcLib::releaseRenderGC(ThProcLib_RenderGC& renderGC)
{
    //release renderGC
    ThProcLib_Cmd exe_cmd;
    exe_cmd.cmdID = ThLibCmd_Release_GC;
    exe_cmd.intParam = renderGC.render_gc_id;
    ThProcLib_CmdOutput ret_cmd;
    dll_loader._dll_th_lib_exe_cmd(&exe_cmd, &ret_cmd);
}
void CTestThProcLib::releaseOutImage(void)
{
    ThProcLib_Cmd exe_cmd;
    exe_cmd.cmdID = ThLibCmd_Release_Image2D;
    exe_cmd.intParam = m_RenderOutImage.imageID;
    ThProcLib_CmdOutput ret_cmd;
    dll_loader._dll_th_lib_exe_cmd(&exe_cmd, &ret_cmd);
}
void CTestThProcLib::releaseVol(void)
{
    ThProcLib_Cmd exe_cmd;
    exe_cmd.cmdID = ThLibCmd_Release_Vol3D;
    exe_cmd.intParam = m_VolData.volID;
    ThProcLib_CmdOutput ret_cmd;
    dll_loader._dll_th_lib_exe_cmd(&exe_cmd, &ret_cmd);
}
void CTestThProcLib::saveImage(const std::wstring fileName)
{
    std::wstring file_str = fileName;

    file_str += (std::to_wstring(m_RenderOutImage.sizeX) + L"_" + std::to_wstring(m_RenderOutImage.sizeY) + L".raw");
    FILE* fp = nullptr;
    unsigned char* pImageBuff = (unsigned char*)(&(aImageBuffer[0]));

    _wfopen_s(&fp, file_str.c_str(), L"wb");
    fwrite(pImageBuff, 4, m_RenderOutImage.sizeX * m_RenderOutImage.sizeY, fp);
    fclose(fp);
}
void CTestThProcLib::testProc(void)
{
  
    ThProcLib_Env outEnv;
    dll_loader._dll_th_lib_get_env(&outEnv);
#if 1
    int out_size = 0;
    // char* data_ptr = dll_loader._dll_th_lib_get_datalist(&out_size);
    int ct_data_size = dll_loader._dll_th_lib_get_data_size(ThLibQueryList_CTData);
    for (int i = 0; i < ct_data_size; i++) {
        ThProcLib_PatientInfo _pat_info;
        int sts = dll_loader._dll_th_lib_get_datalist(&_pat_info, ThLibQueryList_CTData, i);
        std::wstring name = _pat_info.PatientName;
        std::wstring id = _pat_info.PatientID;
    }

    int output_data_size = dll_loader._dll_th_lib_get_data_size(ThLibQueryList_OutputData);
    for (int i = 0; i < output_data_size; i++) {
        ThProcLib_PatientInfo _pat_info;
        int sts = dll_loader._dll_th_lib_get_datalist(&_pat_info, ThLibQueryList_OutputData, i);
        std::wstring name = _pat_info.PatientName;
        std::wstring id = _pat_info.PatientID;
    }

    {
        ThProcLib_Env env;

        int sts = dll_loader._dll_th_lib_get_env(&env);
    }
    if(0){
        ThProcLib_Cmd exe_cmd;
        exe_cmd.cmdID = ThLibCmd_Get_DicomImageInfo;
        wcscpy_s(exe_cmd.strParam1, sizeof(exe_cmd.strParam1) / sizeof(wchar_t), L"123.33.45");

        ThProcLib_DicomImageInfo _image_info;
        int sts = dll_loader._dll_th_lib_get_image_info(&exe_cmd, &_image_info);
        int sizeX = _image_info.sizeX;
    }
 
#endif
 
    if (0) {

        ThProcLib_Cmd exe_cmd;
        ThProcLib_CmdOutput ret_cmd;
        exe_cmd.cmdID = ThLibCmd_Exe_ImportData;//import CD-data
        dll_loader._dll_th_lib_exe_cmd(&exe_cmd, &ret_cmd);

        {
            ThProcLib_Cmd exe_cmd;
            ThProcLib_CmdOutput ret_cmd1;
            exe_cmd.cmdID = ThLibCmd_Exe_AsyncCmd_Cancel;//import CD-data
            exe_cmd.intParam = ret_cmd.cmdUID;
            dll_loader._dll_th_lib_exe_cmd(&exe_cmd, &ret_cmd1);
        }
        ThProcLib_CmdStatus status;
        check_cmd_finished(ret_cmd.cmdUID, status, 5000);
        dll_loader._dll_th_lib_close_cmd(ret_cmd.cmdUID);

        int new_data_id = status.dataID;
        std::wstring new_study_uid = status.strParam1;
    }
    if(1){
        ThProcLib_Cmd exe_cmd;
        ThProcLib_CmdOutput ret_cmd;
        std::wstring new_study_uid = m_selectStudyUID;
        //
        exe_cmd.cmdID = ThLibCmd_Exe_AISegmentator;//AISegmentator
        _copy_wstr(exe_cmd.strParam1, new_study_uid.c_str());
        dll_loader._dll_th_lib_exe_cmd(&exe_cmd, &ret_cmd);

        ThProcLib_CmdStatus status;
        check_cmd_finished(ret_cmd.cmdUID, status, 5000);

        dll_loader._dll_th_lib_close_cmd(ret_cmd.cmdUID);
        //
        {
            int out_size = 0;
            // char* data_ptr = dll_loader._dll_th_lib_get_datalist(&out_size);
            int ct_data_size = dll_loader._dll_th_lib_get_data_size(ThLibQueryList_CTData);
            for (int i = 0; i < ct_data_size; i++) {
                ThProcLib_PatientInfo _pat_info;
                int sts = dll_loader._dll_th_lib_get_datalist(&_pat_info, ThLibQueryList_CTData, i);
                std::wstring name = _pat_info.PatientName;
                std::wstring id = _pat_info.PatientID;
                if (new_study_uid == _pat_info.StudyUID) {
                    printf("ProcStatus: %d\n", _pat_info.ProcessStatus);
                }
            }
        }
    }

    int xx = 0;
}

void CTestThProcLib::testProc2(void)
{
   

    ThProcLib_Env outEnv;
    dll_loader._dll_th_lib_get_env(&outEnv);

    int out_size = 0;

    std::wstring test_study_uid;
    int ct_data_size = dll_loader._dll_th_lib_get_data_size(ThLibQueryList_CTData);
    for (int i = 0; i < ct_data_size; i++) {
        ThProcLib_PatientInfo _pat_info;
        int sts = dll_loader._dll_th_lib_get_datalist(&_pat_info, ThLibQueryList_CTData, i);
        std::wstring name = _pat_info.PatientName;
        std::wstring id = _pat_info.PatientID;
        test_study_uid = _pat_info.StudyUID;
    }

    int output_data_size = dll_loader._dll_th_lib_get_data_size(ThLibQueryList_OutputData);
    for (int i = 0; i < output_data_size; i++) {
        ThProcLib_PatientInfo _pat_info;
        int sts = dll_loader._dll_th_lib_get_datalist(&_pat_info, ThLibQueryList_OutputData, i);
        std::wstring name = _pat_info.PatientName;
        std::wstring id = _pat_info.PatientID;
    }
    test_study_uid = L"1.2.392.200069.17.10338133111.1202505010003201";
    {
        ThProcLib_Env env;

        int sts = dll_loader._dll_th_lib_get_env(&env);
    }
    {//study path
        ThProcLib_Cmd exe_cmd;
        exe_cmd.cmdID = ThLibCmd_Get_StudyPath;
        _copy_wstr(exe_cmd.strParam1, test_study_uid.c_str());
        ThProcLib_CmdOutput ret_cmd;
        dll_loader._dll_th_lib_exe_cmd(&exe_cmd, &ret_cmd);

        exe_cmd.cmdID = ThLibCmd_Get_MeshPath;
        _copy_wstr(exe_cmd.strParam1, test_study_uid.c_str());

        dll_loader._dll_th_lib_exe_cmd(&exe_cmd, &ret_cmd);

        int xx = 0;
    }

    {//mesh path
        ThProcLib_Cmd exe_cmd;
        exe_cmd.cmdID = ThLibCmd_Get_MeshPath;
        _copy_wstr(exe_cmd.strParam1, test_study_uid.c_str());
        std::vector<unsigned char> a_str_buffer;
        a_str_buffer.resize(1024);
        unsigned char* pBuffer = &(a_str_buffer[0]);
        int str_size = a_str_buffer.size();
        dll_loader._dll_th_lib_get_data(&exe_cmd, pBuffer, str_size);
        int xx = 0;
    }
    if (1) {//CT-Marker

        ThProcLib_Cmd exe_cmd;
        ThProcLib_CmdOutput ret_cmd;
        exe_cmd.cmdID = ThLibCmd_CT_Marker_Final;
        _copy_wstr(exe_cmd.strParam1, test_study_uid.c_str());
        dll_loader._dll_th_lib_exe_cmd(&exe_cmd, &ret_cmd);
        exe_cmd.cmdID = ThLibCmd_CT_Marker_Clear;
        _copy_wstr(exe_cmd.strParam1, test_study_uid.c_str());
        dll_loader._dll_th_lib_exe_cmd(&exe_cmd, &ret_cmd);
    }
    if (0) {//ROI

        ThProcLib_Cmd exe_cmd;
        ThProcLib_CmdOutput ret_cmd;
        exe_cmd.cmdID = ThLibCmd_ROI_Final;
        _copy_wstr(exe_cmd.strParam1, test_study_uid.c_str());
        dll_loader._dll_th_lib_exe_cmd(&exe_cmd, &ret_cmd);
        exe_cmd.cmdID = ThLibCmd_ROI_Clear;
        _copy_wstr(exe_cmd.strParam1, test_study_uid.c_str());
        dll_loader._dll_th_lib_exe_cmd(&exe_cmd, &ret_cmd);
    }
    {//Output

        ThProcLib_Cmd exe_cmd;
        ThProcLib_CmdOutput ret_cmd;
        exe_cmd.cmdID = ThLibCmd_Output_Remove;
        exe_cmd.intParam = ThLibCmdOutput_Xreal;
        _copy_wstr(exe_cmd.strParam1, test_study_uid.c_str());
        dll_loader._dll_th_lib_exe_cmd(&exe_cmd, &ret_cmd);
        ThProcLib_CmdStatus status;
        check_cmd_finished(ret_cmd.cmdUID, status, 500);

        //add Xreal
        exe_cmd.cmdID = ThLibCmd_Output_Add;
        _copy_wstr(exe_cmd.strParam1, test_study_uid.c_str());
        exe_cmd.intParam = ThLibCmdOutput_Xreal;
        dll_loader._dll_th_lib_exe_cmd(&exe_cmd, &ret_cmd);
        check_cmd_finished(ret_cmd.cmdUID, status, 500);
        //add iPhone
        exe_cmd.cmdID = ThLibCmd_Output_Add;
        exe_cmd.intParam = ThLibCmdOutput_iPhone;
        dll_loader._dll_th_lib_exe_cmd(&exe_cmd, &ret_cmd);
        check_cmd_finished(ret_cmd.cmdUID, status, 500);
        //clear
        exe_cmd.cmdID = ThLibCmd_Output_Remove;
        exe_cmd.intParam = ThLibCmdOutput_Xreal;
        _copy_wstr(exe_cmd.strParam1, test_study_uid.c_str());
        dll_loader._dll_th_lib_exe_cmd(&exe_cmd, &ret_cmd);
        exe_cmd.intParam = ThLibCmdOutput_iPhone;
        dll_loader._dll_th_lib_exe_cmd(&exe_cmd, &ret_cmd);
        check_cmd_finished(ret_cmd.cmdUID, status, 500);
        //add Xreal_iPhone
        exe_cmd.cmdID = ThLibCmd_Output_Add;
        exe_cmd.intParam = ThLibCmdOutput_Xreal_iPhone;
        dll_loader._dll_th_lib_exe_cmd(&exe_cmd, &ret_cmd);
        check_cmd_finished(ret_cmd.cmdUID, status, 500);

    }
    {
        //setup
        ThProcLib_Cmd exe_cmd;
        ThProcLib_CmdOutput ret_cmd;
        exe_cmd.cmdID = ThLibCmd_ROI_Final;
        _copy_wstr(exe_cmd.strParam1, test_study_uid.c_str());
        dll_loader._dll_th_lib_exe_cmd(&exe_cmd, &ret_cmd);
        //
        exe_cmd.cmdID = ThLibCmd_Output_Add;
        _copy_wstr(exe_cmd.strParam1, test_study_uid.c_str());
        exe_cmd.intParam = ThLibCmdOutput_Xreal_iPhone;
        dll_loader._dll_th_lib_exe_cmd(&exe_cmd, &ret_cmd);

        int output_data_size = dll_loader._dll_th_lib_get_data_size(ThLibQueryList_OutputData);
        for (int i = 0; i < output_data_size; i++) {
            ThProcLib_PatientInfo _pat_info;
            int sts = dll_loader._dll_th_lib_get_datalist(&_pat_info, ThLibQueryList_OutputData, i);
            std::wstring name = _pat_info.PatientName;
            std::wstring id = _pat_info.PatientID;
        }
        int xx = 0;
    }
}
void CTestThProcLib::testProcSts(void)
{
   
    ThProcLib_Env outEnv;
    dll_loader._dll_th_lib_get_env(&outEnv);

    int out_size = 0;

    std::wstring test_study_uid;
    int test_study_no = 0;
    int ct_data_size = dll_loader._dll_th_lib_get_data_size(ThLibQueryList_CTData);
    for (int i = 0; i < ct_data_size; i++) {
        ThProcLib_PatientInfo _pat_info;
        int sts = dll_loader._dll_th_lib_get_datalist(&_pat_info, ThLibQueryList_CTData, i);
        std::wstring name = _pat_info.PatientName;
        std::wstring id = _pat_info.PatientID;
        test_study_uid = _pat_info.StudyUID;
        test_study_no = i;
    }


    {//ROI

        ThProcLib_Cmd exe_cmd;
        ThProcLib_CmdOutput ret_cmd;
        exe_cmd.cmdID = ThLibCmd_ROI_Final;
        _copy_wstr(exe_cmd.strParam1, test_study_uid.c_str());
        dll_loader._dll_th_lib_exe_cmd(&exe_cmd, &ret_cmd);
        //
        {//check status
            ThProcLib_PatientInfo _pat_info;
            int sts = dll_loader._dll_th_lib_get_datalist(&_pat_info, ThLibQueryList_CTData, test_study_no);
            std::wstring name = _pat_info.PatientName;
            std::wstring id = _pat_info.PatientID;
            if (_pat_info.ProcessStatus != ThLibCmdProc_Roi) {
                printf("check ProcessStatus error ROI \n");
            };
        }
        exe_cmd.cmdID = ThLibCmd_ROI_Clear;
        _copy_wstr(exe_cmd.strParam1, test_study_uid.c_str());
        dll_loader._dll_th_lib_exe_cmd(&exe_cmd, &ret_cmd);
        {//check status
            ThProcLib_PatientInfo _pat_info;
            int sts = dll_loader._dll_th_lib_get_datalist(&_pat_info, ThLibQueryList_CTData, test_study_no);
            std::wstring name = _pat_info.PatientName;
            std::wstring id = _pat_info.PatientID;
            if (_pat_info.ProcessStatus != ThLibCmdProc_No_Roi) {
                printf("check ProcessStatus error ROI \n");
            };
        }
    }

}
 
void CTestThProcLib::testRender(void)
{
    
    ThProcLib_Cmd exe_cmd;

    {
        exe_cmd.cmdID = ThLibCmd_Check_Image_Server;
        ThProcLib_CmdOutput ret_cmd;
        dll_loader._dll_th_lib_exe_cmd(&exe_cmd, &ret_cmd);
        bool bSts = ret_cmd.intParam == 1;
        int xx = 0;
    }
    DWORD dwStart = GetTickCount();
    DWORD dwStart_vr = GetTickCount();

    int meas_step = 10;
    for (int run_i = 0; run_i < 2; run_i++) {
        {

            DWORD dwEnd = GetTickCount();
            printf("run_i %d [%.1f]\n", run_i, (float)(dwEnd - dwStart));
            dwStart = GetTickCount();
        }

 
        ThProcLib_CmdOutput ret_cmd;
        ThProcLib_CmdStatus status;
        std::wstring studyUID;
        {
            int ct_data_size = dll_loader._dll_th_lib_get_data_size(ThLibQueryList_CTData);
            if (ct_data_size > 0) {

                ThProcLib_PatientInfo _pat_info;
                int sts = dll_loader._dll_th_lib_get_datalist(&_pat_info, ThLibQueryList_CTData, ct_data_size - 1);
                studyUID = _pat_info.StudyUID;
            }
        }
        //       studyUID = L"1.2.392.200069.17.10338133111.1202505010003201";
        studyUID = m_selectStudyUID;
        
        //load volume
        DWORD dwStart_LoadVol = GetTickCount();
       loadVol(studyUID);
        DWORD dwendLoadVol = GetTickCount();
        printf("LoadVol [%.1f]mSec\n", (float)(dwendLoadVol - dwStart_LoadVol));

      
        //
        //load 3DModle
#if 1
        exe_cmd.cmdID = ThLibCmd_Render_Load_3DModel;
        _copy_wstr(exe_cmd.strParam1, studyUID.c_str());
        dll_loader._dll_th_lib_exe_cmd(&exe_cmd, &ret_cmd);
#endif
        {
            
            ThProcLib_RenderGC renderGC;
            createRenderGC(renderGC);

            exe_cmd.cmdID = ThLibCmd_Render_Op;
            //
            renderGC.render_cmd_major = ThLibRenderCmd_ProcMouse;

            renderGC.target_sizeX = 600;
            renderGC.target_sizeY = 512;
            renderGC.slice_position = 50;
            renderGC.zoom = 1.0;
            renderGC.mouse_type = ThLibMouseEvent_ROI_Pen;
            renderGC.mouse_posX = 200;
            renderGC.mouse_posY = 300;
            ThProcLib_Image2DData outImage;
            dll_loader._dll_th_lib_exe_render(&exe_cmd, &renderGC, &outImage);
            int xx = 0;
            releaseRenderGC(renderGC);
        }
        for (int r_i = 0; r_i < 10; r_i++)
        {//r_i
            if (r_i % meas_step == 0) {

                DWORD dwEnd = GetTickCount();
                printf("run_i %d [%.1f]\n", run_i, (float)(dwEnd - dwStart_vr));
                dwStart_vr = GetTickCount();
            }
            bool enable_3D_Polygon = false;
            bool enable_3D_CTMarker = false;
            int e_3D_no = r_i % 3;
            switch (e_3D_no) {
            case 0:
                break;
            case 1:
                enable_3D_Polygon = true;
                break;
            case 2:
                enable_3D_CTMarker = true;
                break;
            }
            enable_3D_Polygon = true;
            enable_3D_CTMarker = false;

            for (int series_no = 0; series_no < 2; series_no++)
            {//series_no
                eSeriesType sereis_type = tbl_series[series_no];

                //get dicom information
                exe_cmd.cmdID = ThLibCmd_Get_DicomImageInfo;
                exe_cmd.intParam = sereis_type;//   ThLibSeriesType_Juukaku = 1,
                _copy_wstr(exe_cmd.strParam1, studyUID.c_str());

                ThProcLib_DicomImageInfo _image_info;
                int sts = dll_loader._dll_th_lib_get_image_info(&exe_cmd, &_image_info);
                int sizeX = _image_info.sizeX;

                //
#if 1
               
                ThProcLib_RenderGC renderGC;
                createRenderGC(renderGC);

                //exchange Haiya/Juukaku
                {
                    exe_cmd.cmdID = ThLibCmd_Render_Op;
                    exe_cmd.intParam = m_VolData.volID;
                    renderGC.render_cmd_major = ThLibRenderCmd_Setup;
                    renderGC.series_type = sereis_type;
                    ThProcLib_Image2DData outImage;
                    dll_loader._dll_th_lib_exe_render(&exe_cmd, &renderGC, &outImage);
                }
                //select dispaly parts
                if (1) {
                    exe_cmd.cmdID = ThLibCmd_Render_Op;
                    exe_cmd.intParam = m_VolData.volID;
#if 0
                    renderGC.display_parts =
                        ThLibDisplayParts_ROI |
                        ThLibDisplayParts_Lung |
                        ThLibDisplayParts_Heart;
#else
                    renderGC.display_parts = ThLibDisplayParts_Other;
#endif

                    ThProcLib_Image2DData outImage;
                    dll_loader._dll_th_lib_exe_render(&exe_cmd, &renderGC, &outImage);
                }
                //render-2D
                {
                    DWORD rv_time_start = GetTickCount();
                    exe_cmd.cmdID = ThLibCmd_Render_Vol3D;
                    exe_cmd.intParam = m_VolData.volID;
                    renderGC.render_cmd_major = ThLibRenderCmd_2D;
                    renderGC.series_type = sereis_type;
                    renderGC.target_sizeX = 600;
                    renderGC.target_sizeY = 512;
                    renderGC.slice_position = (10 * r_i + 1) % _image_info.sizeZ;// (r_i + _image_info.sizeZ / 2) % _image_info.sizeZ;// -122;
                    renderGC.window_levle = _image_info.WindowCenter;
                    renderGC.window_width = _image_info.WindowWidth;
                    renderGC.zoom = 1.0;
                    // renderGC.slice_position = 7;
                     //
                    renderGC.display_mode = ThLibDisplayMode_Volume;
                    if (enable_3D_Polygon) {
                        renderGC.display_mode |= ThLibDisplayMode_Polygon;
                    }
                    if (enable_3D_CTMarker) {
                        renderGC.display_mode |= ThLibDisplayMode_CTMarker;
                    }
                    //
                    if (!render(renderGC)) return;
                     ;
                    DWORD rv_time_end = GetTickCount();
                    printf("rv_time [%.2f]\n", (float)(rv_time_end - rv_time_start));
                    ///
                    {
                        std::wstring file_str = L"dbg.img_2D";
                        if (enable_3D_Polygon) {
                            file_str += L"_Ply_";
                        }
                        if (enable_3D_CTMarker) {
                            file_str += L"_CTMk_";
                        }
                        file_str += (series_no == ThLibSeriesType_Haiya) ? L".ha" : L".ju";
                        saveImage(file_str);
                    }
                    //release Image2D
                    releaseOutImage();
                    releaseRenderGC(renderGC);
                }
                //render-3D
                {
                    {//rotation
                        exe_cmd.cmdID = ThLibCmd_Render_Op;
                        exe_cmd.intParam = m_VolData.volID;
                        //
                        renderGC.render_cmd_major = ThLibRenderCmd_ProcMouse;
                        renderGC.mouse_type = ThLibMouseEvent_LBUTTONDOWN;
                        renderGC.mouse_posX = 50;
                        renderGC.mouse_posY = 50;
                        ThProcLib_Image2DData outImage_dummy;
                        dll_loader._dll_th_lib_exe_render(&exe_cmd, &renderGC, &outImage_dummy);
                        //
                        if (0) {//rotation
                            for (int rot_i = 0; rot_i < 15; rot_i++) {

                                renderGC.render_cmd_major = ThLibRenderCmd_ProcMouse;
                                if (rot_i == 0) {
                                    renderGC.mouse_type = ThLibMouseEvent_LBUTTONDOWN;
                                }
                                else {
                                    renderGC.mouse_type = ThLibMouseEvent_3D_Rotation;
                                }
                                renderGC.mouse_posX = 55 + rot_i * 10;
                                renderGC.mouse_posY = 50;
                                ThProcLib_Image2DData outImage_dummy;
                                dll_loader._dll_th_lib_exe_render(&exe_cmd, &renderGC, &outImage_dummy);

                            }
                        }
                    }
                    exe_cmd.cmdID = ThLibCmd_Render_Vol3D;
                    exe_cmd.intParam = m_VolData.volID;
                    //
                    renderGC.render_cmd_major = ThLibRenderCmd_3D;
                    renderGC.series_type = sereis_type;
                    renderGC.target_sizeX = 600;
                    renderGC.target_sizeY = 512;
                    //      renderGC.slice_position = (run_i + _image_info.sizeZ / 2) % _image_info.sizeZ;// -122;
                    //      renderGC.window_levle = _image_info.WindowCenter;
                     //     renderGC.window_width = _image_info.WindowWidth;

                    renderGC.display_mode = ThLibDisplayMode_Volume;
                    if (enable_3D_Polygon) {
                        renderGC.display_mode |= ThLibDisplayMode_Polygon;
                    }
                    if (enable_3D_CTMarker) {
                        renderGC.display_mode |= ThLibDisplayMode_CTMarker;
                    }
                    renderGC.zoom = 1.0;
                    if (!render(renderGC)) return;
                    ///
                    {
                        std::wstring file_str = L"dbg.img_3D";
                        if (enable_3D_Polygon) {
                            file_str += L"_Ply_";
                        }
                        if (enable_3D_CTMarker) {
                            file_str += L"_CTMk_";
                        }
                        file_str += (series_no == ThLibSeriesType_Haiya) ? L".ha" : L".ju";
                        saveImage(file_str);
                    }
                    //release Image2D
                    releaseOutImage();
                }
#endif


                releaseRenderGC(renderGC);
            }//series_no
        }//r_i

        releaseVol();

    }
}
