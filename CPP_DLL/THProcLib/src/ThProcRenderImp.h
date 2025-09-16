#ifndef TH_PROC_LIB_RENDER_IMP_H
#define TH_PROC_LIB_RENDER_IMP_H

#pragma warning (disable: 4091)
#pragma warning (disable: 4244)

#include "ThProcRender.h"
#include "../../ViewerCore/include/PxSMViewerIf.h"
#include "../../ViewerCore/include/PxSMViewerSmart.h"
#include "../../ThPCCoreLib/include/IThPCCoreLib.h"

#include "MultiThExe.h"
#include <vector>
#include <map>
#include <string>
#include <memory>
#include "ThProcLibLogger.h"

//#195_using_Grp_for_Undo_find_nearest_Grp
struct GenericObjBallInfo
{
	double	m_Center[3];
	double	m_DistR2;
	int		m_ObjID;
	int		m_ObjStyp;
};
//#127_shared_Haiya_Juukaku
class CThProcRenderGrpParam
{
public:
	//#202_added_point_time_stamp
	struct ROI_Point_with_time
	{
		std::vector<double> m_p;
		unsigned long m_time_stamp;
	};
	CThProcRenderGrpParam();
	virtual ~CThProcRenderGrpParam();
	std::vector<GenericObjBallInfo> m_ROI_Cur_ObjIDs;//while LButtonDown
	std::vector<std::vector<GenericObjBallInfo>> m_ROI_Grp_ObjIDs;//#195_using_Grp_for_Undo 
	std::vector<int> m_CT_Marker_ObjIDs;
	std::vector<std::vector<double>> m_CT_Marker_P3D_in_Voxel;
	std::vector<ROI_Point_with_time> m_ROI_Points;//#161_display_thoracentes_needle
	bool m_bRoiClear;//#175_clear_exist_ROI_Marker
	//
	bool	m_bLButtonDown;
	int *	m_pRoi_Pen_Status_shared;
	double	CurWorldP3D[3];
	int		m_penWidth;
};
class CThProcRenderImp : public CThProcRender,public CThProcRenderPair
{
public:
	struct RenderImgInfo {//#199_panXY
			RenderImgInfo();
			short			m_valid;
			short			m_width;
			short			m_height;
			short			m_bitsPerPixel;
			short			m_recommendedWindowLevel;
			unsigned short	m_recommendedWindowWidth;
			float			m_thickness;
			short			m_sliceNumber;
			float			m_xScale;//#32 2020/05/28
			float			m_yScale;
	};
	static void setupSMAPILibLog(const std::string& folder, int level);
	CThProcRenderImp(ThPCCoreLib::ThPCCoreLibIf* p, const eSeriesType type, CThProcRenderGrpParam *GrpP);
	virtual ~CThProcRenderImp();
	void setupLogger(const CThProcLogger* logger) override {
		m_logger = logger;
	}
	bool init(void) override;
	bool checkClientNvr(void) override;//#137_check_image_server
	//#127_shared_Haiya_Juukaku
	bool setupShareClientNvrAsMaster(CThProcRender* other) override;
	void setupPairing(CThProcRenderPair* p)  override
	{
		m_render_pair = p;
	};
	bool loadVol(const std::string& study_uid, const std::string& series_uid) override;
	void clearExeStatus(void) override
	{//#203_clear_exe_status
		m_exe_status = false;
	}
	//#100_load_3DModel_and_show_3DModel
	bool load3DModel(const std::string& study_uid, const std::string& series_uid,eModelName model = eModel_all) override;
	bool renderAxial(const ThProcLib_RenderGC* param) override;
	bool render3D(const ThProcLib_RenderGC* param) override;
	//#112_Rotatio_3D
	void procMouseEvent(const ThProcLib_RenderGC* param, int& next_render_cmds) override;
	//#115_added_Polygon
	void procMouseEvent_AddPolygon(const ThProcLib_RenderGC* param) override;
	void procMouseEvent_ErasePolygon(const ThProcLib_RenderGC* param) override;
	void saveRoiData(const ThProcLib_RenderGC* param) override;
	void clearRoiData(const ThProcLib_RenderGC* param) override;
	//#195_check_LButtonUp_for_ROI_added
	void removeAllRoiObjs(void);
	void AddGrpRoiObjs(void); //#195_using_Grp_for_Undo 
	void UndoRoiObjs(void); //#195_using_Grp_for_Undo 
	bool saveCTMarker(void) override;
	void clearCTMarker(void) override;
	void setpPenWidth(int w) override {
		m_renderGrpParam->m_penWidth = w;
	}
	const unsigned char* getImageData(int& RGBA_size) const override;
	bool getProgress(int& completed, float& progress) const override;
	const ThProcLib_Vol3DData* getVol3DData(void) const override;
	bool copyOutputImage(unsigned char* BGRA_DataBuffer, int BufferSize) const override;
	bool get_dicom_info(const std::string& study_uid, ThProcLib_DicomImageInfo* image_info) const override;
	void setupDisplay3Dmodel(int DispParts) override {
		m_3DModelDisplayParts = DispParts;
	}
	void setup3DModelAlpha(int alpha) {
		m_3DModelAlpha = alpha;
	}
	std::string getOtherSession(void);
	//implementation
	void pair_RoatationVR(int PointDeltaX, int PointDeltaY) override;
	bool pair_show3DModel(bool OnOff, int model) override;
	void pair_onLButtonUp(void) override;
	bool pair_addROIData(double p3D[3]) override;
	//#169_delete_CT-Marker
	void pair_enableModel(int model,bool bShow) override;
	void setupNeedleDisplay(int	 needle_On, int needle_L, int needle_R, int needle_ext_line = 0) override
	{//#161_display_thoracentes_needle
		m_needle_OnOff = needle_On;
		m_needle_L = needle_L;
		m_needle_D = needle_R;
		m_needle_ext_L = needle_ext_line;
	}
	//#169_delete_CT-Marker
	void enableModel(eModelName model, bool bShow) override;
	void setRotation3DXYDeg(float deg) {
		m_Rotation3DXYDeg = deg;
	}
	
protected:
	void backupRenderImgInfo(RenderImgInfo& dest, const  PxVCoreAPI::PXSMVImage* srcImg);//#199_panXY
	float m_adj_ct_marker_thicness_half;
	void clearCurRoiPos(void);//#161_display_thoracentes_needle
	bool show3DModel( bool OnOff, eModelName model = eModel_all);
	bool show3DModel_in(bool OnOff, eModelName model = eModel_all);
	//#169_delete_CT-Marker
	bool isHidedModel(eModelName model) const;
	//#195_check_LButtonUp_for_ROI_added
	void onLButtonUp(void);
	void onLButtonUp_in(void);
	bool pickupWorldPoint(const ThProcLib_RenderGC* param,double *p3D );
	bool addROIData(double p3D[3]);
	bool addROIData_in(double p3D[3]);
//#125_insert_CT_Marker
	void setupRenderAxial(const ThProcLib_RenderGC* GC_param, PxVCoreAPI::PXSMVRenderParam *render_param,bool bForCoord=false);
	void insertCTMarkerOn3D(const ThProcLib_RenderGC* param);
	void insertCTMarkerOn2D(const ThProcLib_RenderGC* param);//#176_insert_CT_Marker_on_2D
	void setup3DLut(int lut_no);
	void setup3DModelDisplay(const ThProcLib_RenderGC* param);
	void get3DModelFiles(bool bCheckShow,std::vector<std::wstring>& fileList, eModelName model = eModel_all);
	SmartPtr_ClientNvr m_ClientNvr;
	SmartPtr_PXSMVVolumeData m_VolData;
	SmartPtr_PXSMVRenderParam m_renderParam2D;
	SmartPtr_PXSMVRenderParam m_renderParam3D;
	SmartPtr_PXSMVImage m_renderOutputImg;
	//#199_panXY
	RenderImgInfo m_renderImgRef2D;
	RenderImgInfo m_renderImgRef3D;
	ThProcLib_Vol3DData m_Vol3DData;
	bool m_exe_status;
	const eSeriesType m_series_type;
	int m_3DModelDisplayParts;
	int m_3DModelAlpha;

	//#112_Rotatio_3D
	int m_mouseDownPos[2];
	int m_mouseCurPos[2];
	//#161_display_thoracentes_needle
	double m_CurRoiPos[3];
	unsigned long m_CurRoiPosTime;
	//
	ThPCCoreLib::ThPCCoreLibIf* m_pPcCoreLib_ref;
	//
	ThProcLib_RenderGC m_cur3DRenderGC;//#125_insert_CT_Marker
	ThProcLib_RenderGC m_curAxialRenderGC;//#125_insert_CT_Marker,//#176_insert_CT_Marker_on_2D
	
	//#127_shared_Haiya_Juukaku
	CThProcRenderGrpParam* m_renderGrpParam;
	CThProcRenderPair *m_render_pair;
	std::string m_shared_sessionID_of_Master;
	//
	const CThProcLogger *m_logger;
	//
	float m_Rotation3DXYDeg;
	//
	int		m_needle_OnOff;
	int		m_needle_L;
	int		m_needle_D;
	int		m_needle_ext_L;
	//#169_delete_CT-Marker
	std::map<eModelName,int> m_hideModels;
	//
	PxVCoreAPI::PXSMVDICOMData m_CurSeriesData;

};
#define ClearExistObjMask (0x100)
 
#endif //TH_PROC_LIB_RENDER_IMP_H