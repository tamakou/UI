using System.ComponentModel;
using System.Runtime.InteropServices;
using System.Windows.Shapes;

namespace AppUI.Core;

// OutParam
public enum CmdType //ThLibCmdType
{
    Sync = 0,  // 同期
    Async = 1, // 非同期(GetStatusで進捗確認)
}
// OutParam
public enum CmdStatus
{
    Failed = -1,
    Succeeded = 0,
    Progress = 1,
}

// InputParam
public enum ExecCmdType
{
#if DEMO_PROC
    // MEMO: 2025/05/20 テスト用DLLの該当コマンドがAsyncで戻り値がSyncになっておりProcessの進捗通知が実施できないため、
    // デモ用処理では進捗更新が行われる唯一正常にASync動作するExeImportDataの0に置き換えています。
    Unknown = -1,
    ExeImportData = 0,         // CDデータのインポート ★
    ExeOutputData = 0,         // 転送用ファイルの作成 ★
    ExeDeleteData = 2,         // リストの削除
    ExeAISegmentator = 3,      // AI におけるセグメンテーション (Segmentation) とは、画像をピクセル単位で分割し、各ピクセルがどのオブジェクトに属しているかを識別する技術のこと
    ExeTransferData = 0,       // UI側推定挿入 転送用ファイル作成後の実行コマンド  ★
    CancelAsyncCommand = 5,    // Asyncコマンドのキャンセル
    QueryCTSlice = 10,         // 
    QueryCTMarker2D,           // 11
    QueryCTMarker3D,           // 12
    QueryCTRoi2D,              // 13
    QueryCTRoi3D,              // 14
    CTMarkerClear = 20,        // 20
    CTMarkerAdd = 21,          // 21
    CTMarkerFinal = 22,        // 22
    RoiClear = 30,             // 30
    RoiAdd,                    // 31
    RoiErase,                  // 32
    RoiFinal,                  // 33
    RoiSetPen,                 // 34
    OutputRemove = 40,         // 40
    OutputAdd = 41,            // 41
    GetStudyPath = 50,         
    GetDicomImageInfo,         // 51(DICOM情報の取得)
    GetMeshPath,               // 52(転送用フォルダの取得)
    QuerySetting = 99,         // 99
    RenderStart       = 120,
    RenderCreateGC,
    RenderQueryGC,
    RenderVol3D,               
    RenderImage2D,
    RenderLoadVol3D,
    RenderLoad3DModel,
    RenderEnd,
    RenderReleaseStart = 130,
    ReleaseGC,
    ReleaseVol3D,
    ReleaseImage2D,
    RenderOp,
    RenderReleaseEnd,
    CheckImageServer = 200
#else
    Unknown = -1,
    ExeImportData = 0,         // CDデータのインポート
    ExeOutputData = 1,         // 転送用ファイルの作成
    ExeDeleteData = 2,         // リストの削除
    ExeAISegmentator = 3,      // AI におけるセグメンテーション (Segmentation) とは、画像をピクセル単位で分割し、各ピクセルがどのオブジェクトに属しているかを識別する技術のこと
    ExeTransferData = 4,       // UI側推定挿入 転送用ファイル作成後の実行コマンドが必要 TODO:実IDは要調整
    CancelAsyncCommand = 5,    // インポートのキャンセル
    QueryCTSlice = 10,         // 
    QueryCTMarker2D,           // 11
    QueryCTMarker3D,           // 12
    QueryCTRoi2D,              // 13
    QueryCTRoi3D,              // 14
    CTMarkerClear = 20,        // 20 やり直しボタン
    CTMarkerAdd = 21,          // 21
    CTMarkerFinal = 22,        // 22
    RoiClear = 30,             // 30 全て削除
    RoiAdd,                    // 31 追加
    RoiErase,                  // 32 消しゴム
    RoiFinal,                  // 33 最終確定
    RoiSetPen,                 // 34 ペンの太さ
    OutputRemove = 40,         // 40
    OutputAdd,                 // 41
    GetStudyPath = 50,         
    GetDicomImageInfo,         // 51(DICOM情報の取得)
    GetMeshPath,               // 52(転送用フォルダの取得)
    QuerySetting = 99,         // 99
    RenderStart       = 120,
    RenderCreateGC,
    RenderQueryGC,
    RenderVol3D,               
    RenderImage2D,             // Zoom
    RenderLoadVol3D,
    RenderLoad3DModel,
    RenderEnd,
    RenderReleaseStart = 130,
    ReleaseGC,                 // 
    ReleaseVol3D,              // 
    ReleaseImage2D,            // 
    RenderOp,                  // MarkerRoi:縦隔/肺野、描画、マウス操作、ペン、表示/症例選択
    RenderReleaseEnd,
    CheckImageServer = 200
#endif
};

// I/O Param
enum CmdListType           // リスト種別
{
    Unknown = -1,
    CTData = 0,            // CTデータ
    OutputData = 1,        // 転送データ
};

// RoI Param
enum RoiType               //  関心領域（Region of Interest) 種別
{
    Unknown = -1,
    NoRoi = 0,
    Roi = 1,
};

public enum CTDataTransportType　 // 準備した転送用ファイルの種類
{
    [Description("")]
    Unknown = -1,
    [Description("XREAL")]
    Xreal = 0,
    [Description("iPhone")]
    iPhone = 1,
    [Description("XREALiPhone")]
    XrealIPhone = 2,
};

public enum RenderingSeriesType
{
    Unknown = -1,
    Haiya = 0,
    Juukaku = 1,
};

[Flags]
// 表示モード(bit指定)
public enum RenderingDisplayMode
{
    Unknown = 0x00,
    Volume = 0x01,
    Polygon = 0x02,
    CTMarker = 0x04,
    CTDataImport = Volume,
    CTMarkerSettings = Volume | CTMarker,
    RoiSettings = Volume | Polygon,
};

[Flags]
// 表示パーツの指定 (bit指定)
public enum DisplayPartsType
{
    Unknown = 0x0000,
    Roi = 0x0001,
    Body = 0x0002,
    Lung = 0x0004,
    Heart = 0x0008,
    Other = 0x0100,
};

[Flags]
// Renderコマンドの要求
public enum RequestRenderCmd
{
    Unknown = 0x00,
    Dim2Pattern1 = 0x01,
    Dim2Pattern2 = 0x02,
    Dim2Pattern3 = 0x04,
    Dim3 = 0x08,
};

// TBDらしいので暫定
public enum StudyCaseType
{
    Unknown = 0,
    Pneumothorax = 1,
    PleuralEffusion = 2,
    Empyema = 3,
    Hemothorax = 4,
    Other = 5,
};

public enum RenderingCmdType
{
    Unknown = 0,
    Dim2 = 1, // 2D
    Dim3 = 2, // 3D
    Setup = 3,
    ProcMouse = 4,
    ChangeCase = 5,
    SetupSeries = 6,
    Dim3SetupObj = 7,
};

public enum RenderImageFormat
{
    Unknown = -1,
    Gray8 = 0,
    Gray16,
    RGB24,
    ARGB32,
};

public enum PenType
{
    Unknown = 0,
    Pen1 = 1,
    Pen2,
    Pen3,
    Pen4,
};
enum MouseEventType
{
    Unknown = 0,
    MouseMove,
    LeftButtonDown,
    LeftButtonUp,
    LeftButtonDoubleClick,
    RightButtonDown,
    RightButtonUp,
    RightButtonDoubleClick,
    MouseWheel,
    User = 20,
    CTMarker2D,
    CTMarker3D,
    RoiPen,
    RoiEraser,
    Rotation3D,
};

public class ThProcLibEnv
{
    public int LogLevel { get; set; }
    public string ImportFolder { get; set; } = "";
    public string HomeFolder { get; set; } = "";
    public string LogFolder { get; set; } = "";
    public string OutputFolder { get; set; } = "";
    public string DcmRawData { get; set; } = "";
    public string ModelVers { get; set; } = "";
}

public class ThProcCmdStatus
{
    public int CmdID { get; set; }
    public CmdStatus CmdStatus { get; set; }
    public int CmdProgress { get; set; }
    public int DataID { get; set; }
    public string StrParam1 { get; set; } = "";
    public string ErrMessage { get; set; } = "";
}

public class ThProcCmdOutput
{
    public CmdType CmdType { get; set; }  // 0:sync, 1:async
    public int CmdID { get; set; }        // 実行コマンドの元ID
    public int CmdUID { get; set; }       // コマンド実行毎に生成させるID
    public string StrParam1 { get; set; } = "";
    public string StrParam2 { get; set; } = "";
    public int IntParam { get; set; }
    public float FloatParam { get; set; }
}

public record class ThProcPatientInfo
{
    public string PatientName { get; set; } = "";         // 患者名：氏名
    public string PatientID { get; set; } = "";           // 患者ID：数字
    public string Birthday { get; set; } = "";            // 生年月日：YYYY/MM/DD
    public string Age { get; set; } = "";                 // 年齢：数字
    public string Gender { get; set; } = "";              // 性別：男or女
    public string StudyDate { get; set; } = "";           // 撮影日：YYYY/MM/DD
    public string StudyTime { get; set; } = "";           // 撮影時間：HH:mm:ss
    public string StudyDateTime { get; set; } = "";       // 撮影日 & 撮影時間
    public string SliceThickness { get; set; } = "";      // unit : mm
    //public string CTMode { get; set; } = "";
    public string ProcessStatus { get; set; } = "";       // マーカ関心領域：◯or空白
    public string UpdateDate { get; set; } = "";          // YYYY/MM/DD
    public string UpdateTime { get; set; } = "";          // HH:MM:SS
    public string StudyPattern { get; set; } = "";        // 症例：番号
    public string StudDescription { get; set; } = "";     // 症例：詳細？
    public string StudyUID { get; set; } = "";            // データ識別用、表示しない
    //public string CheckDig { get; set; } = "";          // データ識別用、表示しない
}
public record class ThProcTransportPatientInfo
{
    public ThProcPatientInfo Patient { get; set; } = new();
    public CTDataTransportType TransportType { get; set; } = CTDataTransportType.Xreal;
}

public class ThProcLibDicomImageInfo
{
    public string StudyUID { get; set; } = "";
    public int SizeX { get; set; }
    public int SizeY { get; set; }
    public int SizeZ { get; set; }
    public int WindowWidth { get; set; }
    public int WindowCenter { get; set; }
    //public int CheckDig;                                 //check bit, always  2345;
}

public class ThProcLibVol3DData
{
    public int VolID { get; set; }
    public string StudyUID { get; set; } = "";
    public int SizeX { get; set; }
    public int SizeY { get; set; }
    public int SizeZ { get; set; }
#if false // UI側は未使用
    public float PitchX { get; set; }
    public float PitchY { get; set; }
    public float PitchZ { get; set; }
    public int ReservedInt1 { get; set; }
    public int ReservedInt2 { get; set; }
    public int ReservedInt3 { get; set; }
    public float ReservedFloat1 { get; set; }
    public float ReservedFloat2 { get; set; }
    public float ReservedFloat3 { get; set; }
    public string ReservedStr { get; set; } = "";
#endif
}

public class ThProcLibImage2DData
{
    public int ImageID { get; set; }
    public int SizeX { get; set; }
    public int SizeY { get; set; }
    public float PitchX { get; set; }
    public float PitchY { get; set; }
    public RenderImageFormat Format { get; set; }
#if false // UI側未使用
    public int ReservedInt1 { get; set; }
    public int ReservedInt2 { get; set; }
    public float reservedFloat1 { get; set; }
    public float reservedFloat2 { get; set; }
    public string reservedStr { get; set; } = "";
#endif
}

public class ThProcLibRenderGC
{
    public int RenderGcId { get; set; }
    // define rendering command
    public RenderingCmdType RenderCmdMajor { get; set; }
    public int RenderCmdMinor { get; set; }
    //draw area of window
    public int TargeSizeX { get; set; }
    public int TargeSizeY { get; set; }
    //render data
    public RenderingSeriesType SeriesType { get; set; }
    //render parameter
    public float Zoom { get; set; } 
    public float PanX { get; set; } 
    public float PanY { get; set; }
    public float SlicePosition { get; set; }    // slice number
    public int RenderPreset { get; set; }       // for 3D Lut
    //window leve/window width
    public int WindowLevle { get; set; }
    public int WindowWidth { get; set; }
    public RenderingDisplayMode DisplayMode { get; set; }
    //processing mouse event
    public int MouseType { get; set; }
    public int MousePosX { get; set; }
    public int MousePosY { get; set; }
#if false // UI側未使用
    public int ReservedInt1;
    public int ReservedInt2;
    public float ReservedFloat1;
    public float ReservedFloat2;
    public string ReservedStr;
#endif
}
