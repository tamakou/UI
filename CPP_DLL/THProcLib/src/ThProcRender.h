#ifndef TH_PROC_LIB_RENDER_H
#define TH_PROC_LIB_RENDER_H
 
#include "../include/IThProcLibData.h"

#include "MultiThExe.h"
#include <vector>
#include <map>
#include <string>
#include <memory>

#define def_vein L"vein" 							//ê√ñ¨
#define def_artery L"artery"							//ìÆñ¨
#define def_heart L"heart"			
#define def_liver L"liver" 			
#define def_lung L"lung"
#define def_lung_seg L"lung_seg"
#define def_lung_vessels L"lung_vessels"
#define def_lung_substraction L"lung_substraction"
#define def_rib L"rib"								//ò]çú
#define def_vertebrae L"vertebrae"					//ê“í≈
#define def_clavicula L"clavicula"					//çΩçú
#define def_costal_cartilages L"costal_cartilages"	//ò]ìÓçú
#define def_sternum_scapula L"sternum_scapula"  
#define def_stomach L"stomach" 
#define def_spinal_cord L"spinal_cord"				//ê“êë
#define def_body L"body" 
#define def_ct_marker L"ct_marker"
#define def_roi_marker L"roi_marker"

#define add_ext_stl(model_name) (std::wstring(model_name)+L".stl")

class CThProcLogger;
class CThProcRenderPair {//#127_shared_Haiya_Juukaku
public:
	CThProcRenderPair() {};
	virtual void pair_RoatationVR(int PointDeltaX, int PointDeltaY) = 0;
	virtual bool pair_show3DModel(bool OnOff, int model) = 0;
	//#169_delete_CT-Marker
	virtual void pair_enableModel(int model, bool bShow) = 0;
	//#195_check_LButtonUp_for_ROI_added
	virtual void pair_onLButtonUp(void) = 0;
	virtual bool pair_addROIData(double p3D[3]) = 0;
	 
};
class CThProcRender
{
public:
	enum eModelName {
		eModel_unknown,
//		eModel_adrenal_gland,	//ïõêt
//		eModel_autochthon,
		eModel_vein,				//ê√ñ¨
		eModel_artery,			//ìÆñ¨
		eModel_heart,
		eModel_esophagus,		//êHìπ
//		eModel_gallbladder,		//í_îX
//		eModel_kidney,			//êtëü
		eModel_liver,
		eModel_lung,
		eModel_lung_seg,
		eModel_lung_vessels,
		eModel_lung_substraction,
		eModel_rib,				//ò]çú
		eModel_vertebrae,		//ê“í≈
		eModel_clavicula,		//çΩçú
		eModel_costal_cartilages,//ò]ìÓçú
 		
		eModel_sternum_scapula,		//ãπçú//å®çbçú
//		eModel_duodenum,			//è\ìÒéwí∞
//		eModel_iliopsoas,		//í∞çòãÿ
//		eModel_pancreas,			//‰Xëü
//		eModel_spleen,			//‰Bëü
//		eModel_thyroid_gland,	//çbèÛëB
		eModel_stomach,
//		eModel_trachea,			//ãCä«
//		eModel_small_bowel,		//è¨í∞
		eModel_spinal_cord,		//ê“êë
		eModel_body,
		eModel_roi_marker,		//ROI ->lung_substraction
		eModel_ct_marker,		//CTÉ}Å[ÉJÅ[Çåüèoóp
		eModel_all,
	};
	
	CThProcRender() {};
	virtual ~CThProcRender() {};
	virtual bool init(void) = 0;
	virtual bool checkClientNvr(void) = 0;//#137_check_image_server
	virtual bool setupShareClientNvrAsMaster(CThProcRender *other) = 0;
	virtual void setupPairing(CThProcRenderPair *p) = 0;
	virtual void setupLogger(const CThProcLogger* logger) = 0;
	virtual void clearExeStatus(void) = 0;//#203_clear_exe_status
	virtual bool loadVol(const std::string& study_uid, const std::string& series_uid) = 0;
	//#100_load_3DModel_and_show_3DModel
	virtual bool load3DModel(const std::string& study_uid, const std::string& series_uid, eModelName model = eModel_all) = 0;
	//virtual bool show3DModel(const std::string& study_uid, const std::string& series_uid, bool OnOff, eModelName model = eModel_all) = 0;
	virtual bool renderAxial(const ThProcLib_RenderGC* param) = 0;
	virtual bool render3D(const ThProcLib_RenderGC* param) = 0;
	//#112_Rotatio_3D
	virtual void procMouseEvent(const ThProcLib_RenderGC* param, int& next_render_cmds) = 0;
	//#115_added_Polygon
	virtual void procMouseEvent_AddPolygon(const ThProcLib_RenderGC* param) = 0;
	virtual void procMouseEvent_ErasePolygon(const ThProcLib_RenderGC* param) = 0;
	virtual void saveRoiData(const ThProcLib_RenderGC* param) = 0;
	virtual void clearRoiData(const ThProcLib_RenderGC* param) = 0;
	virtual bool saveCTMarker(void) = 0;
	virtual void clearCTMarker(void) = 0;
	virtual void setpPenWidth(int w) = 0;
	virtual const unsigned char* getImageData(int &RGBA_size) const = 0;
	virtual bool getProgress( int& completed, float& progress) const = 0;
	virtual const ThProcLib_Vol3DData* getVol3DData(void) const = 0;
	virtual bool copyOutputImage(unsigned char* BGRA_DataBuffer, int BufferSize) const = 0;
	virtual bool get_dicom_info(const std::string& study_uid, ThProcLib_DicomImageInfo* image_info) const = 0;
	virtual void setupDisplay3Dmodel(int DispParts) = 0;
	//#161_display_thoracentes_needle
	virtual void setupNeedleDisplay(int	 needle_On,int needle_L,int needle_R,int needle_ext_line=0) = 0;
	virtual void enableModel(eModelName model, bool bShow) = 0;
protected:
	 
};

class RenderImg
{
public:
	RenderImg(CThProcRender *render,const ThProcLib_Image2DData* img_info) :
		m_render(render){
		m_p_imge_info = *img_info;
	}
	ThProcLib_Image2DData m_p_imge_info;
	CThProcRender* m_render;
};
 
#endif //TH_PROC_LIB_RENDER_H