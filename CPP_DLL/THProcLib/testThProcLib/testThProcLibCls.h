#pragma once
 

#include "ThProcLibDllLoader.h"
 
#include "windows.h"
#include <string>
#include <vector>

#define _copy_wstr( dest_buff,src_str)  wcscpy_s((dest_buff),sizeof(dest_buff)/sizeof(wchar_t),(src_str));
#define _copy_str( dest_buff,src_str)  strncpy_s((dest_buff),sizeof(dest_buff) ,(src_str),sizeof(dest_buff));

class CTestThProcLib
{
public:
    CTestThProcLib();
    virtual ~CTestThProcLib();
    bool initDll(void);

    void testProc(void);
    void testProc2(void);
    void testProcSts(void);
    void testRender(void);
    void testRenderCnt(void);
    void testCTMarker(void);
    void testRoiMarker(void);
    void setupStudyUID(const std::wstring& uid) {
        m_selectStudyUID = uid;
    }
protected:
    bool loadVol(const std::wstring& uid);
    bool createRenderGC(ThProcLib_RenderGC& renderGC);
    bool render(ThProcLib_RenderGC& renderGC);
    void releaseRenderGC(ThProcLib_RenderGC& renderGC);
    void releaseOutImage(void);
    void releaseVol(void);
    void saveImage(const std::wstring fileName);
    std::wstring m_selectStudyUID;
    eSeriesType tbl_series[2];
    CThProcLibDllLoader dll_loader;
    ThProcLib_Vol3DData m_VolData;
    std::vector< unsigned int> aImageBuffer;
    ThProcLib_Image2DData m_RenderOutImage;
    void check_cmd_finished(int cmdUID, ThProcLib_CmdStatus& status, int wait_inter = 5000);

    bool m_bInitFlag;
};

  
 
