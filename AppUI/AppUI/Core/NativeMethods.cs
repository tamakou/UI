using System.Runtime.InteropServices;

namespace AppUI.Core;


#if false // ENUMはThProcTypeに定義する
enum CsEThLibCmd
{
    Unknown = -1,
    ExeImportData = 0,
    ExeOutputData = 1,
    ExeDeleteData = 2,
    ExeAISegmentator = 3,
    QueryCTSlice = 10,
    QueryCTMarker2D,    //11
    QueryCTMarker3D,    //12
    QueryCTRoi2D,       //13
    QueryCTRoi3D,       //14
    GetStudyPath = 50,
    GetDicomImageInfo,  //51(ThProcLib_DicomImageInfo)
};
enum CsEThLibQueryList
{
    Unknown = -1,
    CTData = 0,
    OutputData = 1,
};
enum CsEThLibCmdType
{
    Sync = 0,  //同期実行、コマンド完了を待つ。
    Async = 1, //非同期実行、コマンド完了を待たず、別途th_lib_get_statusで実行進捗を確認する。
};
enum CsEThLibCmdStatus
{
    Failed = -1,
    Succeeded = 0,
    Progress = 1,
};
enum CsEThLibProcStatus
{
    Unknown = -1,
    NoRoi = 0, // 注目領域なし
    Roi = 1,   // 注目領域あり
};
enum CsEThLibOutputType
{
    Unknown = -1,
    Xreal = 0,
    Xreal_iPhone = 1,
};
#endif

[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
public struct CsPatientInfo
{
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)] public string PatientName;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)] public string PatientID;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 16)] public string Birthday;
    public int Age;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 8)] public string Gender;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 16)] public string StudyDate;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 16)] public string StudyTime;
    public float SliceThickness;        //unit : mm
    public int ProcessStatus;           //関心領域　0: 空白、 1: 〇 ref: CsEThLibProcStatus
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 16)] public string UpdateDate;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 16)] public string UpdateTime;
    public int StudyPattern;            //症例
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)] public string StudDescription;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)] public string StudyUID;//データ識別用、表示しない
    public int OutputType; //準備されたOutputファイルの種類。　ref: CsEThLibOutputType
    public int CheckDig; //check bit, always  1234;
}
[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
public struct CsThProcLibDicomImageInfo
{
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)] public string StudyUID;
    public int SizeX;
    public int SizeY;
    public int SizeZ;
    public int WindowWidth;
    public int WindowCenter;
    public int CheckDig; //check bit, always  2345;
};
[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
public struct CsThProcLibEnv
{
    public int LogLevel;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)] public string ImportFolder;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)] public string HomeFolder;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)] public string OutputFolder;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)] public string DcmRawData;
    public int ThProLibDummy;  //0: system, 1: dummy  
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 512)] public string ModelVers;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)] public string reservedStr1;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)] public string reservedStr2;
    public int reservedData1;
    public float reservedData2;
};
[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
public struct CsCmd
{
    public int cmdID; //0: import data, 1: export mesh data
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)] public string strParam1;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)] public string strParam2;
    public int intParam;
    public float floatParam;
};
[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
public struct CsCmdOutput
{
    public int cmdType; //コマンド実行される時の種類、DLL内部で決める。（同期／非同期）ref: eThLibCmdType 
    public int cmdID; // 実行元のコマンド。　ref: eThLibCmd
    public int cmdUID; //コマンド実行毎に生成させるID
    public int cdmExtDataSize; //コマンド実行毎に生成させるID
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)] public string strParam1;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)] public string strParam2;
    public int intParam;
    public float floatParam;
};
[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
public struct CsCmdStatsu
{
    public int cmdID;	//実行中のコマンドの選択（同時複数の場合） ref: eThLibCmd
    public int cmdStatus; //コマンド実行Status。ref: eThLibCmdStatus 
    public int cmdProgress;//0,1,...,100
    public int dataID;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)] public string strParam1;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)] public string errMessage;
};
[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
// 描画関連コマンド
public struct CsThProcLibVol3DData
{
    public int volID;     //内部生成、管理
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)] public string StudyUID;
    public int sizeX;
    public int sizeY;
    public int sizeZ;
    public float ptichX;  //mm
    public float ptichY;  //mm
    public float ptichZ;  //mm
    //--------
    public int reservedInt1;
    public int reservedInt2;
    public int reservedInt3;
    public float reservedFloat1;
    public float reservedFloat2;
    public float reservedFloat3;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)] public string reservedStr;
};
[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
public struct CsThProcLibImage2DData
{
    public int imageID;//内部生成、管理
    public int sizeX;
    public int sizeY;
    public float ptichX;//mm
    public float ptichY;//mm
    public int format;
    //--------
    public int next_render_cmds; //ref: eRequestRenderCmd  //ビットの組み合わせ
    public int reservedInt1;
    public int reservedInt2;
    public float reservedFloat1;
    public float reservedFloat2;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)] public string reservedStr;
};
[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
public struct CsThProcLibRenderGC
{
    public int renderGcId; //内部生成、管理
    // define rendering command
    public int renderCmdMajor; // ref RenderingCmdType
    public int renderCmdMinor;
    //draw area of window
    public int targeSizeX;
    public int targeSizeY;
    //reander data
    public int seriesType; // ref RenderingSeriesType
    //render parameter
    public float zoom;
    public float panX;
    public float panY;
    public float slicePosition; // slice number
    public int displayParts;//ref: DisplayPartsType
    public int studyCase;	//TBD: 病症
    public int renderPreset;    // for 3D Lut
    //window leve/window width
    public int windowLevle;
    public int windowWidth;
    public int displayMode;     //ref RenderingDisplayMode //表示モードの指定 ビットの組み合わせ
    //processing mouse event
    public int mouseType;
    public int mousePosX;
    public int mousePosY;
    //--------
    public int reservedInt1;
    public int reservedInt2;
    public float reservedFloat1;
    public float reservedFloat2;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)] public string reservedStr;
};

internal static class NativeMethods
{
    [DllImport("ThProcLib.dll", EntryPoint = "th_lib_init", CallingConvention = CallingConvention.Cdecl)]
    internal static extern int Initialize(int a);

    [DllImport("ThProcLib.dll", EntryPoint = "th_lib_close")]
    internal static extern void Terminate();

    [DllImport("ThProcLib.dll", EntryPoint = "th_lib_setup_env", CharSet = CharSet.Unicode)]
    internal static extern int SetEnvironment(ref CsThProcLibEnv env);

    [DllImport("ThProcLib.dll", EntryPoint = "th_lib_get_env", CharSet = CharSet.Unicode)]
    internal static extern int GetEnvironment(ref CsThProcLibEnv env);

    [DllImport("ThProcLib.dll", EntryPoint = "th_lib_get_datalist_size", CharSet = CharSet.None)]
    internal static extern int GetDataListSize(int type);

    [DllImport("ThProcLib.dll", EntryPoint = "th_lib_get_datalist", CharSet = CharSet.None)]
    internal static extern int GetDataList(ref CsPatientInfo out_info, int type, int no);

    [DllImport("ThProcLib.dll", EntryPoint = "th_lib_exe_cmd")]
    internal static extern int ExecuteCommand(ref CsCmd cmd, ref CsCmdOutput cmd_ret);

    [DllImport("ThProcLib.dll", EntryPoint = "th_lib_get_image_info")]
    internal static extern int GetImageInfo(ref CsCmd cmd, ref CsThProcLibDicomImageInfo status);

    [DllImport("ThProcLib.dll", EntryPoint = "th_lib_get_data")]
    internal static extern int GetData(ref CsCmd cmd, byte[] DataBuffer, int BufferSize);

    [DllImport("ThProcLib.dll", EntryPoint = "th_lib_get_status")]
    internal static extern int GetStatus(int cmdUID, ref CsCmdStatsu status);

    [DllImport("ThProcLib.dll", EntryPoint = "th_lib_close_cmd")]
    internal static extern int CloseCommand(int cmdUID);

    [DllImport("ThProcLib.dll", EntryPoint = "th_lib_get_vol_info")]
    internal static extern int GetVolumeInfo(ref CsCmd cmd, ref CsThProcLibVol3DData outVol);

    [DllImport("ThProcLib.dll", EntryPoint = "th_lib_render_GC")]
    internal static extern int RenderGC(ref CsCmd cmd, ref CsThProcLibRenderGC outGC);

    [DllImport("ThProcLib.dll", EntryPoint = "th_lib_exe_render")]
    internal static extern int ExecuteRender(ref CsCmd cmd, ref CsThProcLibRenderGC param, ref CsThProcLibImage2DData outImage);

}
