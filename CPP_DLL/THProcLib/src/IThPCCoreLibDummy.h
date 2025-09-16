#ifndef TH_ITH_PCCORED_LIB_DUMMY_H
#define TH_ITH_PCCORED_LIB_DUMMY_H
 

namespace ThPCCoreLibDummy
{
#define  kVR_AE ( 16 + 1) 
#define  kVR_AS ( 4 + 1)
#define  kVR_AT ( 4 + 1)
#define  kVR_CS ( 16 + 1)
#define  kVR_DA ( 17 + 1)
#define  kVR_DS ( 16 + 1)
#define  kVR_DT ( 26 + 1)
#define  kVR_FL ( 4 + 1)
#define  kVR_FD ( 8 + 1)
#define  kVR_IS ( 12 + 1)
#define  kVR_LO ( 64 + 1)
#define  kVR_LT ( 10240 + 1)
#define  kVR_PN ( 320 + 1)
#define  kVR_SH ( 16 + 1)
#define  kVR_SL ( 4 + 1)
#define  kVR_SS ( 2 + 1)
#define  kVR_ST ( 1024 + 1)
#define  kVR_TM ( 16 + 1)
#define  kVR_UI ( 64 + 1)
#define  kVR_UL ( 4 + 1)
#define  kVR_US ( 2 + 1)

#define  kMediaLabel ( 16+1)
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
class UDICOMStudy_Def
{
public:
	char	m_studyInstanceUID[kVR_UI];
	wchar_t m_patientName[kVR_PN];
	wchar_t m_patientID[kVR_LO];
	wchar_t	m_patientBirthDate[kVR_DA];
	wchar_t	m_patientSex[kVR_CS];
	long	m_patientAge;
	wchar_t	m_studyDate[kVR_DA];
	wchar_t	m_studyTime[kVR_TM];
	wchar_t m_accessionNumber[kVR_SH];
	wchar_t m_studyID[kVR_SH];
	wchar_t m_radiologistName[kVR_PN];
	wchar_t m_referringPhysiciansName[kVR_PN];
	wchar_t m_modalitiesInStudy[kVR_LO];
	wchar_t m_studyDescription[kVR_LO];
	long	m_numberOfStudyRelatedSeries;
	long	m_numberOfStudyRelatedInstances;
	char	m_characterSet[257];
	int		m_status;
	int     m_procStatus;
};

}
#endif //TH_ITH_PCCORED_LIB_DUMMY_H