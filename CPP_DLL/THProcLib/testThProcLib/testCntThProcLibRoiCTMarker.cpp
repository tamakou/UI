// testThItkLib.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "math.h"
 
#include "testThProcLibCls.h"
 
 
void CTestThProcLib::testRoiMarker(void)
{
    ThProcLib_Cmd exe_cmd;
    DWORD dwStart = GetTickCount();
    DWORD dwStart_vr = GetTickCount();


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
    studyUID = m_selectStudyUID;
    loadVol(studyUID);

    int outputX = 600;
    int outputY = 512;

    ThProcLib_RenderGC renderGC;
     
    createRenderGC(renderGC);
    {

         
        //render-3D firt for intert points
        {

            exe_cmd.cmdID = ThLibCmd_Render_Vol3D;
            exe_cmd.intParam = m_VolData.volID;
            //
            renderGC.render_cmd_major = ThLibRenderCmd_3D;
            renderGC.series_type = ThLibSeriesType_Haiya;
            renderGC.target_sizeX = 600;
            renderGC.target_sizeY = 512;

            renderGC.zoom = 1.0;
            DWORD rv_3D_time_start = GetTickCount();
            if (!render(renderGC)) return ;

            saveImage(L"dbg.img_3D.");
            
            //release Image2D
            releaseOutImage();
        }

        //intert points
        exe_cmd.cmdID = ThLibCmd_Render_Op;
        exe_cmd.intParam = m_VolData.volID;
#if 1
        renderGC.series_type = ThLibSeriesType_Haiya;
        renderGC.render_cmd_major = ThLibRenderCmd_ProcMouse;
        renderGC.mouse_type = ThLibMouseEvent_CT_Marker_3D;
        renderGC.target_sizeX = outputX;
        renderGC.target_sizeY = outputY;
        renderGC.mouse_posX = 307;
        renderGC.mouse_posY = 128;
        ThProcLib_Image2DData outImage;
        dll_loader._dll_th_lib_exe_render(&exe_cmd, &renderGC, &outImage);
#endif
        //render-3D with polygon
        {

            //select dispaly parts
            {
                exe_cmd.cmdID = ThLibCmd_Render_Op;
                exe_cmd.intParam = m_VolData.volID;
                renderGC.render_cmd_major = ThLibRenderCmd_Setup;
                renderGC.display_parts =
                    ThLibDisplayParts_ROI |
                    ThLibDisplayParts_Lung |
                    ThLibDisplayParts_Heart;

                ThProcLib_Image2DData outImage;
                dll_loader._dll_th_lib_exe_render(&exe_cmd, &renderGC, &outImage);
            }
             

            exe_cmd.cmdID = ThLibCmd_Render_Vol3D;
            exe_cmd.intParam = m_VolData.volID;
            //
            renderGC.render_cmd_major = ThLibRenderCmd_3D;
            renderGC.series_type = ThLibSeriesType_Haiya;
            renderGC.target_sizeX = outputX;
            renderGC.target_sizeY = outputY;
            renderGC.display_mode |= ThLibDisplayMode_Polygon;
            renderGC.zoom = 1.0;
             
            DWORD rv_3D_time_start = GetTickCount();
            if (!render(renderGC)) return;

            saveImage(L"dbg.img_3D.ply.");

            //release Image2D
            releaseOutImage();

        }
    }
    releaseRenderGC(renderGC);
    releaseVol();
     
}
void CTestThProcLib::testCTMarker(void)
{
    ThProcLib_Cmd exe_cmd;
    DWORD dwStart = GetTickCount();
    DWORD dwStart_vr = GetTickCount();

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
    studyUID = m_selectStudyUID;
    //load volume
    loadVol(studyUID);

    int outputX = 600;
    int outputY = 512;

    ThProcLib_RenderGC renderGC;
    createRenderGC(renderGC);

    {

        {//CT-Marker
            ThProcLib_Cmd exe_cmd;
            // ThProcLib_CmdOutput ret_cmd;
            ThProcLib_Image2DData outImage;
            //
            renderGC.render_cmd_major = ThLibRenderCmd_Setup;
            exe_cmd.cmdID = ThLibCmd_CT_Marker_Clear;
            //     _copy_wstr(exe_cmd.strParam1, test_study_uid.c_str());
            exe_cmd.intParam = m_VolData.volID;
            //dll_loader._dll_th_lib_exe_cmd(&exe_cmd, &ret_cmd);
            dll_loader._dll_th_lib_exe_render(&exe_cmd, &renderGC, &outImage);
            //
            exe_cmd.cmdID = ThLibCmd_CT_Marker_Final;
            //  _copy_wstr(exe_cmd.strParam1, test_study_uid.c_str());
            exe_cmd.intParam = m_VolData.volID;
            //dll_loader._dll_th_lib_exe_cmd(&exe_cmd, &ret_cmd);
            dll_loader._dll_th_lib_exe_render(&exe_cmd, &renderGC, &outImage);
        }
        //render-3D
        {

            exe_cmd.cmdID = ThLibCmd_Render_Vol3D;
            exe_cmd.intParam = m_VolData.volID;
            //
            renderGC.render_cmd_major = ThLibRenderCmd_3D;
            renderGC.series_type = ThLibSeriesType_Haiya;
            renderGC.target_sizeX = 600;
            renderGC.target_sizeY = 512;

            renderGC.zoom = 1.0;
            
            DWORD rv_3D_time_start = GetTickCount();

            if (!render(renderGC)) return;

            saveImage(L"dbg.img_3D.");
            //release Image2D
            releaseOutImage();
            //renderGC.display_mode |= ThLibDisplayMode_Polygon;
        }

        //intert points
        exe_cmd.cmdID = ThLibCmd_Render_Op;
        exe_cmd.intParam = m_VolData.volID;
#if 1
        renderGC.series_type = ThLibSeriesType_Haiya;
        renderGC.render_cmd_major = ThLibRenderCmd_ProcMouse;
        renderGC.mouse_type = ThLibMouseEvent_CT_Marker_3D;
        renderGC.target_sizeX = outputX;
        renderGC.target_sizeY = outputY;
        renderGC.mouse_posX = 307;
        renderGC.mouse_posY = 128;
        ThProcLib_Image2DData outImage;
        dll_loader._dll_th_lib_exe_render(&exe_cmd, &renderGC, &outImage);
#endif
        //render-3D with polygon
        {

            //select dispaly parts
            {
                exe_cmd.cmdID = ThLibCmd_Render_Op;
                exe_cmd.intParam = m_VolData.volID;
                renderGC.render_cmd_major = ThLibRenderCmd_Setup;
                renderGC.display_parts =
                    ThLibDisplayParts_ROI |
                    ThLibDisplayParts_Lung |
                    ThLibDisplayParts_Heart;

                ThProcLib_Image2DData outImage;
                dll_loader._dll_th_lib_exe_render(&exe_cmd, &renderGC, &outImage);
            }
            {//rotation
                for (int rot_i = 0; rot_i < 15; rot_i++) {

                    renderGC.render_cmd_major = ThLibRenderCmd_ProcMouse;
                    if (rot_i == 0) {
                        renderGC.mouse_type = ThLibMouseEvent_LBUTTONDOWN;
                    }
                    else {
                        renderGC.mouse_type = ThLibMouseEvent_3D_Rotation;
                    }
                    renderGC.mouse_posX = 55 + rot_i * 30;
                    renderGC.mouse_posY = 50;
                    ThProcLib_Image2DData outImage_dummy;
                    dll_loader._dll_th_lib_exe_render(&exe_cmd, &renderGC, &outImage_dummy);

                }
            }

            exe_cmd.cmdID = ThLibCmd_Render_Vol3D;
            exe_cmd.intParam = m_VolData.volID;
            //
            renderGC.render_cmd_major = ThLibRenderCmd_3D;
            renderGC.series_type = ThLibSeriesType_Haiya;
            renderGC.target_sizeX = outputX;
            renderGC.target_sizeY = outputY;
            renderGC.display_mode |= ThLibDisplayMode_Polygon;
            renderGC.zoom = 1.0;
             
            DWORD rv_3D_time_start = GetTickCount();
            if (!render(renderGC)) return;

            saveImage(L"dbg.img_3D.ply.");
            //release Image2D
            releaseOutImage();

        }
    }
    releaseRenderGC(renderGC);
    releaseVol();

}
