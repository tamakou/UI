// testThItkLib.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "math.h"
 

#include "testThProcLibCls.h"

void CTestThProcLib::testRenderCnt(void)
{
   
    ThProcLib_Cmd exe_cmd;
    DWORD dwStart = GetTickCount(); 
    DWORD dwStart_vr = GetTickCount();

     
    int meas_step = 10;
    int run_num = 10000;
    try {
        for (int run_i = 0; run_i < run_num; run_i++) {
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
            studyUID = m_selectStudyUID;
            loadVol(studyUID);
             
            //
            //load 3DModle
            exe_cmd.cmdID = ThLibCmd_Render_Load_3DModel;
            _copy_wstr(exe_cmd.strParam1, studyUID.c_str());
            dll_loader._dll_th_lib_exe_cmd(&exe_cmd, &ret_cmd);

            {
               
                ThProcLib_RenderGC renderGC;
                createRenderGC(renderGC);
                //
                renderGC.render_cmd_major = ThLibRenderCmd_ProcMouse;

                renderGC.target_sizeX = 600;
                renderGC.target_sizeY = 512;
                renderGC.slice_position = 100;
                renderGC.zoom = 1.0;
                renderGC.mouse_type = ThLibMouseEvent_ROI_Pen;
                renderGC.mouse_posX = 200;
                renderGC.mouse_posY = 300;
                ThProcLib_Image2DData outImage;
                dll_loader._dll_th_lib_exe_render(&exe_cmd, &renderGC, &outImage);
            }
   
            int rv_run_num =  10000;
            for (int r_i = 0; r_i < rv_run_num; r_i++)
               
            {//r_i
                if (r_i % meas_step == 0) {

                    DWORD dwEnd = GetTickCount();
                    float spent_rv_time = (float)(dwEnd - dwStart_vr);
                    printf("r_i %d [%.1f]\n", r_i, spent_rv_time);
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
                    {
                        exe_cmd.cmdID = ThLibCmd_Render_Op;
                        exe_cmd.intParam = m_VolData.volID;
                        renderGC.display_parts =
                            ThLibDisplayParts_ROI |
                            ThLibDisplayParts_Lung |
                            ThLibDisplayParts_Heart;

                        ThProcLib_Image2DData outImage;
                        dll_loader._dll_th_lib_exe_render(&exe_cmd, &renderGC, &outImage);
                    }
                    //render-2D
                    if(1){
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
#if 1
                        if (enable_3D_Polygon) {
                            renderGC.display_mode |= ThLibDisplayMode_Polygon;
                        }
                        if (enable_3D_CTMarker) {
                            renderGC.display_mode |= ThLibDisplayMode_CTMarker;
                        }
#endif
                        //
                         
                        if (!render(renderGC)) return;

                         
                        DWORD rv_time_end = GetTickCount();
                        float spent_rv_time = (float)(rv_time_end - rv_time_start);
                        printf("rv_time(2D) [%.2f]\n", spent_rv_time);
                        if (spent_rv_time > ((r_i < 5) ? 2000 : 500)) {
                            printf("render 2D error\n");
                            throw(-1);//to stop all
                        }
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

#if 1
                        renderGC.display_mode = ThLibDisplayMode_Volume;
                        if (enable_3D_Polygon) {
                            renderGC.display_mode |= ThLibDisplayMode_Polygon;
                        }
                        if (enable_3D_CTMarker) {
                            renderGC.display_mode |= ThLibDisplayMode_CTMarker;
                        }
#endif
                        renderGC.zoom = 2.5;
                         
                        DWORD rv_3D_time_start = GetTickCount(); 
                        if (!render(renderGC)) return;
                         
                        DWORD rv_3D_time_end = GetTickCount();
                        float spent_3D_time = (float)(rv_3D_time_end - rv_3D_time_start);
                        printf("rv_time(3D) [%.2f]\n", spent_3D_time);
                        if (spent_3D_time > ((r_i < 5) ? 2000 : 500)) {
                            printf("render 3D error\n");
                            throw(-1);
                        }
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
                        releaseRenderGC(renderGC);
                    }
#endif


                    releaseRenderGC(renderGC);
                }//series_no
            }//r_i

            releaseVol();

        }
    }
    catch (...) {
        printf("stop all \n");
    }
}
