 
#include "ThProcLibMain.h"
#include "ThProcRenderImp.h"
#include "ThStringUtils.h"
#include "MultiThExe.h"
#include "windows.h"
#include "time.h"
#include <map>
#include <list>
#include <memory>
#include <filesystem>
namespace fs = std::filesystem;

CThProcRenderGrpParam::CThProcRenderGrpParam():
	m_bRoiClear(false)//#175_clear_exist_ROI_Marker
	, m_bLButtonDown(false)
	, m_pRoi_Pen_Status_shared(nullptr)
	, m_penWidth(1)
{

}
CThProcRenderGrpParam::~CThProcRenderGrpParam()
{

}
 
void CThProcRenderImp::setupSMAPILibLog(const std::string& folder, int level)
{
	PxVCoreAPI::CPXSMAPILib::setupLogFolder(folder.c_str(), level);
}
//#199_panXY
CThProcRenderImp::RenderImgInfo::RenderImgInfo()
{
	memset(this, 0, sizeof(CThProcRenderImp::RenderImgInfo));
}
void CThProcRenderImp::backupRenderImgInfo(CThProcRenderImp::RenderImgInfo& dest, const  PxVCoreAPI::PXSMVImage* srcImg)
{
	dest.m_valid = 1;
	dest.m_width					= srcImg->m_width;
	dest.m_height					= srcImg->m_height;
	dest.m_bitsPerPixel				= srcImg->m_bitsPerPixel;
	dest.m_recommendedWindowLevel	= srcImg->m_recommendedWindowLevel;
	dest.m_recommendedWindowLevel	= srcImg->m_recommendedWindowLevel;
	dest.m_thickness				= srcImg->m_thickness;
	dest.m_sliceNumber				= srcImg->m_sliceNumber;
	dest.m_xScale					= srcImg->m_xScale;
	dest.m_yScale					= srcImg->m_yScale;
}
CThProcRenderImp::CThProcRenderImp(ThPCCoreLib::ThPCCoreLibIf* p, const eSeriesType type, CThProcRenderGrpParam* GrpP):
	m_series_type(type)
	,m_3DModelDisplayParts(ThLibDisplayParts_Lung|ThLibDisplayParts_Heart)//#163_default
	,m_3DModelAlpha(0)
	, m_logger(nullptr)
	, m_render_pair(nullptr)
	, m_renderGrpParam(GrpP)
	, m_pPcCoreLib_ref(p)
	, m_Rotation3DXYDeg(0.0f)
{
	m_adj_ct_marker_thicness_half = 2.5; //5.0mm/2
	m_mouseDownPos[0]= m_mouseDownPos[1]=0;
	m_mouseCurPos[0] = m_mouseCurPos[1] = 0;
	//#161_display_thoracentes_needle
	clearCurRoiPos();
	m_needle_OnOff = 1;
	m_needle_L = 300;
	m_needle_D = 10;
	m_needle_ext_L = 100;;
}
CThProcRenderImp::~CThProcRenderImp()
{

}
bool CThProcRenderImp::init(void)
{
	m_ClientNvr.reset();
	PxVCoreAPI::ClientNvrIf* pclinetNvr = PxVCoreAPI::CPXSMAPILib::createClientNvrInterface();
	//#127_shared_Haiya_Juukaku
	const char* shared_session_id = nullptr;
	if (!m_shared_sessionID_of_Master.empty()) {
		shared_session_id = m_shared_sessionID_of_Master.c_str();
	}
	bool bSts = pclinetNvr->ConnectToServer("localhost", "scan", "scan", shared_session_id);
	if (!bSts) return false;
	m_ClientNvr.reset(pclinetNvr);
	{
		PxVCoreAPI::PXSMVDICOMInformation inP;
		memset(&inP, 0, sizeof(PxVCoreAPI::PXSMVDICOMInformation));

		sprintf_s(inP.m_patientName, sizeof(inP.m_patientName), "**");
		PxVCoreAPI::PxSMVDataList<PxVCoreAPI::PXSMVDICOMInformation>* outlist;
		bSts = pclinetNvr->SearchPatient(&inP, outlist);
	}
	return true;
}
//#137_check_image_server
bool CThProcRenderImp::checkClientNvr(void)
{
	return m_ClientNvr != nullptr;
}

//#127_shared_Haiya_Juukaku
bool CThProcRenderImp::setupShareClientNvrAsMaster(CThProcRender* other)
{
	 
	CThProcRenderImp* org_p = dynamic_cast<CThProcRenderImp*>(other);
	if (org_p == nullptr) return false;
	std::string sessonID = org_p->getOtherSession();
	if (sessonID.empty()) return false;
	m_shared_sessionID_of_Master = sessonID;
 
	return true;
}
std::string CThProcRenderImp::getOtherSession()
{
	std::string str;
	if (m_ClientNvr == nullptr) return str;
	char _session_str[64];
	_session_str[0] = 0;
	m_ClientNvr->getSessionID(_session_str,sizeof(_session_str));
	str = _session_str;
	return str;
}
bool CThProcRenderImp::loadVol(const std::string& study_uid, const std::string& series_uid)
{
	m_exe_status = false;
	m_VolData.reset();
	PxVCoreAPI::PXSMVVolumeData* vol_data = nullptr;
	PxVCoreAPI::PXSMVDICOMData load_seriesData;
	memset(&load_seriesData, 0, sizeof(load_seriesData));
	_copy_str(load_seriesData.m_studyUID, study_uid.c_str());
	_copy_str(load_seriesData.m_seriesUID, series_uid.c_str());
	_copy_str(load_seriesData.m_AETitle, "DB");
	if (m_ClientNvr == nullptr) return false;
	vol_data = m_ClientNvr->LoadData(&load_seriesData);
	if (vol_data == nullptr) {
		if (m_logger != nullptr) {
			m_logger->LogMessage(L"CThProcRenderImp::loadVol failed\n");
		}
		return false;
	}
	m_VolData.reset(vol_data);
	//
	int  sizeX, sizeY, sizeZ;
	float pitchX, pitchY, pitchZ;
	m_VolData->getVolumeDimesion(sizeX, sizeY, sizeZ, pitchX, pitchY, pitchZ);
	m_Vol3DData.sizeX = sizeX;
	m_Vol3DData.sizeY = sizeY;
	m_Vol3DData.sizeZ = sizeZ;
	m_Vol3DData.ptichX = pitchX;
	m_Vol3DData.ptichY = pitchY;
	m_Vol3DData.ptichZ = pitchZ;
	//
#if 0
	{
		int ww, wl;
		m_VolData->getWWWL(ww, wl);
		m_Vol3DData.window_levle = wl;
		m_Vol3DData.window_width = ww;
	}
#endif
	//
	PxVCoreAPI::PXSMVRenderParam* render_param_2D = m_ClientNvr->createRenderParam(m_VolData.get(),
		PxVCoreAPI::Viewer_Orientation_Axial,
		PxVCoreAPI::Viewer_Projection_MPR);
	if (render_param_2D == nullptr) {
		if (m_logger != nullptr) {
			m_logger->LogMessage(L"CThProcRenderImp::loadVol createRenderParam failed\n");
		}
		return false;
	}
	m_renderParam2D.reset(render_param_2D);
	//
	PxVCoreAPI::PXSMVRenderParam* render_param_3D = m_ClientNvr->createRenderParam(m_VolData.get(),
		PxVCoreAPI::Viewer_Orientation_Unknown,
		PxVCoreAPI::Viewer_Projection_VR);
	if (render_param_3D == nullptr) {
		if (m_logger != nullptr) {
			m_logger->LogMessage(L"CThProcRenderImp::loadVol createRenderParam failed\n");
		}
		return false;
	}
	m_renderParam3D.reset(render_param_3D);

	setup3DLut(1);
	if(abs(m_Rotation3DXYDeg)>0.0001){
		 m_renderParam3D->RotationViewMat->get().getData();
		 int render_sizeX = m_renderParam3D.get()->RenderSize[0]->get();
		 int PointDeltaX = -10;
		 int PointDeltaY = 0;
		 int run_num_90_deg = 40;
		 int run_num = (int)(abs(m_Rotation3DXYDeg) / 90.0f * run_num_90_deg);
		 if (m_Rotation3DXYDeg > 0) {
			 PointDeltaX = -PointDeltaX;
		 }
		 for (int i = 0; i < run_num; i++) {
			 m_ClientNvr->RotateVR(m_renderParam3D.get(), PointDeltaX, PointDeltaY);
		 }
		  
	}
	m_hideModels.clear();//#169_delete_CT-Marker
	m_renderOutputImg.reset();
	m_exe_status = true;
	return true;
}
const ThProcLib_Vol3DData* CThProcRenderImp::getVol3DData(void) const
{
	return &m_Vol3DData;
}
void CThProcRenderImp::setup3DModelDisplay(const ThProcLib_RenderGC* param)
{
	bool bShowGenericPolygon = (param->display_mode & ThLibDisplayMode_Polygon) ||
		(param->display_mode & ThLibDisplayMode_CTMarker);

	m_ClientNvr->EnableGenericPolygon(bShowGenericPolygon);
	if (bShowGenericPolygon) {
		PxVCoreAPI::PXSMVDICOMData* pDicmData = m_VolData->getDICOMData();
		if ((param->display_mode & ThLibDisplayMode_Polygon)) {
#if 0
//#160_changed_ThLibRenderCmd_Setup
			if (m_3DModelDisplayParts == ThLibDisplayParts_Other) {
			show3DModel( true, eModel_all);

			}
			else
#endif
			{
				//clear all
				show3DModel(false, eModel_all);

				//show ROI-marker
				show3DModel(true, eModel_roi_marker);

				if (m_3DModelDisplayParts & ThLibDisplayParts_ROI) {
					show3DModel(true, eModel_roi_marker);
				}
				if (m_3DModelDisplayParts & ThLibDisplayParts_Body) {
					show3DModel(true, eModel_body);
				}
				if (m_3DModelDisplayParts & ThLibDisplayParts_Lung) {
					show3DModel(true, eModel_lung);
				}
				if (m_3DModelDisplayParts & ThLibDisplayParts_Heart) {
					show3DModel(true, eModel_vein);
					show3DModel(true, eModel_heart);
					show3DModel(true, eModel_artery);
				}
			 
				if (m_3DModelDisplayParts & ThLibDisplayParts_Other) {
					//#160_changed_ThLibRenderCmd_Setup
					//other
					show3DModel(true, eModel_liver);
					show3DModel(true, eModel_stomach);
					show3DModel(true, eModel_spinal_cord);
					show3DModel(true, eModel_rib);
					show3DModel(true, eModel_vertebrae);
					show3DModel(true, eModel_clavicula);
					show3DModel(true, eModel_costal_cartilages);
					show3DModel(true, eModel_sternum_scapula);
				}
			}
		}
		else {
			show3DModel( false, eModel_all);
			show3DModel( true, eModel_ct_marker);
		}

	}
}
void CThProcRenderImp::setupRenderAxial(const ThProcLib_RenderGC* param, PxVCoreAPI::PXSMVRenderParam* render_param, bool bForCoord)
{
	float slice_pos = param->slice_position-1;
	//	if (slice_pos < 0) slice_pos = 0;
	slice_pos = -slice_pos;

	float n_v[3] = { 0,0,1 };
	PxVCoreAPI::fMat13 slice_normal(n_v);
	render_param->RenderSize[0]->set(param->target_sizeX);
	render_param->RenderSize[1]->set(param->target_sizeY);
	render_param->SliceNormal->set(slice_normal);
	render_param->SlicePosition->set(slice_pos);// -160.0);
	render_param->WL->set(param->window_levle);// -100);
	render_param->WW->set(param->window_width);
	render_param->Zoom->set(param->zoom);
	render_param->PlaneType->set(2);
	render_param->ImageQuality->set(100);
	render_param->BitsPerPixel->set(8);
	render_param->RenderImageType->set(PxVCoreAPI::Viewer_ImageType_BMP);
	
	if (m_renderImgRef2D.m_valid != 0) {//#199_panXY
		if (param->panX != 0) {
			int xx = 0;
		}
		float panX = param->panX * m_renderImgRef2D.m_xScale;
		float panY = param->panY * m_renderImgRef2D.m_yScale;

		render_param->PanX->set(panX);
		if (bForCoord) {//#199_panXY
			render_param->PanY->set(panY);
		}
		else {
			render_param->PanY->set(-panY);
		}
	}
}
bool CThProcRenderImp::renderAxial(const ThProcLib_RenderGC* param)
{
	float df_zoom = abs(m_curAxialRenderGC.zoom - param->zoom);
	float df_panX = abs(param->panX);
	float df_panY = abs(param->panY);

	m_curAxialRenderGC = *param;//#176_insert_CT_Marker_on_2D

//#199_panXY
	if ( (df_zoom>0.000001) && ((df_panX > 0.000001) || (df_panY > 0.000001))) {
		//update m_renderImgRef2D for Pan calculation
		renderAxial(param);
	}

	PxVCoreAPI::PXSMVImage* img_out = nullptr;
 
	setupRenderAxial(param, m_renderParam2D.get());
 
 	//#100_load_3DModel_and_show_3DModel
	if (m_3DModelAlpha == 0) {
		m_renderParam2D->WireFrameMode->set(1);
	}
	setup3DModelDisplay(param);
	if (m_logger != nullptr) {
		m_logger->LogMessage(L"to 2D Render\n");
	}
	 
	bool bSts = m_ClientNvr->Render(m_VolData.get(), m_renderParam2D.get(), img_out);
	if (m_logger != nullptr) {
		m_logger->LogMessage(L" 2D Render OK\n");
	}
	if (bSts && img_out != nullptr) {
		m_renderOutputImg.reset(img_out);
		//#199_panXY
		backupRenderImgInfo(m_renderImgRef2D, m_renderOutputImg.get());
	}
	else {

	}
	
	return bSts;
}
bool CThProcRenderImp::render3D(const ThProcLib_RenderGC* param)
{
	 
	float df_zoom = abs(m_cur3DRenderGC.zoom - param->zoom);
	float df_panX = abs(param->panX);
	float df_panY = abs(param->panY);
 
	m_cur3DRenderGC = *param;//#125_insert_CT_Marker

//#199_panXY
	if ((df_zoom > 0.000001) && ((df_panX > 0.000001) || (df_panY > 0.000001))) {
		//update m_renderImgRef3D for Pan calculation
		render3D(param);
	}

	PxVCoreAPI::PXSMVImage* img_out = nullptr;
	float slice_pos = param->slice_position;
	//	if (slice_pos < 0) slice_pos = 0;
	slice_pos = -slice_pos;

	float n_v[3] = { 0,0,1 };
	PxVCoreAPI::fMat13 slice_normal(n_v);
	m_renderParam3D->RenderSize[0]->set(param->target_sizeX);
	m_renderParam3D->RenderSize[1]->set(param->target_sizeY);
//	m_renderParam3D->SliceNormal->set(slice_normal);
//	m_renderParam3D->SlicePosition->set(-160.0);
	m_renderParam3D->WL->set(-100);
	m_renderParam3D->WW->set(2000);
	m_renderParam3D->Zoom->set(param->zoom);//#113_3D_Zoom
	m_renderParam3D->PlaneType->set(2);
	m_renderParam3D->ImageQuality->set(100);
	m_renderParam3D->BitsPerPixel->set(16);
	m_renderParam3D->RenderImageType->set(PxVCoreAPI::Viewer_ImageType_BMP);

	setup3DModelDisplay(param);
	//#199_panXY
	if (m_renderImgRef3D.m_valid != 0) {
		if (param->panX != 0) {
			int xx = 0;
		}
		float panX = param->panX * m_renderImgRef3D.m_xScale;
		float panY = param->panY * m_renderImgRef3D.m_yScale;
		if ((panX * panX > 0.00001) || (panY * panY > 0.00001)) {
			m_renderParam3D->PanX->set(panX);
			m_renderParam3D->PanY->set(panY);

			m_renderParam3D->updateInside();
		}
	}

	if (m_logger != nullptr) {
		m_logger->LogMessage(L"to 3D Render\n");
	}
	bool bSts = m_ClientNvr->Render(m_VolData.get(), m_renderParam3D.get(), img_out);
	if (m_logger != nullptr) {
		m_logger->LogMessage(L" 3D Render OK\n");
	}
	if (bSts && img_out != nullptr) {
		m_renderOutputImg.reset(img_out);
		//#199_panXY
		backupRenderImgInfo(m_renderImgRef3D, m_renderOutputImg.get());
	} 
	else {
		
	}
	return bSts;
}
const unsigned char* CThProcRenderImp::getImageData(int& BGRA_size) const
{
	const unsigned char* pImg = nullptr;
	if (m_renderOutputImg!= nullptr) {
		pImg = m_renderOutputImg->getPixels();
		BGRA_size = m_renderOutputImg->m_bitsPerPixel / 8;
	}
	return pImg;
}
struct BGRA_def {
	unsigned char B;
	unsigned char G;
	unsigned char R;
	unsigned char A;
};
struct BGR_def {
	unsigned char B;
	unsigned char G;
	unsigned char R;
};
//setup RGB order 2025/07/03
bool  CThProcRenderImp::copyOutputImage(unsigned char* BGRA_DataBuffer, int BufferSize) const
{
	if (m_renderOutputImg == nullptr) return false;
	int  BGRA_size;
	const unsigned char* img_data = this->getImageData(BGRA_size);
	int img_size = m_renderOutputImg->m_width * m_renderOutputImg->m_height;
	int sizeX = m_renderOutputImg->m_width;
	int sizeY = m_renderOutputImg->m_height;
	if (BufferSize < img_size) return false;
	switch (BGRA_size) {
	case 1://2D+3DModleOff
	{
		BGRA_def* pbuffer = (BGRA_def*)BGRA_DataBuffer;
		for (int y_i = 0; y_i < sizeY; y_i++) {
			const unsigned char* src_line = img_data + (sizeY - 1 - y_i)*sizeX;
			BGRA_def* dest_line = pbuffer + y_i * sizeX;
			for (int x_i = 0; x_i < sizeX; x_i++) {
				unsigned char v = src_line[x_i];
				dest_line[x_i].A = 255;// v;
				dest_line[x_i].R = v;
				dest_line[x_i].G = v;
				dest_line[x_i].B = v;
			}
		}
 
	}
	break;
	case 3://VR
	{
		BGRA_def* pbuffer = (BGRA_def*)BGRA_DataBuffer;
		const BGR_def* src_buffer = (BGR_def*)img_data;
		for (int y_i = 0; y_i < sizeY; y_i++) {
			const BGR_def* src_line = src_buffer + (sizeY - 1 - y_i) * sizeX;
			BGRA_def* dest_line = pbuffer + y_i * sizeX;
			for (int x_i = 0; x_i < sizeX; x_i++) {
				BGR_def v = src_line[x_i];
				dest_line[x_i].A =  255;
				dest_line[x_i].R =  v.R;
				dest_line[x_i].G =  v.G;
				dest_line[x_i].B =  v.B;
			}
		}
 
	}
	break;
	case 4://2D+3DModleOn
	{
		BGRA_def* pbuffer = (BGRA_def*)BGRA_DataBuffer;
		const BGRA_def* src_buffer = (BGRA_def*)img_data;
		for (int y_i = 0; y_i < sizeY; y_i++) {
			const BGRA_def* src_line = src_buffer + (sizeY - 1 - y_i) * sizeX;
			BGRA_def* dest_line = pbuffer + y_i * sizeX;
			for (int x_i = 0; x_i < sizeX; x_i++) {
				BGRA_def v = src_line[x_i];
				dest_line[x_i].A =  255;// v.A;
				dest_line[x_i].R =  v.R;
				dest_line[x_i].G =  v.G;
				dest_line[x_i].B =  v.B;
			}
		}
 
	}
	break;

	}
	{
#if 0
		std::wstring file_str = L"C:/tmp/dbg.img_get_";
		file_str = file_str + std::to_wstring(sizeX) + L"_" + std::to_wstring(sizeY);
		file_str = file_str + L"_" + std::to_wstring(32) + L".raw";
		FILE* fp = nullptr;
		_wfopen_s(&fp, file_str.c_str(), L"wb");
		fwrite(BGRA_DataBuffer, 4, sizeX * sizeY, fp);
		fclose(fp);
#endif
	}
	return true;
}
bool CThProcRenderImp::get_dicom_info(const std::string& study_uid,  ThProcLib_DicomImageInfo* image_info) const
{
	if (m_VolData == nullptr) return false;
	{
		float  minX, maxX, minY, maxY, minZ, maxZ;
		m_VolData->getVolumeRange(minX, maxX, minY, maxY, minZ, maxZ);
		int  sizeX, sizeY, sizeZ;
		float pitchX, pitchY, pitchZ;
		m_VolData->getVolumeDimesion(sizeX, sizeY, sizeZ, pitchX, pitchY, pitchZ);
		int ww, wl;
		m_VolData->getWWWL(ww, wl);
		 
		float  Slope, Offset;
		m_VolData->getSlopeOffset(Slope, Offset);
		//
		image_info->sizeX = sizeX;
		image_info->sizeY = sizeY;
		image_info->sizeZ = sizeZ;
		image_info->WindowWidth = ww;
		image_info->WindowCenter = wl;
	}
	return true;
}
 
bool CThProcRenderImp::getProgress(int& completed, float& progress) const
{
	completed = 1;
	progress = 100;
	return m_exe_status;
}
//#100_load_3DModel_and_show_3DModel

bool CThProcRenderImp::load3DModel(const std::string& study_uid, const std::string& series_uid, eModelName model)
{
 //#127_shared_Haiya_Juukaku
	if (!m_shared_sessionID_of_Master.empty()) {
		//do not load 3D model in shared session
		return true;
	}
	m_renderGrpParam->m_bRoiClear = false;//#175_clear_exist_ROI_Marker

	m_exe_status = false;
	 
 
	memset(&m_CurSeriesData, 0, sizeof(m_CurSeriesData));
	_copy_str(m_CurSeriesData.m_studyUID, study_uid.c_str());
	_copy_str(m_CurSeriesData.m_seriesUID, series_uid.c_str());
//	_copy_str(seriesData.m_AETitle, "DB");
	std::vector<std::wstring> fileList;
	bool bCheckShow = false;//#169_delete_CT-Marker
	get3DModelFiles(bCheckShow, fileList, model);
	for (int i = 0; i < fileList.size(); i++) {
		std::string model_name = ThStringUtils::_convertStr(fileList[i]);
		m_ClientNvr->LoadGenericPolygonFile(&m_CurSeriesData, model_name.c_str());
	}
	m_exe_status = true;
	return true;
}
 
#include <map>
std::map< CThProcRender::eModelName, std::wstring> NameList3Model= {
{	CThProcRender::eModel_vein,					   add_ext_stl(def_vein)},
{	CThProcRender::eModel_artery,                  add_ext_stl(def_artery)  },
{	CThProcRender::eModel_heart,                   add_ext_stl(def_heart) },
{	CThProcRender::eModel_liver,                   add_ext_stl(def_liver) },
{	CThProcRender::eModel_lung,                    add_ext_stl(def_lung) },
{	CThProcRender::eModel_lung_seg,                add_ext_stl(def_lung_seg) },
{	CThProcRender::eModel_lung_vessels,            add_ext_stl(def_lung_vessels)   },
//{	CThProcRender::eModel_lung_substraction,       add_ext_stl(def_lung_substraction )       },
{	CThProcRender::eModel_rib,                     add_ext_stl(def_rib)  }	,
{	CThProcRender::eModel_vertebrae,               add_ext_stl(def_vertebrae)  },
{	CThProcRender::eModel_clavicula,               add_ext_stl(def_clavicula) },
{	CThProcRender::eModel_costal_cartilages,       add_ext_stl(def_costal_cartilages)  },
{	CThProcRender::eModel_sternum_scapula,         add_ext_stl(def_sternum_scapula) },
{	CThProcRender::eModel_stomach,                 add_ext_stl(def_stomach)  },
{	CThProcRender::eModel_spinal_cord,             add_ext_stl(def_spinal_cord)  },
{	CThProcRender::eModel_body,                    add_ext_stl(def_body)  },
{	CThProcRender::eModel_ct_marker,               add_ext_stl(def_ct_marker)   },		//ROI ->lung_substraction
{	CThProcRender::eModel_roi_marker,              add_ext_stl(def_roi_marker) },
 
};
 
bool CThProcRenderImp::show3DModel(bool OnOff, eModelName model)
{
	//#127_shared_Haiya_Juukaku
	//change to m_masterSeriesType
	if (!m_shared_sessionID_of_Master.empty()) {
		return m_render_pair->pair_show3DModel(OnOff, model);
	}
	else {
		return show3DModel_in(OnOff, model);
	}
}
bool CThProcRenderImp::show3DModel_in(bool OnOff, eModelName model)
{
#if 0
	if (m_logger != nullptr) {
		m_logger->LogMessage(L"show3DModel entry\n");
		std::wstring uid_str = ThStringUtils::_convertStr(study_uid+";"+ series_uid);
		m_logger->LogMessage(L" %s\n", uid_str.c_str());
	}
#endif

	std::vector<std::wstring> fileList;
	//#169_delete_CT-Marker
	bool bCheckShow = OnOff;//just check display On
	get3DModelFiles(bCheckShow,fileList, model );
	PxVCoreAPI::PXSMVDICOMData seriesData_null;
	memset(&seriesData_null, 0, sizeof(seriesData_null));
//	_copy_str(seriesData.m_studyUID, study_uid.c_str());
//	_copy_str(seriesData.m_seriesUID, series_uid.c_str());
	for (int i = 0; i < fileList.size(); i++) {
		std::string model_name = ThStringUtils::_convertStr(fileList[i]);
		m_ClientNvr->ShowGenericPolygonFile(&seriesData_null, model_name.c_str(), OnOff);
	}
#if 0
	if (m_logger != nullptr) {
		m_logger->LogMessage(L"show3DModel leave\n");
	}
#endif
	return true;
}
void CThProcRenderImp::get3DModelFiles(bool bCheckShow,std::vector<std::wstring>& fileList, eModelName model)
{
	//#169_delete_CT-Marker
	//bCheckShow
	
	fileList.clear();
	if (model == eModel_unknown) return;
	if (model == eModel_all) {
		for (auto it : NameList3Model) {
			if (bCheckShow) {
				if (isHidedModel(it.first)) continue;
			}
			fileList.push_back(it.second);
		}
	}
	else {
		for (auto it : NameList3Model) {
			if (it.first == model) {
				if (bCheckShow) {
					if (isHidedModel(it.first)) break;
				}
				fileList.push_back(it.second);
				break;
			}
		}
	}
}
//#161_display_thoracentes_needle
double dist_2Points(double p1[2] , double p2[2]) {
	 
	double dx = p1[0] - p2[0];
	double dy = p1[1] - p2[1];
	 
	double dist = dx * dx + dy * dy  ;
	return sqrt(dist);
}
double dist_2Points(const std::vector<double>& p1, const std::vector<double>& p2) {
	if ((p1.size() < 3) || (p2.size() < 3)) return 0;
	double dx = p1[0] - p2[0];
	double dy = p1[1] - p2[1];
	double dz = p1[2] - p2[2];
	double dist = dx * dx + dy * dy + dz * dz;
	return sqrt(dist);
}
bool vec_2Points(const std::vector<double>& p1, const std::vector<double>& p2,double *vec) {
	if ((p1.size() < 3) || (p2.size() < 3)) return false;
	double dx = p2[0] - p1[0];
	double dy = p2[1] - p1[1];
	double dz = p2[2] - p1[2];
	double dist = dx * dx + dy * dy + dz * dz;
	dist = sqrt(dist);
	if (dist < 0.0000000000001) dist = 0.0000000000001;
	vec[0] = dx / dist;
	vec[1] = dy / dist;
	vec[2] = dz / dist;
	return true;
}
void CThProcRenderImp::clearCurRoiPos(void) {
	m_CurRoiPos[0] = m_CurRoiPos[1] = m_CurRoiPos[2] = -10000;
	m_CurRoiPosTime = ::GetTickCount();
}
//#115_added_Polygon
void CThProcRenderImp::procMouseEvent_AddPolygon(const ThProcLib_RenderGC* param)
{
 
	double OutP3D[3];
	if (!pickupWorldPoint(param, OutP3D)) return;
 
	addROIData( OutP3D);
}
bool CThProcRenderImp::pair_addROIData(double p3D[3])
{
	return addROIData_in(p3D);
}
bool CThProcRenderImp::addROIData(double OutP3D[3])
{
//add ROI Data alway in MasterSeries
// ref: exe_cmd_ROI_Clear
//  using m_masterSeriesType

	if (!m_shared_sessionID_of_Master.empty()) {
		return m_render_pair->pair_addROIData(OutP3D);
	}
	else {
		return addROIData_in(OutP3D);
	}
}
bool CThProcRenderImp::addROIData_in(double OutP3D[3])
{

//#161_display_thoracentes_needle
	const int mouse_inter_time = 320;
	double dist_2p = dist_2Points(OutP3D, m_CurRoiPos);
	DWORD cur_time = ::GetTickCount();
	DWORD diff_time = cur_time - m_CurRoiPosTime;
	if ((dist_2p < 1)&&(diff_time< mouse_inter_time)) {
		return false;
	}
	m_CurRoiPosTime = cur_time;
	m_CurRoiPos[0] = OutP3D[0];
	m_CurRoiPos[1] = OutP3D[1];
	m_CurRoiPos[2] = OutP3D[2];

	//double Point3D[3] = { 0,-90,0 };
	double Vec[3] = { 0,0,1 };
	int ObjType = 0;
	int ExtStlType = PxVCoreAPI::ExtStlType_RoiMarker;
	int obj_id = m_ClientNvr->AddGenericPolygonObject(m_VolData.get(), OutP3D, Vec, m_renderGrpParam->m_penWidth, ExtStlType);
	double obj_dia = m_ClientNvr->GetGenericPolygonObjectDia(m_renderGrpParam->m_penWidth);
	GenericObjBallInfo ObjBall;
	ObjBall.m_Center[0] = OutP3D[0];
	ObjBall.m_Center[1] = OutP3D[1];
	ObjBall.m_Center[2] = OutP3D[2];
	ObjBall.m_DistR2 = (obj_dia/2)* (obj_dia / 2);
	ObjBall.m_ObjID = obj_id;
	ObjBall.m_ObjStyp = PxVCoreAPI::ExtStlType_RoiMarker;
	m_renderGrpParam->m_ROI_Cur_ObjIDs.push_back(ObjBall);


//#161_display_thoracentes_needle
	m_renderGrpParam->m_ROI_Points.push_back({
		{ OutP3D[0], OutP3D[1], OutP3D[2] },
		cur_time
		}
		); //#202_added_point_time_stamp
	{
		if ((m_needle_OnOff!=0)&&(m_renderGrpParam->m_ROI_Points.size() == 2)) {
			 
	 
			const CThProcRenderGrpParam::ROI_Point_with_time& p1 = m_renderGrpParam->m_ROI_Points[0];
			const CThProcRenderGrpParam::ROI_Point_with_time& p2 = m_renderGrpParam->m_ROI_Points[1];
			double dist = dist_2Points(p1.m_p, p2.m_p);
			int diff_time = p1.m_time_stamp - p2.m_time_stamp;
			if (diff_time < 0) diff_time = -diff_time;
			if ((dist > 14) && (diff_time>500)) {//#202_added_point_time_stamp
				int out_len = m_needle_L - dist;

				int ext_line = m_needle_ext_L - out_len;
				if (ext_line < 0) ext_line = 0;
				//vector: p2-p1
				vec_2Points(p1.m_p, p2.m_p, Vec);
				double needl_end_p[3];
#if 0
				needl_end_p[0] = p2[0] + Vec[0] * ext_line;
				needl_end_p[1] = p2[1] + Vec[1] * ext_line;
				needl_end_p[2] = p2[2] + Vec[2] * ext_line;
#else
				needl_end_p[0] = p1.m_p[0] + Vec[0] * m_needle_L;
				needl_end_p[1] = p1.m_p[1] + Vec[1] * m_needle_L;
				needl_end_p[2] = p1.m_p[2] + Vec[2] * m_needle_L;
#endif
				//
				double centerP3D[3];
				centerP3D[0] = (p1.m_p[0] + needl_end_p[0]) / 2.0;
				centerP3D[1] = (p1.m_p[1] + needl_end_p[1]) / 2.0;
				centerP3D[2] = (p1.m_p[2] + needl_end_p[2]) / 2.0;
				int ObjSizeNo = m_needle_D * 2048 + m_needle_L;// (dist + ext_line);
				int ExtStlType = PxVCoreAPI::ExtStlType_InsertionPathway;
				int obj_id = m_ClientNvr->AddGenericPolygonObject(m_VolData.get(), centerP3D, Vec, ObjSizeNo, ExtStlType);
				GenericObjBallInfo ObjBall;
				ObjBall.m_Center[0] = centerP3D[0];
				ObjBall.m_Center[1] = centerP3D[1];
				ObjBall.m_Center[2] = centerP3D[2];
				ObjBall.m_DistR2 = 10*10;
				ObjBall.m_ObjID = obj_id;
				ObjBall.m_ObjStyp = PxVCoreAPI::ExtStlType_InsertionPathway;

				m_renderGrpParam->m_ROI_Cur_ObjIDs.push_back(ObjBall);
			}
		}
	}
	return true;
}
bool CThProcRenderImp::pickupWorldPoint(const ThProcLib_RenderGC* param, double* OutP3D)
{
	if (m_VolData == nullptr) false;
	int v_size[3];
	float f_pitch[3];
	m_VolData.get()->getVolumeDimesion(v_size[0], v_size[1], v_size[2],
		f_pitch[0], f_pitch[1], f_pitch[2]);
	double inP2D[2];
	inP2D[0] = param->mouse_posX;
	inP2D[1] = param->target_sizeY - param->mouse_posY;

//adjust the slice position
	m_renderParam2D->SlicePosition->set(param->slice_position - v_size[2]/2.0f-1.0f);
	m_renderParam2D->RenderSize[0]->set(param->target_sizeX);
	m_renderParam2D->RenderSize[1]->set(param->target_sizeY);
	m_ClientNvr->ClientMPR2World(m_VolData.get(), m_renderParam2D.get(), inP2D, OutP3D);

	return true;
}
void CThProcRenderImp::procMouseEvent_ErasePolygon(const ThProcLib_RenderGC* param)
{
	//ref: procMouseEvent_AddPolygon
	//pick up point here
 
	if (!pickupWorldPoint(param, m_renderGrpParam->CurWorldP3D)) return;
 
}
void CThProcRenderImp::saveRoiData(const ThProcLib_RenderGC* param)
{
	//if (m_renderGrpParam->m_ROI_ObjIDs.size() < 1) return;
	int ExtStlType = 0 ;
	if (m_renderGrpParam->m_bRoiClear) {
		//#175_clear_exist_ROI_Marker
		//setup clear flag 
		ExtStlType += ClearExistObjMask;
	}
	m_ClientNvr->SaveGenericPolygonObjectToStl(m_VolData.get(), ExtStlType);
	removeAllRoiObjs();
}

void CThProcRenderImp::removeAllRoiObjs(void)
{
	for (int i = 0; i < m_renderGrpParam->m_ROI_Grp_ObjIDs.size(); i++) {
		//#195_using_Grp_for_Undo 
		auto& cur_ObjIDs = m_renderGrpParam->m_ROI_Grp_ObjIDs[i];

		for (int ij = 0; ij < cur_ObjIDs.size(); ij++) {
			m_ClientNvr->RemoveGenericPolygonObject(cur_ObjIDs[ij].m_ObjID, true);
		}
	}
	m_renderGrpParam->m_ROI_Grp_ObjIDs.clear();
}
//#195_using_Grp_for_Undo 
void CThProcRenderImp::AddGrpRoiObjs(void)
{
	m_renderGrpParam->m_ROI_Grp_ObjIDs.push_back(m_renderGrpParam->m_ROI_Cur_ObjIDs);
	m_renderGrpParam->m_ROI_Cur_ObjIDs.clear();
}
void CThProcRenderImp::clearRoiData(const ThProcLib_RenderGC* param)
{
	//#175_clear_exist_ROI_Marker
	m_renderGrpParam->m_bRoiClear = true;
	clearCurRoiPos();
	removeAllRoiObjs();
	m_renderGrpParam->m_ROI_Points.clear();

	//#175_clear_exist_ROI_Marker
	//just hide it only here
	enableModel(eModel_roi_marker, false);
	if (m_render_pair != nullptr) {
		m_render_pair->pair_enableModel(eModel_roi_marker, false);
	}
}
bool CThProcRenderImp::saveCTMarker(void)
{
	bool bSts = true;
	if (m_renderGrpParam->m_CT_Marker_ObjIDs.size() < 1) {
		//normal pass
		return true;
	}

	int ExtStlType = 1;
	//#175_clear_exist_ROI_Marker
	//setup clear flag 
	//CT-Marker, clear exist always
	ExtStlType += ClearExistObjMask;

	m_ClientNvr->SaveGenericPolygonObjectToStl(m_VolData.get(), ExtStlType);
	PxVCoreAPI::PXSMVDICOMData* pDcm= m_VolData->getDICOMData();
	if (m_renderGrpParam->m_CT_Marker_P3D_in_Voxel.size() > 2) {
		double p3D_1[3];
		double p3D_2[3];
		double p3D_3[3];
		for (int i = 0; i < 3; i++) {
			p3D_1[i] = m_renderGrpParam->m_CT_Marker_P3D_in_Voxel[0][i];
			p3D_2[i] = m_renderGrpParam->m_CT_Marker_P3D_in_Voxel[1][i];
			p3D_3[i] = m_renderGrpParam->m_CT_Marker_P3D_in_Voxel[2][i];
		}
		{//adjust coorf for thdcm2mesh 
			int sizeY = m_Vol3DData.sizeY;
			int sizeZ = m_Vol3DData.sizeZ;
			//Y
			p3D_1[1] = sizeY - p3D_1[1];
			p3D_2[1] = sizeY - p3D_2[1];
			p3D_3[1] = sizeY - p3D_3[1];
			//Z
			p3D_1[2] = sizeZ - p3D_1[2];
			p3D_2[2] = sizeZ - p3D_2[2];
			p3D_3[2] = sizeZ - p3D_3[2];
		}
		bool async = false;
		bSts = m_pPcCoreLib_ref->setupCTMarker(p3D_1, p3D_2, p3D_3,pDcm->m_studyUID, async);
	}
	else {
		bSts = false;
	}
	for (int i = 0; i < m_renderGrpParam->m_CT_Marker_ObjIDs.size(); i++) {
		m_ClientNvr->RemoveGenericPolygonObject(m_renderGrpParam->m_CT_Marker_ObjIDs[i], true);
	}
	m_renderGrpParam->m_CT_Marker_ObjIDs.clear();
	return bSts;
}
void CThProcRenderImp::clearCTMarker(void)
{
	for (int i = 0; i < m_renderGrpParam->m_CT_Marker_ObjIDs.size(); i++) {
		m_ClientNvr->RemoveGenericPolygonObject(m_renderGrpParam->m_CT_Marker_ObjIDs[i], true);
	}
	m_renderGrpParam->m_CT_Marker_ObjIDs.clear();
	m_renderGrpParam->m_CT_Marker_P3D_in_Voxel.clear();
	enableModel(eModel_ct_marker, false);
	if (m_render_pair != nullptr) {
		m_render_pair->pair_enableModel(eModel_ct_marker, false);
	}

}
//#112_Rotatio_3D
void CThProcRenderImp::procMouseEvent(const ThProcLib_RenderGC* param, int& next_render_cmds)
{
	switch (param->mouse_type)
	{
	case ThLibMouseEvent_LBUTTONDOWN:
		m_mouseDownPos[0] = m_mouseCurPos[0] = param->mouse_posX;
		m_mouseDownPos[1] = m_mouseCurPos[1] = param->mouse_posY;
		if (m_renderGrpParam != nullptr)
			m_renderGrpParam->m_bLButtonDown = true;

		break;
	case ThLibMouseEvent_LBUTTONUP:
		onLButtonUp();
		next_render_cmds |= ThLibRRCmd_2D_1;
		next_render_cmds |= ThLibRRCmd_3D;
		if (m_renderGrpParam != nullptr)
			m_renderGrpParam->m_bLButtonDown = false;
		break;
	case ThLibMouseEvent_3D_Rotation:
	{
		int PointDeltaX = param->mouse_posX - m_mouseCurPos[0];
		m_mouseCurPos[0] = param->mouse_posX;
		//
		int PointDeltaY = param->mouse_posY - m_mouseCurPos[1];
		m_mouseCurPos[1] = param->mouse_posY;

		if ((std::abs(PointDeltaX) > 180)|| (std::abs(PointDeltaY) > 180)) {
			//reset it 
			m_mouseDownPos[0] = m_mouseCurPos[0] = param->mouse_posX;
			m_mouseDownPos[1] = m_mouseCurPos[1] = param->mouse_posY;
		}
		else {
			m_ClientNvr->RotateVR(m_renderParam3D.get(), PointDeltaX, PointDeltaY);
			if (m_render_pair != nullptr) {
				m_render_pair->pair_RoatationVR(PointDeltaX, PointDeltaY);
			}
		}
	}
	break;
	case ThLibMouseEvent_CT_Marker_2D:
	//#125_insert_CT_Marker
		//#176_insert_CT_Marker_on_2D
		insertCTMarkerOn2D(param);
		next_render_cmds |= ThLibRRCmd_2D_1;
		next_render_cmds |= ThLibRRCmd_3D;
		break;
	case ThLibMouseEvent_CT_Marker_3D:
	//#125_insert_CT_Marker
		insertCTMarkerOn3D(param);
		next_render_cmds |= ThLibRRCmd_2D_1;
		next_render_cmds |= ThLibRRCmd_3D;
		break;
 
	case ThLibMouseEvent_ROI_Eraser:
		{
			int xx = 0;
		}
		break;
	}
	
}

//#125_insert_CT_Marker
//#176_insert_CT_Marker_on_2D
void CThProcRenderImp::insertCTMarkerOn2D(const ThProcLib_RenderGC* param)
{
	 
	double inP2D[2];
	double outP3D[3];

	inP2D[0] = param->mouse_posX;
	inP2D[1] = param->mouse_posY;
 
	PxVCoreAPI::PXSMVRenderParam* param_temp = m_ClientNvr->createRenderParam(m_VolData.get(),
		PxVCoreAPI::Viewer_Orientation_Unknown,
		PxVCoreAPI::Viewer_Projection_MPR);
	SmartPtr_PXSMVRenderParam mpr_param;
	mpr_param.reset(param_temp);

	bool bForCoord = true;
	//#199_panXY
	setupRenderAxial(&m_curAxialRenderGC, mpr_param.get(), bForCoord);

	m_ClientNvr->ClientMPR2World(m_VolData.get(), mpr_param.get(), inP2D, outP3D);
	

	double Vec[3] = { 0,0,1 };
	int ObjType = 0;
	//	double ct_marker_p[3];
	int  sizeX, sizeY, sizeZ;
	float pitchX, pitchY, pitchZ;
	m_VolData->getVolumeDimesion(sizeX, sizeY, sizeZ, pitchX, pitchY, pitchZ);
	//adjust it , ref: insertCTMarkerOn3D(): outP3D
	outP3D[1] = -outP3D[1]; //not needed + m_adj_ct_marker_thicness_half;
	outP3D[2] = -pitchZ*sizeZ/2 -outP3D[2];
 
	double outP3DVoxel[3];
	outP3DVoxel[0] = outP3D[0] / pitchX + sizeX / 2.0;
	outP3DVoxel[1] = outP3D[1] / pitchY + sizeY / 2.0;
	outP3DVoxel[2] = outP3D[2] / pitchZ + sizeZ / 2.0;
 
	m_renderGrpParam->m_CT_Marker_P3D_in_Voxel.push_back({ outP3DVoxel[0],outP3DVoxel[1],outP3DVoxel[2] });
	int ExtStlType = PxVCoreAPI::ExtStlType_CTMarker;
	int id = m_ClientNvr->AddGenericPolygonObject(m_VolData.get(), outP3D, Vec, 2, ExtStlType);
	m_renderGrpParam->m_CT_Marker_ObjIDs.push_back(id);
 
}
//#125_insert_CT_Marker
void CThProcRenderImp::insertCTMarkerOn3D(const ThProcLib_RenderGC* param)
{
	m_cur3DRenderGC.display_mode = ThLibDisplayMode_Volume;
	render3D(&m_cur3DRenderGC);

	double inP2D[2];
	double outP3DVoxel[3];
	 
	inP2D[0] = param->mouse_posX;
	inP2D[1] = param->mouse_posY;
//	inP2D[1] = param->target_sizeY - inP2D[1];
 
	PxVCoreAPI::PXSMVRenderParam* param_temp = m_ClientNvr->createRenderParam(m_VolData.get(),
		PxVCoreAPI::Viewer_Orientation_Unknown,
		PxVCoreAPI::Viewer_Projection_MPR);

	SmartPtr_PXSMVRenderParam vr_param;
	vr_param.reset(param_temp);
	vr_param->RenderSize[0]->set(param->target_sizeX);
	vr_param->RenderSize[1]->set(param->target_sizeY);

	float iAlphaTermination = 0.6f;
	m_ClientNvr->pickup3DPoint(m_VolData.get(), vr_param.get(), inP2D, outP3DVoxel, iAlphaTermination);
	double Vec[3] = { 0,0,1 };
	int ObjType = 0;
//	double ct_marker_p[3];
	int  sizeX, sizeY, sizeZ;
	float pitchX, pitchY, pitchZ;
	m_VolData->getVolumeDimesion(sizeX, sizeY, sizeZ, pitchX, pitchY, pitchZ);
	 
	outP3DVoxel[1] += (m_adj_ct_marker_thicness_half / pitchY);//fixed 2025/08/19

	double outP3D[3];
	outP3D[0] = pitchX * (outP3DVoxel[0] - sizeX / 2.0);// / sizeX;;
	outP3D[1] = pitchY * (outP3DVoxel[1] - sizeY / 2.0); // +m_adj_ct_marker_thicness_half;// / sizeY; 
	                                                     // ^^^ fixed 2025/08/19
	outP3D[2] = pitchZ * (outP3DVoxel[2] - sizeZ / 2.0);// / sizeZ;
 
	m_renderGrpParam->m_CT_Marker_P3D_in_Voxel.push_back({ outP3DVoxel[0],outP3DVoxel[1],outP3DVoxel[2] });
	int ExtStlType = PxVCoreAPI::ExtStlType_CTMarker;
	int id = m_ClientNvr->AddGenericPolygonObject(m_VolData.get(), outP3D, Vec, 2, ExtStlType);
	m_renderGrpParam->m_CT_Marker_ObjIDs.push_back(id);
	 
}
//#127_shared_Haiya_Juukaku
void CThProcRenderImp::pair_RoatationVR(int PointDeltaX, int PointDeltaY)
{
	m_ClientNvr->RotateVR(m_renderParam3D.get(), PointDeltaX, PointDeltaY);
}
bool  CThProcRenderImp::pair_show3DModel(bool OnOff, int model)
{
	return show3DModel_in(OnOff, (eModelName)model);
}
void CThProcRenderImp::pair_enableModel(int model, bool bShow)
{
	this->enableModel((eModelName)model, bShow);
}
bool CThProcRenderImp::isHidedModel(eModelName model) const
{
	for (auto it : m_hideModels) {
		if (it.first == model) {
			return it.second != 0;
		}
	}
	//default show all 
	return false;
}
void CThProcRenderImp::enableModel(eModelName model, bool bShow)
{
	m_hideModels[model] = bShow?0:1;
}
//#195_check_LButtonUp_for_ROI_added
void CThProcRenderImp::pair_onLButtonUp(void)
{
	onLButtonUp_in();
}
void CThProcRenderImp::onLButtonUp(void)
{
#if 1
	if (!m_shared_sessionID_of_Master.empty()) {
		return m_render_pair->pair_onLButtonUp();
	}
	else {
		onLButtonUp_in();
	}
#else
	onLButtonUp_in();
#endif
}
void CThProcRenderImp::onLButtonUp_in(void)
{
	if (m_renderGrpParam->m_bLButtonDown)
	{
#if 0
		//save to exist polygon
		if (m_renderGrpParam->m_ROI_ObjIDs.size() > 0) {
	 		this->saveRoiData(nullptr);
			std::string model_name = ThStringUtils::_convertStr(NameList3Model[CThProcRender::eModel_roi_marker]);
		 	m_ClientNvr->LoadGenericPolygonFile(&m_CurSeriesData, model_name.c_str());
			m_ClientNvr->ShowGenericPolygonFile(&m_CurSeriesData, model_name.c_str(), true);
 
		}
#endif
		if (m_renderGrpParam->m_pRoi_Pen_Status_shared) {
			if (*(m_renderGrpParam->m_pRoi_Pen_Status_shared) == ThLibCmd_ROI_Erase) {
				UndoRoiObjs();
				return;
			}
		}
		AddGrpRoiObjs();
	}
}
//#195_using_Grp_for_Undo_find_nearest_Grp
bool hitOneGrpObj(double P3D[3],const std::vector<GenericObjBallInfo>& GrpObj,double &Dist)
{
	bool bSts = false;
	double dist_temp_2 = 1000000000;
	for (int i = 0; i < GrpObj.size(); i++) {
		const GenericObjBallInfo &gObj = GrpObj[i];
		double dx = gObj.m_Center[0] - P3D[0];
		double dy = gObj.m_Center[1] - P3D[1];
		double dz = gObj.m_Center[2] - P3D[2];
		double dist2C = dx * dx + dy * dy + dz * dz;
		if ((gObj.m_ObjStyp == PxVCoreAPI::ExtStlType_RoiMarker) //-hit Ball only
			&&( dist2C < GrpObj[i].m_DistR2)) 
		{
			bSts = true;
			if (dist_temp_2 > dist2C) {
				dist_temp_2 = dist2C;
			}
		}
	}
	if (bSts) {
		Dist = sqrt(dist_temp_2);
	}
	return bSts;
}
void CThProcRenderImp::UndoRoiObjs()
{
 
	//#195_using_Grp_for_Undo_find_nearest_Grp
	double minDist = 1000000;
	int minNo = -1;
	for (int i = 0; i < m_renderGrpParam->m_ROI_Grp_ObjIDs.size(); i++) {
		//#195_using_Grp_for_Undo 
		auto& cur_ObjIDs = m_renderGrpParam->m_ROI_Grp_ObjIDs[i];

		double  DistTemp;
		if (hitOneGrpObj(m_renderGrpParam->CurWorldP3D, cur_ObjIDs, DistTemp)) {
			if (minDist > DistTemp) {
				minDist = DistTemp;
				minNo = i;
			}
		}
	}
	if (minNo >= 0) {
		//remove point from ROI_Points -1 
		std::list<CThProcRenderGrpParam::ROI_Point_with_time> Exist_ROI_Points;
		for (auto it : m_renderGrpParam->m_ROI_Points) {
			Exist_ROI_Points.push_back(it);
		}

		//remove nearest grp;
		auto& nearest_ObjIDs = m_renderGrpParam->m_ROI_Grp_ObjIDs[minNo];
		for (int ij = 0; ij < nearest_ObjIDs.size(); ij++) {
			{//remove point from ROI_Points -2
				double* cp = nearest_ObjIDs[ij].m_Center;
				for (auto it = Exist_ROI_Points.begin(); it != Exist_ROI_Points.end();it++) {
					double dx = (*it).m_p[0] - cp[0];
					double dy = (*it).m_p[1] - cp[1];
					double dz = (*it).m_p[2] - cp[2];
					if ((dx * dx + dy * dy + dz * dz) < 0.0000000001) {
						Exist_ROI_Points.erase(it);
						break;
					}
				}
			}
			m_ClientNvr->RemoveGenericPolygonObject(nearest_ObjIDs[ij].m_ObjID, false);
			                        //--- do not remove model, it's the sampe pen--^^^
		}
		{//remove point from ROI_Points -3
			m_renderGrpParam->m_ROI_Points.clear();
			for (auto it : Exist_ROI_Points) {
				m_renderGrpParam->m_ROI_Points.push_back(it);
			}
		}

		std::vector<std::vector<GenericObjBallInfo>> GrpObjTemp;
		for (int i = 0; i < m_renderGrpParam->m_ROI_Grp_ObjIDs.size(); i++) {
			if (i != minNo) {
				GrpObjTemp.push_back(m_renderGrpParam->m_ROI_Grp_ObjIDs[i]);
			}
			 
		}
		m_renderGrpParam->m_ROI_Grp_ObjIDs.clear();
		for (int i = 0; i < GrpObjTemp.size(); i++) {
			m_renderGrpParam->m_ROI_Grp_ObjIDs.push_back(GrpObjTemp[i]);
		}
	}
	 
}
 