#ifndef TH_ITH_PCCORED_LIB__H
#define TH_ITH_PCCORED_LIB__H

#pragma warning (disable: 4305)
#include "IThDcmData.h"
 
#ifdef MakeThPCCoreLib 
	#define IThPCCoreDefDllCls __declspec(dllexport)
#else 
	#define IThPCCoreDefDllCls __declspec(dllimport)
#endif

namespace ThPCCoreLib
{
 
/// <summary>
/// �����̃C�x���g�N���X
/// �g�p��������������āA
/// SetProcEvent�ɃZ�b�g����ƁA������LOG�o�͂ł���
/// </summary>
class ProcEvent
{
public:
	virtual bool progress(float rate)=0;
	virtual bool procMsg(const char *msg)=0;
	virtual bool doIdle(void)=0;
};
enum eCoreLibThreadType
{
	eCoreLibThread_Unknown = 0,
	eCoreLibThread_CDImport,
	eCoreLibThread_AiSegWatcher,
};
enum eProcStatus
{
	eProcStatus_Init = 0,
	eProcStatus_AiSeg = 1,
	eProcStatus_CTMarker = 2,
	eProcStatus_Roi = 4,
	eProcStatus_Xreal = 8,
	eProcStatus_iPhone = 16,
};
/// <summary>
/// 
/// </summary>
class ThPCCoreLibIf
{
public:
	class ThPCCoreLibConfig
	{
	public:
		int m_type;
		wchar_t m_home_folder[256];
		wchar_t m_CD_Drive[128];
		wchar_t m_db_file[256];
		int m_3DModelAlpha;
		int m_DegRotation3D;//#17_XYRotation ����ʗp��90����],�\���ɂRD��]�̊p�x�w��B�i�P�ʁ��j
		//#23_added_thoracentes_needl
		int		m_NeedleOn;		//���h���[�g�̕\���E��\��
		int		m_NeedleLength; //���h�j�̒���
		int		m_NeedleR;		//���h�j�̔��a
		int reserved_int1;
		int reserved_int2;
		float reserved_float1;
		float reserved_float2;
		wchar_t reserved_char[64];
	};	
	 
virtual void selfTest(void)=0;
/// <summary>
/// �����C�x���g�̃Z�b�g
/// </summary>
/// <param name="event"></param>
virtual void setProcEvent(ProcEvent *event)=0;
  
/// <summary>
/// Home�t�H���_�̐ݒ�
/// </summary>
/// <param name="in_home_folder"></param>
virtual void setupHomeFolder(const wchar_t* in_home_folder) = 0;
//virtual void setupDcmFolder(const wchar_t* in_cmd_folder) = 0;

/// <summary>
/// ���C�u�����̏������A�C���X�^���X�̍쐬
/// </summary>
/// <param name="">true:�����Afalse:���s</param>
/// <returns></returns>
virtual bool init(void) = 0;
virtual bool cancelCmd(eCoreLibThreadType cmd) = 0;
virtual bool getConfig(ThPCCoreLibConfig &config) = 0;
virtual bool importCDROMData(const wchar_t*folder_name=nullptr, bool async = true) = 0;
virtual const char*getCurrentStudyUID(void) const = 0;
virtual bool runAISegmentator(const char* studyUID = nullptr, bool async = true) = 0;
virtual bool outputMesh(const char* studyUID = nullptr, bool glbOnly = true, bool async = true) = 0;
//#13_Setup_CT_Marker_Manual
virtual bool setupCTMarker(double P3D_1[3], double P3D_2[3], double P3D_3[3], const char* studyUID = nullptr, bool async = true) = 0;
virtual bool getProgress(eCoreLibThreadType type,int &completed, float &progress) = 0;
virtual ThDcmMgrLib::DcmStudyList* getStudyList(void) const = 0;
virtual bool getStudyData(const char* studyUID, ThDcmMgrLib::UDICOMStudy_Def& out) const = 0;
virtual ThDcmMgrLib::DcmSeriesList* getSeriesList(const char* studyUID) const = 0;
virtual bool getSeriesData(const char* studyUID, const char* seriesUID, ThDcmMgrLib::UDICOMSeries_Def& out) const = 0;
virtual bool getDicomFolder(const char* studyUID, ThDcmMgrLib::UDICOMFolder_Def& folder, bool bIsHaiya = false) const = 0;
//#8_get_mesh_folder_name
virtual bool getStudyDateTimeUIDFolder(const char* studyUID, ThDcmMgrLib::UDICOMFolder_Def& outFolder) const = 0;
virtual bool getMeshFolder(const char* studyUID, eProcStatus modelType, ThDcmMgrLib::UDICOMFolder_Def& folder) const = 0;
virtual bool getStlFolder(const char* studyUID, ThDcmMgrLib::UDICOMFolder_Def& folder) const = 0;
virtual bool getCTMarkerFolder(const char* studyUID, ThDcmMgrLib::UDICOMFolder_Def& folder) const = 0;
virtual bool changeDBProcStatus(const char* studyUID, const char* seriesUID, int status) = 0;
virtual bool changeProcStatus(const char* studyUID, int procStatus) = 0;
virtual bool deleteStudy(const char* studyUID) = 0;
//#14_XREAL_iPhone_different_mesh_folder
virtual bool outputModelFile(const char* studyUID, eProcStatus modelType) = 0;
virtual void Destroy(void)=0;
virtual void setupDebug(bool onOff) = 0;
virtual void setupLogFile(const wchar_t* logFile) = 0;
 
};
template<class T>
struct deleter_THLibIf {
	void operator()(T* ptr_) {
		ptr_->Destroy();
	}
};
/// <summary>
/// CPCCoreLib DLL���C�u�����N���X
/// </summary>
class IThPCCoreDefDllCls CThPCCoreLib
{
public:
	CThPCCoreLib(void);
	~CThPCCoreLib(void);
	/// <summary>
	/// ThDcmMgrIf ���b�V���o�͏����C���^�t�F�[�X�̍쐬
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	static ThPCCoreLibIf*createThPCCoreLibIf(void);
 
};
/// <summary>
/// �X�}�[�g�|�C���g�̒�`
/// </summary>
#define SmartPtr_PCCoreLibIf  std::unique_ptr < ThPCCoreLib::ThPCCoreLibIf, ThPCCoreLib::deleter_THLibIf<ThPCCoreLib::ThPCCoreLibIf >>

}
#endif //TH_ITH_DCM_MGR_LIB__H