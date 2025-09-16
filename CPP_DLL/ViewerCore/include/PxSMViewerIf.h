#ifndef __H_PxSMVIEWER_INTERFACE_H__
#define __H_PxSMVIEWER_INTERFACE_H__

#include "PxSMVMatrix.h"

#ifdef MakePxSMVCls 
	#define IPxSMVDefDllCls __declspec(dllexport)
	 
#else 
#define IPxSMVDefDllCls __declspec(dllimport)
	 
#endif


namespace PxVCoreAPI
{
	enum ExtStlType
	{
		ExtStlType_RoiMarker = 0,
		ExtStlType_CTMarker,
		ExtStlType_InsertionPathway,
 
	};
	enum TaskType
	{
		kUnknown = (1 << 0),
		kVolumeLoad = (1 << 1),
		kRetrieve = (1 << 2),
		kQuery = (1 << 3),
		kProcess = (1 << 4),
		kConferenceHost = (1 << 6),
		kJoinedSession = (1 << 7), /* number of conference hosts change	*/
		kFileTransfer = (1 << 8), /* number of bytes transferred			*/
		kImageTransfer = (1 << 9),
		kFilming = (1 << 10),
		kCenterLine = (1 << 11), /* new centerline becomes available		*/
		kDirTransfer = (1 << 12), /* number of files transferred so far	*/
		kIdleTimeout = (1 << 13), /* server sends this when it's about to close the client*/
		kPatientList = (1 << 14), /* patient list changed					*/
		kAsyncTaskStatus = (1 << 15), /* asyn Q/R, PE and other stuff			*/

		kDefault = (kVolumeLoad | kRetrieve | kQuery | kProcess | kCenterLine),
		kAll = (1 << 16) - 1
	};
	enum PXSMExeCmdIDInViewer
	{
		ExeCmdIDInViewer_AirwayMode_On		= 100, //airwayMode - On
		ExeCmdIDInViewer_AirwayMode_Off		= 101, //airwayMode - Off
		ExeCmdIDInViewer_AirwayDisplay_On	= 200,  
		ExeCmdIDInViewer_AirwayDisplay_Off	= 201, 
		ExeCmdIDInViewer_DeleteAirwayAxis	= 300, //airwayDelete - delete axis
		ExeCmdIDInViewer_DeleteAirway	, //airwayDelete - delete airway
		ExeCmdIDInViewer_AddAirwayAxis			,
		ExeCmdIDInViewer_GetAirwayAxis	,
		ExeCmdIDInViewer_SetAirwayCenter,
		ExeCmdIDInViewer_AirwayAxisDisplay_On	, //airway axis display On
		ExeCmdIDInViewer_AirwayAxisDisplay_Off	, //airway axis display Off
		ExeCmdIDInViewer_SetAirwayAreaRange,
		ExeCmdIDInViewer_GetAirwayAreaRange,
		//////
		ExeCmdIDInViewer_VR3Displa_On		 , 
		ExeCmdIDInViewer_VR3Displa_Off		 ,
		//
		ExeCmdIDInViewer_ArrowLableDisplay_On,
		ExeCmdIDInViewer_ArrowLableDisplay_Off,
		//	
		ExeCmdIDInViewer_Outline_On		 ,  
		ExeCmdIDInViewer_Outline_Off	 , 

		//
		ExeCmdIDInViewer_AirwayViewerParam,
		//
		ExeCmdIDInViewer_AirwayMode_Update,
		//
		ExeCmdIDInViewer_CheckRequiredData,
		//
		ExeCmdIDInViewer_GetCurrentVRTemplate,
		//
		ExeCmdIDInViewer_GetHaspInfo,
	};
	enum PXSMVLayout
{
	Viewer_Layout_Unknown = 0,
	Viewer_Layout_3D1x1,
	Viewer_Layout_MIP1x1,
	Viewer_Layout_MPR1x1,
	Viewer_Layout_Axial1x1,	
	Viewer_Layout_Coronal1x1,
	Viewer_Layout_Sagittal1x1,
	Viewer_Layout_3D2x2,
	Viewer_Layout_MIP2x2,
	Viewer_Layout_MPR2x2,
	Viewer_Layout_Perspective1x1,
	Viewer_Layout_Minip1x1,
	Viewer_Layout_Raysum1x1,
	Viewer_Layout_Perspective2x2,
	Viewer_Layout_Minip2x2,
	Viewer_Layout_Raysum2x2,

//
	Viewer_Layout_3D1x4,
	Viewer_Layout_3D1x2,
	//
	Viewer_Layout_3DCC,
	Viewer_Layout_3DCR,

};
enum PXSMVImpagePos
{
	Viewer_Image_Pos_Unknown = 0,
	Viewer_Image_Pos_SL,
	Viewer_Image_Pos_3M,
	Viewer_Image_Pos_UL,
	Viewer_Image_Pos_LL,
	Viewer_Image_Pos_LR,
	Viewer_Image_Pos_CPR,
	Viewer_Image_Pos_CSM,
};

enum PXSMVOrientationType
{
	Viewer_Orientation_Unknown = -1,
	Viewer_Orientation_Scan = 0,
	Viewer_Orientation_Axial,
	Viewer_Orientation_Coronal,
	Viewer_Orientation_Sagittal,
	Viewer_Orientation_DoubleOblique
};


enum PXSMVProjectionMode
{
	Viewer_Projection_Unknown = -1,
	Viewer_Projection_VR = 0,
	Viewer_Projection_Perspective,
	Viewer_Projection_MPR,
	Viewer_Projection_MIP,
	Viewer_Projection_Minip,
	Viewer_Projection_RaySum,
	Viewer_Projection_CPR,
	Viewer_Projection_Original,
	Viewer_Projection_CrossSection
};
enum PXSMVImageType
{
	Viewer_ImageType_BMP = 0,
	Viewer_ImageType_JPEG  ,
	Viewer_ImageType_PNG ,
	 
};


#define UID_STR_LEN (68)
#define G_STR_LEN (68)
#define PNAME_STR_LEN (256)

class PXSMVDICOMData
{
public:
	char	m_seriesUID[UID_STR_LEN];
	char	m_studyUID[UID_STR_LEN];
	int		m_ImageCount;
	//for DICOM server
	unsigned short	m_port;
 	//char	m_AEName[20];
	char	m_AETitle[20];
	char	m_IPAddress[16]; /* xxx.xxx.xxx.xxx */
	char	m_hostname[16];	 /* not fully qualified */
};
class PXSMVDICOMInformation
{
public:
	char	m_patientName[PNAME_STR_LEN];
	char	m_patientID[G_STR_LEN];
	char	m_patientGender[G_STR_LEN];
	char	m_patientBirthDate[G_STR_LEN];

	char	m_seriesUID[UID_STR_LEN];
	char	m_studyUID[UID_STR_LEN];
	char	m_studyDate[UID_STR_LEN];
	char	StudyDescription[G_STR_LEN];
};

////////////////////////////
// support data destroy inside DLL
/////
class PxSMVDllBase
{
public:
	virtual void destroy() = 0;
};
 
class PXSMVVolumeData : public PxSMVDllBase
{
public:
	virtual void destroy() = 0;
	virtual bool getVolumeRange(float &minX, float &maxX, float &minY, float &maxY, float &minZ, float &maxZ) const = 0;
	virtual bool getVolumeDimesion(int &sizeX, int &sizeY, int &sizeZ, float &pitchX, float &pitchY, float&pitchZ) const = 0;
	virtual bool getWWWL(int &ww, int &wl) const = 0;
	virtual const PXSMVDICOMData *getDICOMData(void) const = 0;
	virtual  PXSMVDICOMData* getDICOMData(void)  = 0;
	virtual bool getSlopeOffset(float &Slope, float &Offset) const = 0;

};

/////////////
//#29 LUT entry (scene XML like)
class PXSMVLutEntry
{
public:
	int CurveID;
	int WW;
	int WL;
	int LColor[3];//RGB
	int HColor[3];//RGB
	float Opacity;
};
class PXSMVLight
{
public:
	PXSMVLight():
	HeadLight(0)
	, TopLeftLight(0)
	, TopRightLight(0)
	, BottomLeftLight(0)
	, BottomRightLight(0)
	, fIntensity(0.0f)
	, fContrast(0.0f)
	, fSpecular(0.0f)
	, fShininess(0)
	{
		BackgroundColor[0] = BackgroundColor[1] = BackgroundColor[2] = 0;
	}
	int  BackgroundColor[3]; //RGB (0,0,0) - (255,255,255)
	//right position On/Off
	int  HeadLight;
	int  TopLeftLight;
	int  TopRightLight;
	int  BottomLeftLight;
	int  BottomRightLight;
	//	righting property
	float	fIntensity;
	float	fContrast;
	float	fSpecular;
	float	fShininess;
};
template<class T>
class PxSMVDataList : public PxSMVDllBase
{
public:
	virtual void destroy() = 0;
	virtual T *getList() = 0;
	virtual const T *getList() const = 0;
	virtual int getSize() const = 0;
};

PxSMVDataList<PXSMVDICOMInformation>;
PxSMVDataList<PXSMVDICOMData>;
PxSMVDataList<PXSMVLutEntry>;

template<class T>
class Vector2D
{
public:
	T m_vector[2];
};

template<class T>
class Vector3D
{
public:
	T m_vector[3];
	Vector3D operator*(double t) const {
		Vector3D retVector;
		retVector.m_vector[0] = (T)(m_vector[0] * t);
		retVector.m_vector[1] = (T)(m_vector[1] * t);
		retVector.m_vector[2] = (T)(m_vector[2] * t);
		return retVector;
	}
	Vector3D &operator=(const Vector3D &in){
		m_vector[0] = in.m_vector[0];
		m_vector[1] = in.m_vector[1];
		m_vector[2] = in.m_vector[2];
		return *this;
	}
	Vector3D operator+(const Vector3D &a) const{
		Vector3D retVector;
		retVector.m_vector[0] = m_vector[0] +a.m_vector[0] ;
		retVector.m_vector[1] = m_vector[1] + a.m_vector[1];
		retVector.m_vector[2] = m_vector[2] + a.m_vector[2];
		return retVector;
	}
};
template<class T>
class Vector4D
{
public:
	T m_vector[4];
};
//////////////////
PxSMVDataList<Vector2D<float>>;
PxSMVDataList<Vector3D<float>>;
PxSMVDataList<Vector4D<float>>;
/////////////////
class AccessParamBase
{
public:
	virtual ~AccessParamBase(){};
	virtual void assign(const AccessParamBase *inRC) = 0;
};
template<class T> 
class AccessParam : public AccessParamBase
{
public:
	virtual T get(void) const = 0;
	virtual void set(const T &) = 0;
	void assign(const AccessParamBase *inRC) override
	{
		this->set( ((const AccessParam *)inRC)->get());
	}
};
 
///
/// @NAME 
///  PXSMVRenderParam - rendering parameter
/// 
/// @SECTION 
///  rendering parameter inside of DLL, use ClientNvrIf::createRenderParam to create it.
//// rerate to  PXSMVVolumeData
class PXSMVRenderParam : public PxSMVDllBase
{
public:
	///////////////////////////
	/// @brief destroy the parameter 
	/// @return  
	virtual void destroy() = 0;
	virtual void calNormal(const AccessParam<fMat44> *ViewMatrix, AccessParam<fMat13> *Normal) = 0;
	//#29 2020/05/20
	///////////////////////////
	/// @brief update LUT for VR
	/// @param LutEntryList [in] LUT entry list
	/// @return true on success.
	virtual bool updateVRLUT(PxVCoreAPI::PXSMVVolumeData *vdata, const PxSMVDataList<PXSMVLutEntry> *LutEntryList, int bgRGB[3]) = 0;

	///////////////////////////
	/// @brief query LUT entry from parameter
	/// @param LutEntryList [out] LUT entry list, is new created, Must be deleted after use.
	/// @return true on success.
	virtual bool queryVRLUT(PxSMVDataList<PXSMVLutEntry> *&LutEntryList) = 0;

	///////////////////////////
	/// @brief update Light for VR
	/// @param light [in] light property to update
	/// @return true on success.
	virtual bool updateVRLight(const PXSMVLight &light) = 0;

	///////////////////////////
	/// @brief query Light from parameter
	/// @param light [out] return light property 
	/// @return true on success.
	virtual bool queryVRLight( PXSMVLight &light) = 0;

	//#31 2020/05/26
	///////////////////////////
	/// @brief update parameter inside using reration PXSMVVolumeData
	/// @param   
	/// @return true on success.
	virtual bool updateInside(void) = 0;
	////////////
	virtual PXSMVRenderParam* clone(void) const = 0 ;

	////////////
	//ProjectionMode:
	// PXSMVProjectionMode
	AccessParam<int> *ProjectionMode;
	///////////
	//PlaneType:
	//0:kVLIAxial, 1:kVLICoronal, 2:kVLISagittal ,3:kVLIOblique
	//4:kVLIOblique,5:kVLI3DVR,6:kVLIOblique2, 
	//7:kVLIDefaultPlane,8:kVLICPR,
	AccessParam<int> *PlaneType;
	AccessParam<float> *PanX;
	AccessParam<float> *PanY;
	AccessParam<float> *Zoom;
	AccessParam<float> *FitZoom;
	AccessParam<float> *SlicePosition;//FOV: [-Fov,+Fox] (mm),
	AccessParam<int> *WW;
	AccessParam<int> *WL;
	AccessParam<float> *Thickness;
	//
	AccessParam<int> *RenderSize[2]; //0: sizeX, 1: sizeY
	AccessParam<int> *RenderImageType; //ref: PXSMVImageType
	AccessParam<int> *BitsPerPixel;
	AccessParam<int> *ImageQuality; // 0--100  
	//
	AccessParam<fMat44> *RotationViewMat;
	AccessParam<fMat44> *TranslateViewMat;
	AccessParam<fMat13> *SliceNormal;
	//
	AccessParam<float> *MPRRotateAngle;
	AccessParam<int>* WireFrameMode;
	//
//	AccessParam<int>* StencilMode;
};

///
/// @NAME 
///  PXSMVImage - the output of rendering
/// 
/// @SECTION 
///  the rendering result is 3D image 

class  PXSMVImage : public PxSMVDllBase
{
public:
	virtual void destroy() = 0;
	virtual unsigned char* getPixels() = 0;
 
	short			m_width;
	short			m_height;
 
	short			m_bitsPerPixel;
	 
	short			m_renderType;		// Axial, VR etc

	unsigned int	m_imageLength;
 
	// compression parameters
	short			m_imageType;
	 
	short			m_quality;
	 


	short			m_recommendedWindowLevel;
	unsigned short	m_recommendedWindowWidth;

	float			m_position[3];
	float			m_thickness;
	short			m_sliceNumber;
	short			m_pad;
	float			m_xScale;//#32 2020/05/28
	float			m_yScale;
	float			m_cprLength;
	char			m_pixelRepresentation;
	char			m_photometricInterpretation;
	int				m_CrossHairCenter[2];//#34 2020/06/01
};

class ProgressIf : public PxSMVDllBase
{
public:
	virtual void destroy(void) = 0;
	virtual void onProgress(int iTaskRemaining, int iTaskCompleted) = 0;
	virtual void onStart(void) = 0;
	virtual void onFinished(bool successFlag) = 0;
};


///
/// @NAME 
///  ClientRenderModel - render model base
/// 
/// @SECTION 
///   define the render model base
class ClientRenderModel : public PxSMVDllBase
{
public:
	virtual void destroy() = 0;
};

///
/// @NAME 
///  ClientNvrIf - the Client Nvr interface,use CPXSMAPILib::createClientNvrInterface to create it
/// 
/// @SECTION 
///   support the fuctions of Client Nvr interface to connect to imageServer by Nvr.
class ClientNvrIf : public PxSMVDllBase
{
public:
	///////////////////////////
	/// @brief destroy the ClientNvrIf 
	/// @param  
	/// @return  
	virtual void destroy(void) = 0;

	///////////////////////////
	/// @brief connect to imageServer.
	/// @param host [in] host name or IP address. 
	/// @param user [in] user account name.
	/// @param pw [in] user password. 
	/// @return true on success.
	virtual bool ConnectToServer(const char *host, const char *user, const char *pw,const char*shared_sessionID=nullptr) = 0;

	virtual bool getSessionID(char *sessionIDBuffer,int size) = 0;
	///////////////////////////
	/// @brief disconnect from imageServer.
	/// @param 
	/// @return true on success.
	virtual bool Disconnect(void) = 0;

	///////////////////////////
	/// @brief search the patient (study) data
	/// @param inP [in] Describe patient information as search key. 
	/// @param outlist [out] the list of search result.
	/// @return true on success.
	virtual bool SearchPatient(const PXSMVDICOMInformation *inP, PxSMVDataList<PXSMVDICOMInformation>* &outlist) = 0;

	///////////////////////////
	/// @brief list up series data
	/// @param studyInfo [in] study uid.
	/// @param outlist [out] the list of result.
	/// @return true on success.
	virtual bool ListupSeries(const PXSMVDICOMInformation &studyInfo, PxSMVDataList<PXSMVDICOMData>* &outlist) = 0;

	///////////////////////////
	/// @brief load one series 
	/// @param seriesData [in] series uid.
	/// @param Prog [in] pass the progress interface for async mode. nullptr is synchronous mode.
	/// @return volume instance on success, nullptr on failure.if async mode,
	virtual PXSMVVolumeData * LoadData(const PXSMVDICOMData *seriesData, ProgressIf *Prog=nullptr) = 0;
	

	//#27 2020/05/19
	///////////////////////////
	/// @brief rotate render parameter by using two mouse point.
	/// @param param [in] Describe render parameter to update
	/// @param PointDeltaX [in] mouse point delta X 
	/// @param PointDeltaY [in] mouse point delta Y
	/// @return true on success.
	virtual bool RotateVR(PXSMVRenderParam* param, int PointDeltaX, int PointDeltaY) = 0;

	//#30 2020/05/25
	///////////////////////////
	/// @brief create render Param from  VolumeData
	/// @param vdata [in] Volume data .
	/// @param type [in] Orientation type.
	/// @param mode [in] Projection mode.
	/// @return PXSMVRenderParam instance on success. nullptr on failure.
	virtual PXSMVRenderParam *createRenderParam(const PXSMVVolumeData *vdata, 
		PXSMVOrientationType type = Viewer_Orientation_Unknown, PXSMVProjectionMode mode = Viewer_Projection_Unknown) = 0;

	///////////////////////////
	/// @brief do rendering on volume data. 
	/// @param vdata [in] volume data which from  LoadData.
	/// @param param [in] Describe render parameter.
	/// @param ImgOut [out] out put the rendering result as 3D image.
	/// @param pRenderModel [in] model refrenced
	/// @return true on success.
	virtual bool Render(PXSMVVolumeData *vdata, const PXSMVRenderParam* param, PXSMVImage* &ImgOut, ClientRenderModel*pRenderModel=nullptr) = 0;

	///////////////////////////
	/// @brief setup default model parameter. 
	/// @param vdata [in] volume data which from  LoadData.
	/// @param pRenderModel [out] model parameter to setup
	/// @param mask [in]   parameter mask
	/// @return true on success.
	virtual bool setupDefaultModelParam(PXSMVVolumeData *vdata, ClientRenderModel*pRenderModel,int mask=0) = 0;


	///////////////////////////
	/// @brief conver world coord to client coord of MPR. 
	/// @param vdata [in] volume data which from  LoadData.
	/// @param param [in] Describe render parameter.
	/// @param inP3D[in] world 3D point.
	/// @param OutP2D [out] out put 2D Client coord.
	/// @return true on success.
	virtual bool World2ClientMPR(const PXSMVVolumeData *vdata, const PXSMVRenderParam* param, const double inP3D[3], double *OutP2D) const = 0;
	virtual bool getWorld2ClientMPRMatrix(const PXSMVVolumeData *vdata, const PXSMVRenderParam* param,  float *OutMat42) const = 0;

	///////////////////////////
	/// @brief conver the client coord of MPR to world coord  
	/// @param vdata [in] volume data which from  LoadData.
	/// @param param [in] Describe render parameter.
	/// @param inP2D[in] client coord 2D point.
	/// @param OutP3D [out] out put 3D World coord.
	/// @return true on success.
	virtual bool ClientMPR2World(const PXSMVVolumeData *vdata, const PXSMVRenderParam* param, const double inP2D[2], double *OutP3D) const = 0;
	virtual bool getClientMPR2WorldMatrix(const PXSMVVolumeData *vdata, const PXSMVRenderParam* param, float *OutMat33) const = 0;

	/// <summary>
	/// Pickup 3D point from VR
	/// </summary>
	/// <param name="vdata">volume data</param>
	/// <param name="param">render parameter</param>
	/// <param name="inP2D">input screen 2D point [-1,1]</param>
	/// <param name="OutP3D">output Voxel 3D Point</param>
	/// <param name="iAlphaTermination">pick level</param>
	/// <returns></returns>
	virtual bool pickup3DPoint(const PXSMVVolumeData* vdata, const PXSMVRenderParam* param, const double inP2D[2], double* OutP3D, float iAlphaTermination=0.6f) const = 0;

	///////////////////////////
	/// @brief conver world coord to client coord of MPR. 
	/// @param vdata [in] volume data which from  LoadData.
	/// @param param [in] Describe render parameter.
	/// @param inP3D[in] world 3D point.
	/// @param OutP2D_x,OutP2D_y [out] out put 2D Client coord.
	/// @return true on success.
	virtual bool GetMPRCenter(const PXSMVVolumeData *vdata, const PXSMVRenderParam*paramMPR[3] ,  double *OutputP3D  ) const = 0;

	//_added_GenericPolygon_as_implant
	virtual bool LoadGenericPolygonFile(const PXSMVDICOMData* seriesData, const char* fileName) = 0;
	virtual bool UnloadGenericPolygonFile(const PXSMVDICOMData* seriesData, const char* fileName) = 0;
	virtual bool ShowGenericPolygonFile(const PXSMVDICOMData* seriesData, const char* fileName,bool bShow) = 0;
	virtual void EnableGenericPolygon(bool b) = 0;
	/// <summary>
	/// insert GenericPolygon Ojbect
	/// </summary>
	/// <param name="vdata"> volume data </param>
	/// <param name="Point3D"> world coord point</param>
	/// <param name="Vec"> define the vector of object</param>
	/// <param name="ObjType"> object type</param>
	/// <returns> objecto ID</returns>
	virtual int  AddGenericPolygonObject(const PXSMVVolumeData* vdata, double Point3D[3], double Vec[3], int ObjSizeNo = 0, int ExtStlType =0) = 0;
	virtual double  GetGenericPolygonObjectDia(int ObjSizeNo = 0, int ExtStlType = 0) = 0;
	virtual void  RemoveGenericPolygonObject(int ObjID, bool remove_model=false) = 0;
	virtual void  SaveGenericPolygonObjectToStl(const PXSMVVolumeData* vdata,int ExtStlType =0) = 0;
	///////////////////////////
	/// @brief Task cancellation . 
	/// @param type [in] which task to cancel.
	/// @return true on success.
	virtual bool CancelTask(TaskType type) = 0;

	///////////////////////////
	/// @brief get last error from NVR.
	/// @param  
	/// @return error code.
	virtual int GetLastError(void) = 0;
};



///
/// @NAME 
///  ClientMPRModel - the Client MPR model, use CPXSMAPILib::createClientMPRModel to create it
/// 
/// @SECTION 
///   support the fuctions of MPR model which there are 3 plane Axial/Coronal/Sagittal
class ClientMPRModel : public ClientRenderModel
{
public:
	enum MPR_Plane
	{
		MPR_Axial = 0,
		MPR_Coronal,
		MPR_Sagittal,
	};
	///////////////////////////
	/// @brief destroy the ClientMPRModel 
	/// @param  
	/// @return  
	virtual void destroy(void) = 0;

	///////////////////////////
	/// @brief Setup MPR model. 
	/// @param paramAxial [in] render parameter of Axial plane
	/// @param paramCoronal [in] render parameter of Coronal plane
	/// @param paramSagittal [in] render parameter of Sagittal plane
	/// @return true on success.
	virtual bool SetupMPR(PXSMVRenderParam* paramAxial, PXSMVRenderParam* paramCoronal, PXSMVRenderParam* paramSagittal ) = 0;

	///////////////////////////
	/// @brief rotate MPR model. 
	/// @param angle [in] rotation angle
	/// @param axis [in] rotate angle on this plane 
	/// @return true on success.
	virtual bool rotateAngle(float angle, ClientMPRModel::MPR_Plane axis) = 0;
};


///
/// @NAME 
///  ClientCPRModel - the Client CPR model, use CPXSMAPILib::createClientCPRModel to create it
/// 
/// @SECTION 
///   support the fuctions of CPR model which there are VR/Pano/CPRs 
class ClientCPRModel : public ClientRenderModel
{
public:
	enum SetpDefaultMask {
		SetpDefaultMaskNaviWWWL				= 0x0001,
		SetpDefaultMaskCPRWWWL				= 0x0002,
		SetpDefaultMaskCrossSectionWWWL		= 0x0004,
		SetpDefaultMaskCurve				= 0x0008,
	};
	enum CurveNodeType {
		CurveNode_Unknown = 0,
		CurveNode_Curve,
		CurveNode_Linear,
		CurveNode_Spline,
	};
	///////////////////////////
	/// @brief destroy the ClientCPRModel 
	/// @param  
	/// @return  
	virtual void destroy(void) = 0;

	///////////////////////////
	/// @brief Setup Render Parameter for CPR model. 
	/// @param paramNavi [in] render parameter of Navi plane
	/// @param paramCPR [in] render parameter of  CPR (Pano) plane
	/// @param paramCrossSection [in] render parameter list of CrossSection
	/// @return true on success.
	virtual bool setRenderParamNavi(PXSMVRenderParam* paramNavi) = 0;
	virtual bool setRenderParamCPR(PXSMVRenderParam* paramCPR) = 0;
	virtual bool setRenderParamCrossSection(PXSMVRenderParam**paramCrossSection, int CrossSectionSize) = 0;
	//
	virtual PXSMVRenderParam* getRenderParamNavi(void) = 0;
	virtual PXSMVRenderParam* getRenderParamCPR(void) = 0;
	virtual int getRenderParamCrossSectionSize(void) = 0;
	virtual PXSMVRenderParam* getRenderParamCrossSection(int no) = 0;
#if 0
	///////////////////////////
	/// @brief Setup CPR curve . 
	/// @param nodes [in] curve nodes
	/// @param spline [in] spline flag;
	/// @return true on success.
	virtual bool SetupCurve(const PxSMVDataList<float> &nodes,int spline) = 0;
#endif

	///////////////////////////
	/// @brief query CPR Curve parameter
	/// @param CurveParam [out] return CurveParam, is new created, Must be deleted after use.
	/// @param outType  [out] return Curve type
	/// @param CurveIndex  [in] select curve index as , -n,...,-1,0,1,...n
	/// @return true on success.
	virtual bool queryCurve(PxSMVDataList<Vector3D<float>> *&CurveParam, CurveNodeType &outType, int CurveIndex, bool bCtrlNode = false) const = 0;

	///////////////////////////
	/// @brief update CPR Curve parameter
	/// @param CurveNodes [in] CPR Curve Curve Nodes to update
	/// @param Type [in] define the type of Curve Nodes 
	/// @param targetSize [in] interpolation size when Type is CurveNode_Linear or CurveNode_Spline
	/// @return true on success.
	virtual bool updateCurve(const Vector3D<float> *CurveNodes, int size, CurveNodeType Type = CurveNode_Curve,int targetSize=0) = 0;

	///////////////////////////
	/// @brief query Cross Section nodes
	/// @param Nodes [out] return Cross Section nodes, is new created, Must be deleted after use.
	/// @return true on success.
	virtual bool queryCrossSectionNodes(PxSMVDataList<Vector3D<float>> *&Nodes) const = 0;
	//
	virtual bool setCrossSectionNo(int no) = 0;
	virtual bool setCrossSectionPitch(float pitch) = 0;
	virtual bool setNumberOfCPR(int size) = 0;
	virtual bool setCPRNo(int size) = 0;
	virtual bool setCPRPitch(float pitch) = 0;
	virtual bool setTotalCrossSection(int size) = 0;
 //
	virtual int getCrossSectionNo(void) const = 0;
	virtual float getCrossSectionPitch(void) const = 0;
	virtual int getNumberOfCPR(void) const = 0;
	virtual int getCPRNo(void) const = 0;
	virtual float getCPRPitch(void) const = 0;
	virtual int getTotalCrossSection(void) const = 0;
	 
};

///
///
/// @NAME 
///  SMViewerCore LIb
///   
/// @SECTION 
///   
///

class IPxSMVDefDllCls  CPXSMAPILib
{
public:
	CPXSMAPILib(void);
	~CPXSMAPILib(void);
  
	static void setupLogFolder(const char *folder,int logLevel=0,int stdout_log=0);
	static void getLogFolder(char *folderBuffer,int bufferSize, int &logLevel , int &stdout_log);
	static void getVersion(unsigned int &major, unsigned int &minor, unsigned int &sub, unsigned int &micro, char *pBufGitHashTag, int Bufsize);
	//#36 2020/07/13 get Nvr version
	static void getVersionNvr(unsigned int &major, unsigned int &minor, unsigned int &sub, unsigned int &micro, char *pBufGitHashTag, int Bufsize);

	 
	static ClientNvrIf *createClientNvrInterface(void);
	static ClientMPRModel *createClientMPRModel(void);
//#30 moved to ClientNvrIf 2020/05/25
// 	static PXSMVRenderParam *createRenderParam(PXSMVOrientationType type = Viewer_Orientation_Unknown, PXSMVProjectionMode mode = Viewer_Projection_Unknown);
	static PxSMVDataList<PXSMVLutEntry> *createLutEntryList(int size);

	static ClientCPRModel *createClientCPRModel(void);
};


////////////
} //PxVCoreAPI

#endif //__H_PxSMVIEWER_INTERFACE_H__