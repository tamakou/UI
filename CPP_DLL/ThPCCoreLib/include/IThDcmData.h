#ifndef TH_ITH_DCM_DATA__H
#define TH_ITH_DCM_DATA__H
 
namespace ThDcmMgrLib
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
	class DICOMPatient_Def
	{
	public:

		wchar_t m_patientName[kVR_PN];
		wchar_t m_patientID[kVR_LO];
		wchar_t	m_patientBirthDate[kVR_DA];
		wchar_t	m_patientSex[kVR_CS];
		char	m_characterSet[257];
	};

	class DICOMStudy_Def
	{
	public:
		char	m_studyInstanceUID[kVR_UI];
		char m_patientName[kVR_PN];
		char m_patientID[kVR_LO];
		char	m_patientBirthDate[kVR_DA];
		char	m_patientSex[kVR_CS];
		long	m_patientAge;
		char	m_studyDate[kVR_DA];
		char	m_studyTime[kVR_TM];
		char m_accessionNumber[kVR_SH];
		char m_studyID[kVR_SH];
		char m_radiologistName[kVR_PN];
		char m_referringPhysiciansName[kVR_PN];
		char m_modalitiesInStudy[kVR_LO];
		char m_studyDescription[kVR_LO];
		long	m_numberOfStudyRelatedSeries;
		long	m_numberOfStudyRelatedInstances;
		char	m_characterSet[257];
		int		m_status;

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

	class DICOMSeries_Def
	{
	public:

		char	m_studyInstanceUID[kVR_UI];
		char	m_seriesInstanceUID[kVR_UI];
		long	m_seriesNumber;
		char m_seriesDescription[kVR_LO];
		char	m_modality[kVR_CS];
		char	m_bodyPartExamined[kVR_CS];
		char	m_viewPosition[kVR_CS];
		long	m_numberOfSeriesRelatedInstances;
		char m_stationName[kVR_SH];
		int		m_offlineFlag;
		int		m_IsQRData;
		long	m_seriesModifyTime;
		long	m_seriesHoldToDate;
		int		m_status;

		char	m_seriesDate[kVR_DA];
		char	m_seriesTime[kVR_TM];
		char	m_manufacturer[33];
		float	m_FLCScaleFactor;
		char	m_remoteAETitle[kVR_AE];
		char	m_characterSet[257];
	};

	class UDICOMSeries_Def
	{
	public:

		char	m_studyInstanceUID[kVR_UI];
		char	m_seriesInstanceUID[kVR_UI];
		long	m_seriesNumber;
		wchar_t m_seriesDescription[kVR_LO];
		wchar_t	m_modality[kVR_CS];
		wchar_t	m_bodyPartExamined[kVR_CS];
		wchar_t	m_viewPosition[kVR_CS];
		long	m_numberOfSeriesRelatedInstances;
		wchar_t m_stationName[kVR_SH];
		int		m_offlineFlag;
		int		m_IsQRData;
		long	m_seriesModifyTime;
		long	m_seriesHoldToDate;
		int		m_status;

		wchar_t	m_seriesDate[kVR_DA];
		wchar_t	m_seriesTime[kVR_TM];
		wchar_t	m_manufacturer[33];
		float	m_FLCScaleFactor;
		wchar_t	m_remoteAETitle[kVR_AE];
		char	m_characterSet[257];
		//
		short m_rows;
		short m_columns;
		int m_windowWidth;
		int m_windowCenter;
		float m_SliceThickness;//#11_added_SliceThickness_to_DB_SeriesLevel
		int   m_procStatus;
	};
	class UDICOMFolder_Def
	{
	public:
		wchar_t m_dicom_folder[1024];

	};
	class DcmStudyList {
	public:
		virtual int getSize(void) = 0;
		virtual bool getStudy(int no, UDICOMStudy_Def& outStudy) = 0;
		virtual void Destroy(void) = 0;
	};
	class DcmSeriesList {
	public:
		virtual int getSize(void) = 0;
		virtual bool getSeries(int no, UDICOMSeries_Def& outSeries) = 0;
		virtual void Destroy(void) = 0;
	};
}
#endif //TH_ITH_DCM_DATA__H