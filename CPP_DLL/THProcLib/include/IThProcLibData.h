#ifndef TH_PROC_OUT_LIB_DATA__H
#define TH_PROC_OUT_LIB_DATA__H

	enum eThLibCmd //���s�R�}���h�̒�`
	{
		ThLibCmd_Unknown = -1,
		ThLibCmd_Exe_ImportData = 0,
		ThLibCmd_Exe_OutputData = 1,
		ThLibCmd_Exe_DeleteData = 2,
		ThLibCmd_Exe_AISegmentator = 3,
		ThLibCmd_Exe_USB_Output = 4,
		ThLibCmd_Exe_AsyncCmd_Cancel = 5,//#205_Cancel_import_data
		//-----------
		ThLibCmd_Query_CTSlice = 10,
		ThLibCmd_Query_CTMarker2D ,	//11
		ThLibCmd_Query_CTMarker3D,	//12
		ThLibCmd_Query_CTRoi2D,		//13
		ThLibCmd_Query_CTRoi3D,		//14
		//-----------
		//#29_command_added
		ThLibCmd_CT_Marker_Clear = 20,	//20
		ThLibCmd_CT_Marker_Add,			//21
		ThLibCmd_CT_Marker_Final,		//22
		//-----------
		ThLibCmd_ROI_Clear = 30,		//30
		ThLibCmd_ROI_Add,				//31
		ThLibCmd_ROI_Erase  ,			//32
		ThLibCmd_ROI_Final,				//33
		ThLibCmd_ROI_SetPen,			//34
		//-----------
		ThLibCmd_Output_Remove = 40,	//40
		ThLibCmd_Output_Add,			//41
	 
		//-----------
		ThLibCmd_Get_StudyPath=50,    
		ThLibCmd_Get_DicomImageInfo,	//51        // th_lib_get_image_info�Ɏw��A/ThProcLib_DicomImageInfo
		ThLibCmd_Get_MeshPath ,			//52 //#97_get_mesh_folder

		ThLibCmd_Query_Setting = 99,
		//-----------
		//#43_rendering_command
		ThLibCmd_Render_Start		= 120,
		ThLibCmd_Render_Create_GC,	//121
		ThLibCmd_Render_Query_GC,	//122
		ThLibCmd_Render_Vol3D,		//123
		ThLibCmd_Render_Image2D,	//124
		ThLibCmd_Render_Load_Vol3D,	//125
		ThLibCmd_Render_Load_3DModel,//126   //#103_load3DModel_command
		ThLibCmd_Render_End,
		ThLibCmd_Render_Release_Start = 130,	//
		ThLibCmd_Release_GC,			//131
		ThLibCmd_Release_Vol3D,			//132
		ThLibCmd_Release_Image2D,		//133
		ThLibCmd_Render_Op,				//134
		ThLibCmd_Render_Release_End,
		//
		ThLibCmd_Check_Image_Server		= 200,//#137_check_image_server
		
		ThLibCmd_End,
	};
	enum eThLibQueryList //�f�[�^�ꗗ�̎�ނ̒�`
	{
		ThLibQueryList_Unknown = -1,
		ThLibQueryList_CTData = 0,
		ThLibQueryList_OutputData = 1,
	};
	enum eThLibCmdType //�R�}���h���s���
	{ 
		ThLibCmdType_Sync = 0,  //�������s�A�R�}���h������҂B
		ThLibCmdType_Async = 1, //�񓯊����s�A�R�}���h������҂����A�ʓrth_lib_get_status�Ŏ��s�i�����m�F����B
	};
	enum eThLibCmdStatus //�R�}���h���sStatus
	{
		ThLibCmdStatus_Failed = -1,
		ThLibCmdStatus_Succeeded = 0,
		ThLibCmdStatus_Progress = 1,
	};
	enum eThLibProcStatus //�f�[�^����Status
	{
		ThLibCmdProc_Unknown = -1,
		ThLibCmdProc_No_Roi = 0,
		ThLibCmdProc_Roi = 1,
	};
	enum eThLibOutputType //�������ꂽOutput�t�@�C���̎��
	{
		ThLibCmdOutput_Unknown = -1,
		ThLibCmdOutput_Xreal = 0,
		ThLibCmdOutput_iPhone = 1,
		ThLibCmdOutput_Xreal_iPhone = 2,
	};
	/// <summary>
	/// //#43_rendering_command
	/// </summary>
	enum eSeriesType //Study�ɂ���x��^�c�u�f�[�^
	{
		ThLibSeriesType_Unknown = -1,
		ThLibSeriesType_Haiya = 0,
		ThLibSeriesType_Juukaku = 1,

	};
	enum eDisplayMode //�\�����[�h�̎w�� �r�b�g�̑g�ݍ��킹
	{
		ThLibDisplayMode_Unknown = 0,
		ThLibDisplayMode_Volume = 1,
		ThLibDisplayMode_Polygon = 2,
		ThLibDisplayMode_CTMarker = 4,//#101,#102
	};
	enum eDisplayParts //�\���p�[�c�̎w�� �r�b�g�̑g�ݍ��킹
	{
		ThLibDisplayParts_Unknown = 0,
		ThLibDisplayParts_ROI = 1,
		ThLibDisplayParts_Body = 2,
		ThLibDisplayParts_Lung = 4,
		ThLibDisplayParts_Heart = 8,
		ThLibDisplayParts_Other = 256,
	};
	enum eRenderCmd  //�����_�����O�R�}���h�̒�`
	{
		ThLibRenderCmd_Unknown = 0,
		ThLibRenderCmd_2D , // 1,
		ThLibRenderCmd_3D, // 2,
		ThLibRenderCmd_Setup, // 3,//�p�����[�^�ݒ�̂݁B
		ThLibRenderCmd_ProcMouse,    // 4,//�}�E�X�C�x���g����������B�摜��Ԃ���������B
		ThLibRenderCmd_Change_Case,  // 5,
		//#160_added_new_Setup_type
		ThLibRenderCmd_Setup_Series, // 6 for Juukaku/Haiya
		ThLibRenderCmd_Setup_3D_Obj, // 7 for �RD���F�\��(Preset)
	};
	enum ePenType  //�֐S�̈摀�쎞�Ƀy���̑���
	{
		ThLibPenType_Unknown = 0,
		ThLibPenType_Pen1 = 1,  //�ōׂ�
		ThLibPenType_Pen2,
		ThLibPenType_Pen3,
		ThLibPenType_Pen4,     //�ő���
	};
	enum eMouseEvent  //�}�E�X�C�x���g�̒�`
	{
		ThLibMouseEvent_Unknown = 0,
		ThLibMouseEvent_MOUSEMOVE,
		ThLibMouseEvent_LBUTTONDOWN,
		ThLibMouseEvent_LBUTTONUP,
		ThLibMouseEvent_LBUTTONDBLCLK,
		ThLibMouseEvent_RBUTTONDOWN,
		ThLibMouseEvent_RBUTTONUP,
		ThLibMouseEvent_RBUTTONDBLCLK,
		ThLibMouseEvent_MOUSEWHEEL,
		ThLibMouseEvent_User = 20,
		ThLibMouseEvent_CT_Marker_2D,
		ThLibMouseEvent_CT_Marker_3D,
		ThLibMouseEvent_ROI_Pen,
		ThLibMouseEvent_ROI_Eraser,
		ThLibMouseEvent_3D_Rotation,
	};
	//#123_request_next_render_cmd_from_DLL
	enum eRequestRenderCmd  //DLL����Render�R�}���h�̗v��
							//�r�b�g�̑g�ݍ��킹
	{
		ThLibRRCmd_Unknown = 0,
		ThLibRRCmd_2D_1 = 1,
		ThLibRRCmd_2D_2 = 2,
		ThLibRRCmd_2D_3 = 4,
		ThLibRRCmd_3D	= 8,
	};
	/// <summary>
	/// �f�[�^�ꗗ�̍��ڃf�[�^
	/// </summary>
	struct ThProcLib_PatientInfo{
		wchar_t PatientName[128] ;
		wchar_t PatientID[64] ;
		wchar_t Birthday[16];// YYYY/MM/DD
		int Age;
		wchar_t Gender[8];
		wchar_t StudyDate[16];// YYYY/MM/DD
		wchar_t StudyTime[16];// HH:MM:SS
		float SliceThickness;//unit : mm
//		wchar_t CTMode[16];
		int ProcessStatus;//�֐S�̈�@0: �󔒁A 1: �Z ref: eThLibProcStatus
		wchar_t UpdateDate[16];// YYYY/MM/DD
		wchar_t UpdateTime[16];// HH:MM:SS
		int StudyPattern; //�Ǘ�
		wchar_t StudDescription[64];
		wchar_t StudyUID[64];//�f�[�^���ʗp�A�\�����Ȃ�
		int OutputType;////�������ꂽOutput�t�@�C���̎�ށB�@ref: eThLibOutputType
		int CheckDig; //check bit, always  1234;
	};
	/// <summary>
	/// �摜�f�[�^�̒�`(read from Dicom)
	/// </summary>
	struct ThProcLib_DicomImageInfo {
		wchar_t StudyUID[64]; 
		int sizeX;
		int sizeY;
		int sizeZ;
		int WindowWidth;
		int WindowCenter;
		int CheckDig; //check bit, always  2345;
	};
	/// <summary>
	/// ThProcLib���p�����^�[
	/// </summary>
	struct ThProcLib_Env {
		int LogLevel;
		wchar_t ImportFolder[256]; //#181_added_import_path
		wchar_t HomeFolder[256];
		wchar_t OutputFolder[256];
		wchar_t DcmRawData[256];
		int ThProLibDummy;//0: system, 1: dummy  
		//#191_Model_Version
		wchar_t ModelVers[512];//���s���܂܂��A�����s�̕����񂻂̂܂ܕ\��
		//added 2025/08/26
		wchar_t reserved_str1[256]; 
		wchar_t reserved_str2[256]; 
		int reserved_data1;
		float reserved_data2;
	};
	struct ThProcLib_Cmd {
		int cmdID;// ���s�R�}���h�B�@ref: eThLibCmd
		wchar_t strParam1[256];
		wchar_t strParam2[256];
		int		intParam;
		float	floatParam;
	};
	/// <summary>
	/// �R�}���h���s�̕Ԓl
	/// </summary>
	struct ThProcLib_CmdOutput {
		int cmdType; //�R�}���h���s����鎞�̎�ށADLL�����Ō��߂�B�i�����^�񓯊��jref: eThLibCmdType 
		int cmdID;   // ���s���̃R�}���h�B�@ref: eThLibCmd
		int cmdUID;  //�R�}���h���s���ɐ���������ID
		int cdmExtDataSize;  //�R�}���h�Ԓl�̊g���f�[�^�B0: �g���f�[�^�Ȃ��A>0: �g���f�[�^����(�o�C�g���j�A�ʓr�R�}���hth_lib_get_data�Ō��ʃf�[�^���擾����B
		wchar_t strParam1[256];
		wchar_t strParam2[256];
		int		intParam;
		float	floatParam;
	};
	struct ThProcLib_CmdStatus {
		int cmdID;	//���s���̃R�}���h�̑I���i���������̏ꍇ�j ref: eThLibCmd
		int cmdStatus; //�R�}���h���sStatus�Bref: eThLibCmdStatus 
		int cmdProgress;//0,1,...,100
		int dataID;
		wchar_t strParam1[256];//added 2025/05/20 for studyUID
		wchar_t errMessage[256];
	};
	/// <summary>
	///#43_rendering_command
	/// ThProcLib_Vol3DData
	/// DICOM �t�@�C�����烁�����Ƀ��[�h���ꂽVolume�f�[�^�̏��
	/// </summary>
	struct ThProcLib_Vol3DData {
		int volID;//���������A�Ǘ�
		wchar_t StudyUID[64];
		int sizeX;
		int sizeY;
		int sizeZ;
		float ptichX;//mm
		float ptichY;//mm
		float ptichZ;//mm
		//--------
		int reserved_int1;
		int reserved_int2;
		int reserved_int3;
		float reserved_float1;
		float reserved_float2;
		float reserved_float3;
		wchar_t reserved_char[64];
	};
	/// <summary>
	///#43_rendering_command
	/// ThProcLib_Image2DData
	/// �摜�\���iVolume�f�[�^���烌���_�����O���ʁj
	/// </summary>
	struct ThProcLib_Image2DData {
		int imageID;//���������A�Ǘ�
		int sizeX;
		int sizeY;
		float ptichX;//mm
		float ptichY;//mm
		int format; //ref: eImageFormat
		//--------
		//#123_request_next_render_cmd_from_DLL
		int next_render_cmds; //ref: eRequestRenderCmd  //�r�b�g�̑g�ݍ��킹
		int reserved_int1;
		int reserved_int2;
		float reserved_float1;
		float reserved_float2;
		wchar_t reserved_char[64];
	};
	enum eImageFormat  
	{
		ImageFormat_Unknown = -1,
		ImageFormat_8Gray = 0,
		ImageFormat_16Gray ,	//1
		ImageFormat_24RGB ,		//2
		ImageFormat_32ARGB,		//3
	};
	/// <summary>
	///#43_rendering_command
	/// ThProcLib_RenderGC
	/// �����_�����O�̃p�����[�^�w��
	/// ���̕\���E�B���h�E�̈�Ɗ֘A�t���A�Ǘ�����B
	/// </summary>
	struct ThProcLib_RenderGC {
		int render_gc_id; //���������A�Ǘ�
		//define rendering command
		int render_cmd_major;//ref:eRenderCmd
		int render_cmd_minor;
		//draw area of window
		//��ɏo�̓E�B���h�E�̕`��A���A�ƈ�v����!
		int target_sizeX;
		int target_sizeY;
		//reander data
		int series_type; //ref: eSeriesType
		//render parameter
		float zoom;
		float panX;
		float panY;
		float slice_position;//slice number
		int   display_parts;//ref: eDisplayParts
		int   study_case;	//TBD: �a��
		int   render_preset;//for 3D Lut
		//window leve/window width
		int window_levle;
		int window_width;
		//
		int display_mode;//ref: eDisplayMode //�\�����[�h�̎w�� �r�b�g�̑g�ݍ��킹

		//processing mouse event
		int mouse_type;//ref: eMouseEvent
		int mouse_posX;
		int mouse_posY;
		//--------
		int reserved_int1;
		int reserved_int2;
		float reserved_float1;
		float reserved_float2;
		wchar_t reserved_char[64];
	};
#endif //TH_PROC_OUT_LIB_DATA__H