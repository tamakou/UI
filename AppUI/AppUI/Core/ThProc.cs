using System.Text.RegularExpressions;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using AppUI.Services;
using AppUI.ViewModels;
using CommunityToolkit.Mvvm.Messaging;

namespace AppUI.Core;

public partial class ThProc
{
    /// <summary>
    /// DLLの初期化
    /// </summary>
    /// <param name="mode">mode指定</param>
    /// <returns>0：成功,それ以外は失敗。失敗の場合はth_lib_get_statusで確認</returns>
    public static bool Initialize(int mode)
    {
        int result;
        result = NativeMethods.Initialize(mode);
        if (result != 0)
        {
            GetLastError(out var code, out var message);
            throw new ExpectedException(code, message);
        }
        return (result == 0);
    }

    public static void Terminaite()
    {
        NativeMethods.Terminate();
    }

    /// <summary>
    /// DLLの実行環境を設定する
    /// </summary>
    /// <param name="env">実行環境</param>
    /// <returns>0：成功,それ以外は失敗。失敗の場合はth_lib_get_statusで確認</returns>
    public static bool SetEnvironment(ThProcLibEnv env)
    {
        var setEnv = new CsThProcLibEnv
        {
            LogLevel = env.LogLevel,
            HomeFolder = env.HomeFolder,
            DcmRawData = env.DcmRawData,
            OutputFolder = env.OutputFolder,
        };
        Log.Info("th_lib_set_env LogLevel:{0} HomeFolder:{1} DcmRawData:{2} OutputFolder:{3}",
            setEnv.LogLevel, setEnv.HomeFolder, setEnv.DcmRawData, setEnv.OutputFolder);
        if (NativeMethods.SetEnvironment(ref setEnv) != 0)
        {
            Log.Warn("th_lib_set_env failed.");
            return false;
        }
        return true;
    }

    public static bool GetEnvironment(out ThProcLibEnv output)
    {
        int result;
        var env = new CsThProcLibEnv();
        output = new ThProcLibEnv();
        result = NativeMethods.GetEnvironment(ref env);
        if (result == 0)
        {
            output.ImportFolder = env.ImportFolder;
            output.LogLevel = env.LogLevel;
            output.HomeFolder = env.HomeFolder;
            output.OutputFolder = env.OutputFolder;
            output.DcmRawData = env.DcmRawData;
            output.ModelVers = env.ModelVers;
            Log.Info("th_lib_get_env LogLevel:{0} HomeFolder:{1} DcmRawData:{2} OutputFolder:{3}",
                output.LogLevel, output.HomeFolder, output.DcmRawData, output.OutputFolder);
        }
        else
        {
            Log.Warn("th_lib_get_env failed result:{0}.", result);
            GetLastError(out var errCode, out var errMessage);
            throw new ExpectedException(errCode, errMessage);
        }
        return true;
    }
    public static bool IsImportFromCD(out string ImportPath)
    {
        /*
         * DLL 仕様メモ
         * ドライブ直下の場合はCD-ROMとする
         * C:\DicomData\
         */
        if (ThProc.GetEnvironment(out var env))
        {
            ImportPath = env.ImportFolder;
            var ret = DriveRootPathRegex().IsMatch(ImportPath);
            return ret;
        }
        ImportPath = "";
        return false;
    }

    public static int GetCTDataListSize() => GetDataListSize(CmdListType.CTData);
    public static int GetTransportDataListSize() => GetDataListSize(CmdListType.OutputData);

    private static int GetDataListSize(CmdListType type)
    {
        int result;
        result = NativeMethods.GetDataListSize((int)type);
        Log.Info("th_lib_get_datalist_size({0}) result:{1}", type, result);
        return result;
    }

    public static bool GetCTDataList(int no, out CsPatientInfo patientInfo) => GetDataList(no, CmdListType.CTData, out patientInfo);
    public static bool GetTransportDataList(int no, out CsPatientInfo patientInfo) => GetDataList(no, CmdListType.OutputData, out patientInfo);
    private static bool GetDataList(int no, CmdListType type, out CsPatientInfo patientInfo)
    {
        int result;
        patientInfo = new CsPatientInfo();
        result = NativeMethods.GetDataList(ref patientInfo, (int)type, no);
        if (result != 0)
        {
            Log.Warn("th_lib_get_datalist(no:{0}, type:{1}) failed. result:{2}", no, type, result);
            return false;
        }
        return true;
    }

    public static Task<ThProcCmdStatus> ExecuteImportCommand(out ThProcCmdOutput output, 
        CancellationTokenSource cts, Action<ThProcCmdStatus>? onProgress = null, Action<ThProcCmdStatus>? onSucceeded = null, Action<ThProcCmdStatus>? onFailed = null) =>
        ExecuteAsyncCommand(new CsCmd { cmdID = (int)ExecCmdType.ExeImportData }, out output, cts, onProgress, onSucceeded, onFailed);                                    // CDデータのインポート(Async)
    public static Task<ThProcCmdStatus> ExecuteCreatingTranferFileCommand(string studyUid, CTDataTransportType type, out ThProcCmdOutput output, 
        CancellationTokenSource cts, Action<ThProcCmdStatus>? onProgress = null, Action<ThProcCmdStatus>? onSucceeded = null, Action<ThProcCmdStatus>? onFailed = null) =>
        ExecuteAsyncCommand(new CsCmd { cmdID = (int)ExecCmdType.ExeOutputData, intParam = (int)type }, out output, cts, onProgress, onSucceeded, onFailed);               // 転送用ファイルの作成(Async)
    public static Task<ThProcCmdStatus> ExecuteFileTranferCommand(string studyUid, CTDataTransportType type, out ThProcCmdOutput output,
        CancellationTokenSource cts, Action<ThProcCmdStatus>? onProgress = null, Action<ThProcCmdStatus>? onSucceeded = null, Action<ThProcCmdStatus>? onFailed = null) =>
        ExecuteAsyncCommand(new CsCmd { cmdID = (int)ExecCmdType.ExeTransferData, strParam1 = studyUid, intParam = (int)type }, out output, cts, onProgress, onSucceeded, onFailed);    // 転送の実行(Async)
    public static bool ExecuteCTDataListDeleteCommand(string studyUid, out ThProcCmdOutput output) =>
        ExecuteCommand(new CsCmd { cmdID = (int)ExecCmdType.ExeDeleteData, strParam1 = studyUid, intParam = (int)CmdListType.CTData }, out output) == 0;    // リストの削除(Sync) : CTデータ用
    public static Task<ThProcCmdStatus> ExecuteAISegmentatorCommand(string studyUid, out ThProcCmdOutput output,
        CancellationTokenSource cts, Action<ThProcCmdStatus>? onProgress = null, Action<ThProcCmdStatus>? onSucceeded = null, Action<ThProcCmdStatus>? onFailed = null) =>
        ExecuteAsyncCommand(new CsCmd { cmdID = (int)ExecCmdType.ExeAISegmentator, strParam1 = studyUid, strParam2 = "" }, out output, cts, onProgress, onSucceeded, onFailed);       // AIセグメント(Async)
    public static Task<ThProcCmdStatus> ExecuteOutputRemoveCommand(string studyUid, CTDataTransportType removeType, out ThProcCmdOutput output,
        CancellationTokenSource cts, Action<ThProcCmdStatus>? onProgress = null, Action<ThProcCmdStatus>? onSucceeded = null, Action<ThProcCmdStatus>? onFailed = null) =>
        ExecuteAsyncCommand(new CsCmd { cmdID = (int) ExecCmdType.OutputRemove, strParam1 = studyUid, intParam = (int)removeType }, out output, cts, onProgress, onSucceeded, onFailed);
    public static Task<ThProcCmdStatus> ExecuteOutputAddCommand(string studyUid, CTDataTransportType addType, out ThProcCmdOutput output,
        CancellationTokenSource cts, Action<ThProcCmdStatus>? onProgress = null, Action<ThProcCmdStatus>? onSucceeded = null, Action<ThProcCmdStatus>? onFailed = null) =>
        ExecuteAsyncCommand(new CsCmd { cmdID = (int)ExecCmdType.OutputAdd, strParam1 = studyUid, intParam = (int)addType }, out output, cts, onProgress, onSucceeded, onFailed);
    public static string ExecuteGetMeshFolder(string studyUid, CTDataTransportType type)
    {
        if (ExecuteCommand(new CsCmd { cmdID = (int)ExecCmdType.GetMeshPath, strParam1 = studyUid, intParam=(int)type }, out var output) == 0)
        {
            return output.StrParam1;
        }
        return "";
    }
    public static bool ExecuteImportManualTextCommand(out ThProcCmdOutput output) 
        => ExecuteCommand(new CsCmd { cmdID = (int)ExecCmdType.QuerySetting, intParam = 0}, out output) == 0;
    public static bool ExecuteImportVersionTextCommand(out ThProcCmdOutput output) 
        => ExecuteCommand(new CsCmd { cmdID = (int)ExecCmdType.QuerySetting, intParam = 1 }, out output) == 0;
    public static bool ExecuteImportInquiryTextCommand(out ThProcCmdOutput output) 
        => ExecuteCommand(new CsCmd { cmdID = (int)ExecCmdType.QuerySetting, intParam = 2 }, out output) == 0;
    private static int ExecuteCommand(CsCmd cmd, out ThProcCmdOutput output)
    {
        int result;
        output = new ThProcCmdOutput();

        var cmdOutput = new CsCmdOutput();
        result = NativeMethods.ExecuteCommand(ref cmd, ref cmdOutput);
        if (result == 0)
        {
            output.CmdType = (CmdType)cmdOutput.cmdType;
            output.CmdID = cmdOutput.cmdID;
            output.CmdUID = cmdOutput.cmdUID;
            output.StrParam1 = cmdOutput.strParam1;
            output.StrParam2 = cmdOutput.strParam2;
            output.IntParam = cmdOutput.intParam;
            output.FloatParam = cmdOutput.floatParam;
            Log.Trace("th_lib_exe_cmd in(cmdId:{0} strParam1:{1} strParam2:{2} intParam:{3} floatParam:{4}",
                cmd.cmdID, cmd.strParam1, cmd.strParam2, cmd.intParam, cmd.floatParam);
            Log.Trace("               out(CmdType:{0} CmdID:{1} CmdUID:{2} StrParam1:{3} StrParam2:{4} IntParam:{5} FloatParam:{6}",
                output.CmdType, output.CmdID, output.CmdUID, output.StrParam1, output.StrParam2, output.IntParam, output.FloatParam);
        }
        else
        {
            Log.Error("th_lib_exe_cmd CsCmd(cmdId:{0} strParam1:{1} strParam2:{2} intParam:{3} floatParam:{4}",
                cmd.cmdID, cmd.strParam1, cmd.strParam2, cmd.intParam, cmd.floatParam);
        }

        return result;
    }

    private static bool ExecuteCancelAsyncCommand(int cmdUId, out ThProcCmdOutput output)
        => ExecuteCommand(new CsCmd { cmdID = (int)ExecCmdType.CancelAsyncCommand, intParam = cmdUId }, out output) == 0;

    private static Task<ThProcCmdStatus> ExecuteAsyncCommand(CsCmd cmd, 
        out ThProcCmdOutput output,
        CancellationTokenSource cts,        
        Action<ThProcCmdStatus>? onProgress = null,
        Action<ThProcCmdStatus>? onSucceeded = null,
        Action<ThProcCmdStatus>? onFailed = null,
        double delayTime = 0.5)

    {
        int result;
        output = new ThProcCmdOutput();

        var cmdOutput = new CsCmdOutput();
        result = NativeMethods.ExecuteCommand(ref cmd, ref cmdOutput);
        if (result == 0)
        {
            output.CmdType = (CmdType)cmdOutput.cmdType;
            output.CmdID = cmdOutput.cmdID;
            output.CmdUID = cmdOutput.cmdUID;
            output.StrParam1 = cmdOutput.strParam1;
            output.StrParam2 = cmdOutput.strParam2;
            output.IntParam = cmdOutput.intParam;
            output.FloatParam = cmdOutput.floatParam;
            Log.Trace("th_lib_exe_cmd in(cmdId:{0} strParam1:{1} strParam2:{2} intParam:{3} floatParam:{4}",
                cmd.cmdID, cmd.strParam1, cmd.strParam2, cmd.intParam, cmd.floatParam);
            Log.Trace("               out(CmdType:{0} CmdID:{1} CmdUID:{2} StrParam1:{3} StrParam2:{4} IntParam:{5} FloatParam:{6}",
                output.CmdType, output.CmdID, output.CmdUID, output.StrParam1, output.StrParam2, output.IntParam, output.FloatParam);
        }
        else
        {
            if (GetLastError(out var errCode, out var errMessage, cmdOutput.cmdID))
            {
                Log.Warn("ExecuteAsyncCommand th_lib_exe_cmd failed. message:{0}", errMessage);
                throw new WarningException(errCode, errMessage); 
            }
            else
            {
                Log.Warn("ExecuteAsyncCommand th_lib_exe_cmd failed.");
                throw new WarningException("ExecuteAsyncCommand th_lib_exe_cmd failed.");
            }
        }

        if (output.CmdType != CmdType.Async)
        {
            throw new NotSupportedException($"th_lib_exe_cmd CsCmd(cmdId:{cmd.cmdID}) is not Async Command?");
        }

        return PollingLoopAsync(output.CmdUID, onProgress, onSucceeded, onFailed, delayTime, cts.Token);
    }
    private static async Task<ThProcCmdStatus> PollingLoopAsync(
        int cmdUid,
        Action<ThProcCmdStatus>? onProgress,
        Action<ThProcCmdStatus>? onSucceeded,
        Action<ThProcCmdStatus>? onFailed,
        double delayTime,
        CancellationToken token)
    {
        var statusResult = new ThProcCmdStatus() { CmdStatus = CmdStatus.Failed };
        while (true)
        {
            try
            {
                // ポーリング対象処理
                if (ThProc.GetStatus(cmdUid, out statusResult) == 0)
                {
                    if (statusResult.CmdStatus is CmdStatus.Progress)
                    {
                        onProgress?.Invoke(statusResult);
                    }
                    else if (statusResult.CmdStatus is CmdStatus.Succeeded)
                    {
                        onSucceeded?.Invoke(statusResult);
                        break;
                    }
                    else
                    {
                        onFailed?.Invoke(statusResult);
                        break;
                    }
                }
                else
                {
                    break;
                }

                await Task.Delay(TimeSpan.FromSeconds(delayTime), token);
            }
            catch(OperationCanceledException)
            {
                if (!ExecuteCancelAsyncCommand(cmdUid, out _))
                {
                    onFailed?.Invoke(statusResult);
                    break;
                }
            }
        }

        if (ThProc.CloseCommandProc(cmdUid) != 0)
        {
            if (GetLastError(out var errCode, out var errMessage))
            {
                Log.Warn("ExecuteAsyncCommand th_lib_close_cmd failed. message:{0}", errMessage);
                throw new WarningException(errCode, errMessage);
            }
            else
            {
                throw new FatalException("");
            }
        }
        else if (statusResult.CmdStatus != CmdStatus.Succeeded)
        {
            if (GetLastError(out var errCode, out var errMessage))
            {
                Log.Warn("ExecuteAsyncCommand statusResult.CmdStatus isn't Succeeded.");
                throw new WarningException(errCode, errMessage);
            }
            else
            {
                throw new FatalException("");
            }
        }

        token.ThrowIfCancellationRequested(); // 処理中にキャンセルした場合はCmdStatusがSucceededにならないため、通常呼ばれることはない

        return statusResult;
    }

    public static bool ExecutGetMediastinumDicomImageInfoCommand(string studyUid, out ThProcLibDicomImageInfo output)
        => GetDicomImageInfo(new CsCmd { cmdID = (int)ExecCmdType.GetDicomImageInfo, strParam1 = studyUid, intParam = (int)RenderingSeriesType.Juukaku }, out output);
    public static bool ExecutGetLungFieldDicomImageInfoCommand(string studyUid, out ThProcLibDicomImageInfo output)
        => GetDicomImageInfo(new CsCmd { cmdID = (int)ExecCmdType.GetDicomImageInfo, strParam1 = studyUid, intParam = (int)RenderingSeriesType.Haiya }, out output);
    private static bool GetDicomImageInfo(CsCmd cmd, out ThProcLibDicomImageInfo output)
    {
        output = new ThProcLibDicomImageInfo();

        var cmdOutput = new CsThProcLibDicomImageInfo();
        var result = NativeMethods.GetImageInfo(ref cmd, ref cmdOutput);
        if (result == 0)
        {
            output.StudyUID = cmdOutput.StudyUID;
            output.SizeX = cmdOutput.SizeX;
            output.SizeY = cmdOutput.SizeY;
            output.SizeZ = cmdOutput.SizeZ;
            output.WindowWidth = cmdOutput.WindowWidth;
            output.WindowCenter = cmdOutput.WindowCenter;
            Log.Trace("th_lib_get_image_info in(cmdId:{0} strParam1:{1} strParam2:{2} intParam:{3} floatParam:{4}",
                cmd.cmdID, cmd.strParam1, cmd.strParam2, cmd.intParam, cmd.floatParam);
            Log.Trace("                      out(StudyUID:{0} SizeX:{1} SizeY:{2} SizeZ:{3} WindowWidth:{4} WindowCenter:{5}",
                output.StudyUID, output.SizeX, output.SizeY, output.SizeZ, output.WindowWidth, output.WindowCenter);
        }
        else
        {
            Log.Warn("th_lib_get_image_info failed");
            Log.Warn("                      in(cmdId:{0} strParam1:{1} strParam2:{2} intParam:{3} floatParam:{4})", 
                cmd.cmdID, cmd.strParam1, cmd.strParam2, cmd.intParam, cmd.floatParam);
            return false;
        }
        return true;
    }
    public static int CloseCommandProc(int cmUid)
    {
        return NativeMethods.CloseCommand(cmUid);
    }

    public static bool GetLastError(out string errCode, out string errMessage, int cmdId = -1)
    {
        if (GetStatus(cmdId, out var result) == 0)
        {
            errCode = result.StrParam1;
            errMessage = result.ErrMessage;
            return true;
        }

        errCode = "";
        errMessage = "";
        return false;
    }

    private static int GetStatus(int cmUid, out ThProcCmdStatus output)
    {
        output = new ThProcCmdStatus();

        var csCmd = new CsCmdStatsu();
        var result = NativeMethods.GetStatus(cmUid, ref csCmd);
        if (result == 0)
        {
            output.CmdID = csCmd.cmdID;
            output.CmdStatus = (CmdStatus)csCmd.cmdStatus;
            output.CmdProgress = csCmd.cmdProgress;
            output.DataID = csCmd.dataID;
            output.StrParam1 = csCmd.strParam1;
            output.ErrMessage = csCmd.errMessage;
            Log.Trace("th_lib_get_status in(cmdId:{0})", cmUid);
            Log.Trace("                  out(CmdId:{0} CmdStatus:{1} CmdProgress:{2} DataID:{3} StrParam1:{4})", 
                output.CmdID, output.CmdStatus, output.CmdProgress, output.DataID, output.StrParam1);
            Log.Trace("                  out(ErrMessage:{0})", output.ErrMessage);
        }

        return result;
    }

    // 未使用コマンド
    public static bool ExecuteQueryCTSliceCommand(string studyUid, out ThProcCmdOutput output) =>
        ExecuteCommand(new CsCmd { cmdID = (int)ExecCmdType.QueryCTSlice, strParam1 = studyUid, strParam2 = "" }, out output) == 0;          // 2D確認用の画像取得(Sync)
    public static bool ExecuteQueryCTMarker2DCommand(string studyUid, out ThProcCmdOutput output) =>
        ExecuteCommand(new CsCmd { cmdID = (int)ExecCmdType.QueryCTMarker2D, strParam1 = studyUid, strParam2 = "" }, out output) == 0;       // 2D画像情報の取得(Sync)
    public static bool ExecuteQueryCTMarker3DCommand(string studyUid, out ThProcCmdOutput output) =>
        ExecuteCommand(new CsCmd { cmdID = (int)ExecCmdType.QueryCTMarker3D, strParam1 = studyUid, strParam2 = "" }, out output) == 0;
    public static bool ExecuteQueryCTRoi2DCommand(string studyUid, out ThProcCmdOutput output) =>
        ExecuteCommand(new CsCmd { cmdID = (int)ExecCmdType.QueryCTRoi2D, strParam1 = studyUid, strParam2 = "" }, out output) == 0;
    public static bool ExecuteQueryCTRoi3DCommand(string studyUid, out ThProcCmdOutput output) =>
        ExecuteCommand(new CsCmd { cmdID = (int)ExecCmdType.QueryCTRoi3D, strParam1 = studyUid, strParam2 = "" }, out output) == 0;
    public static bool ExecuteCTMarkerAddCommand(out ThProcCmdOutput output) =>
        ExecuteCommand(new CsCmd { cmdID = (int)ExecCmdType.CTMarkerAdd }, out output) == 0;
    public static bool ExecuteRoiAddCommand(out ThProcCmdOutput output) =>
        ExecuteCommand(new CsCmd { cmdID = (int)ExecCmdType.RoiAdd }, out output) == 0;
    public static bool ExecuteRoiEraseCommand(out ThProcCmdOutput output) =>
        ExecuteCommand(new CsCmd { cmdID = (int)ExecCmdType.RoiErase }, out output) == 0;
    public static bool ExecuteTransportListDeleteCommand(string studyUid, out ThProcCmdOutput output) =>
        ExecuteCommand(new CsCmd { cmdID = (int)ExecCmdType.ExeDeleteData, strParam1 = studyUid, intParam = (int)CmdListType.OutputData }, out output) == 0; // リストの削除(Sync) : 転送データ用
    public static bool ExecutGettingStudyPathCommand(string studyUid, out ThProcCmdOutput output) =>
        ExecuteCommand(new CsCmd { cmdID = (int)ExecCmdType.GetStudyPath, strParam1 = studyUid, strParam2 = "" }, out output) == 0;

    //===========================
    // レンダリングコマンド
    //===========================
    private class ThRenderCTModeInfo
    {
        public int RenderGcId { get; set; } = 0;
        public int WindowLevel { get; set; } = 0;
        public int WindowWidth { get; set; } = 0;
    }
    private class ThRenderImageInfo
    {
        public CsThProcLibRenderGC RenderGC { get; set; } = new();
        public CsThProcLibImage2DData Image2D { get; set; } = new();

    }

    private class ThVolumeRenderInfo
    {
        public CsThProcLibVol3DData VolumeInfo { get; set; } = new();
        public ThRenderImageInfo? RenderImageInfo2D { get; set; }
        public ThRenderImageInfo? RenderImageInfo3D { get; set; }
        public RenderingSeriesType LinkedSeriesType { get; set; } = RenderingSeriesType.Juukaku;
        public RenderingDisplayMode LinkedDisplayMode { get; set; } = RenderingDisplayMode.CTMarker;
        public ThRenderCTModeInfo MediastinumWindowInfo { get; set; } = new();
        public ThRenderCTModeInfo LungFieldWindowInfo { get; set; } = new();
    }

    private static Dictionary<int, ThVolumeRenderInfo> VolumeRenderInfos { get; set; } = new();

    public static Task<int> LoadRenderingVolumeFor3D(
        string studyUid,
        CancellationTokenSource cts,
        Action<ThProcCmdStatus>? onProgress = null,
        Action<ThProcCmdStatus>? onSucceeded = null,
        Action<ThProcCmdStatus>? onFailed = null)
    {
        return LoadRenderingVolume(studyUid, true, cts, onProgress, onSucceeded, onFailed);
    }
    public static Task<int> LoadRenderingVolumeFor2D(
        string studyUid,
        CancellationTokenSource cts,
        Action<ThProcCmdStatus>? onProgress = null,
        Action<ThProcCmdStatus>? onSucceeded = null,
        Action<ThProcCmdStatus>? onFailed = null)
    {
        return LoadRenderingVolume(studyUid, false, cts, onProgress, onSucceeded, onFailed);
    }

    private static async Task<int> LoadRenderingVolume(
        string studyUid,
        bool isLoadModel3D,
        CancellationTokenSource cts, 
        Action<ThProcCmdStatus>? onProgress = null, 
        Action<ThProcCmdStatus>? onSucceeded = null, 
        Action<ThProcCmdStatus>? onFailed = null)
    {
        var dataId = 0;

        try
        {
            var pollingStatus = await ExecuteRenderLoadVolume3DCommand(studyUid, out var exportResult, cts, onProgress, onSucceeded);
            dataId = pollingStatus.DataID;
        }
        catch
        {
            throw;
        }

        // Volumeデータ情報を取得
        if (!ExecuteRenderVolume3DCommand(dataId, out var vol3DData))
        {
            Log.Debug("ExecuteRenderVolume3DCommand failed");
            GetLastError(out var errCode, out var errMessage);
            throw new WarningException(errCode, errMessage);
        }

        if (isLoadModel3D)
        {
            if (!ExecuteRenderLoad3DModel(studyUid, out _))
            {
                Log.Debug("ExecuteRenderLoad3DModel failed");
                GetLastError(out var errCode, out var errMessage);
                throw new WarningException(errCode, errMessage);
            }
        }

        // Renderingコマンド用のGCを新規作成
        if (!ExecutGetMediastinumDicomImageInfoCommand(studyUid, out var dicomInfoMed))
        {
            Log.Debug("ExecutGetMediastinumDicomImageInfoCommand failed");
            GetLastError(out var errCode, out var errMessage);
            throw new WarningException(errCode, errMessage);
        }

        if (!ExecutGetLungFieldDicomImageInfoCommand(studyUid, out var dicomInfoLng))
        {
            Log.Debug("ExecutGetLungFieldDicomImageInfoCommand failed");
            GetLastError(out var errCode, out var errMessage);
            throw new WarningException(errCode, errMessage);
        }

        var key = 0;
        while (VolumeRenderInfos.ContainsKey(key++));
        VolumeRenderInfos.Add(key, new ThVolumeRenderInfo 
        {   
            VolumeInfo = vol3DData,
            MediastinumWindowInfo = new ThRenderCTModeInfo { WindowLevel = dicomInfoMed.WindowCenter, WindowWidth = dicomInfoMed.WindowWidth },
            LungFieldWindowInfo = new ThRenderCTModeInfo { WindowLevel=dicomInfoLng.WindowCenter, WindowWidth = dicomInfoLng.WindowWidth} 
        });

        return key;
    }

    // Volumeデータのロード
    private static Task<ThProcCmdStatus> ExecuteRenderLoadVolume3DCommand(string studyUid, out ThProcCmdOutput output,
        CancellationTokenSource cts, Action<ThProcCmdStatus>? onProgress = null, Action<ThProcCmdStatus>? onSucceeded = null, Action<ThProcCmdStatus>? onFailed = null) =>
        ExecuteAsyncCommand(new CsCmd { cmdID = (int)ExecCmdType.RenderLoadVol3D, strParam1 = studyUid }, out output, cts, onProgress, onSucceeded, onFailed);
    // Volumeデータ情報の取得
    private static bool ExecuteRenderVolume3DCommand(int volId, out CsThProcLibVol3DData output)
        => GetVolumeInfo((int)ExecCmdType.RenderVol3D, volId, out output) == 0;
    // 3Dモデルのロード
    private static bool ExecuteRenderLoad3DModel(string studyUid, out ThProcCmdOutput output)
        => ExecuteCommand(new CsCmd { cmdID = (int)ExecCmdType.RenderLoad3DModel, strParam1 = studyUid }, out output) == 0;
    // ImageServerの起動確認
    public static bool ExecuteCheckImageServerCommand()
    {
        if (ExecuteCommand(new CsCmd { cmdID = (int)ExecCmdType.CheckImageServer }, out var outResult) != 0) return false;
        return outResult.IntParam == 1; // 1:ImageServer正常、0:ImageServer異常
    }

    // Renderingコマンド用のGCを新規作成
    public static bool GenerateRenderGraphicContextFor2D(int volumeInfoKey, RenderingSeriesType type)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo))
        {
            TryGetValueErrorLog("GenerateRenderGraphicContextFor2D", volumeInfoKey);
            return false;
        }
        volInfo.RenderImageInfo2D ??= new ThRenderImageInfo();
        if (!GenerateRenderGraphicContext(volInfo.RenderImageInfo2D))
        {
            Log.Warn("GenerateRenderGraphicContext({0}, {1}) failed",
                volumeInfoKey, type);
            GetLastError(out var errCode, out var errMessage);
            throw new WarningException(errCode, errMessage);
        }
        volInfo.MediastinumWindowInfo.RenderGcId = volInfo.RenderImageInfo2D.RenderGC.renderGcId;

        var lungRenderInfo = new ThRenderImageInfo();
        if (!GenerateRenderGraphicContext(lungRenderInfo))
        {
            Log.Warn("GenerateRenderGraphicContext2({0}, {1}) failed",
                volumeInfoKey, type);
            GetLastError(out var errCode, out var errMessage);
            throw new WarningException(errCode, errMessage);
        }
        volInfo.LungFieldWindowInfo.RenderGcId = lungRenderInfo.RenderGC.renderGcId;

        var windowInfo = volInfo.MediastinumWindowInfo;
        if (type == RenderingSeriesType.Haiya)
        {
            volInfo.RenderImageInfo2D = lungRenderInfo;
            windowInfo = volInfo.LungFieldWindowInfo;
        }

        var renderInfo = volInfo.RenderImageInfo2D!;
        var renderGc = renderInfo.RenderGC;
        renderGc.renderGcId = windowInfo.RenderGcId;
        renderGc.windowLevle = windowInfo.WindowLevel;
        renderGc.windowWidth = windowInfo.WindowWidth;

        volInfo.RenderImageInfo2D.RenderGC = renderGc;

        return true;
    }
    public static bool GenerateRenderGraphicContextFor3D(int volumeInfoKey)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo))
        {
            TryGetValueErrorLog("GenerateRenderGraphicContextFor3D", volumeInfoKey);
            return false;
        }
        volInfo.RenderImageInfo3D ??= new ThRenderImageInfo();

        if (!GenerateRenderGraphicContext(volInfo.RenderImageInfo3D))
        {
            Log.Warn("GenerateRenderGraphicContext({0}) failed", volumeInfoKey);
            GetLastError(out var errCode, out var errMessage);
            throw new WarningException(errCode, errMessage);
        }
        return true;
    }
    private static bool GenerateRenderGraphicContext(ThRenderImageInfo? targetRenderGc)
    {
        if (!ExecuteRenderGCCommand(new CsCmd { cmdID = (int)ExecCmdType.RenderCreateGC }, out var renderGc)) return false;
        if (targetRenderGc == null) targetRenderGc = new ThRenderImageInfo() { RenderGC = renderGc };
        targetRenderGc.RenderGC = renderGc;
        return true;
    }

    // DLL側のGC情報を取得する
    public static bool UpdateRenderGraphicContextFor2D(int volumeInfoKey)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo) || volInfo.RenderImageInfo2D == null)
        {
            TryGetValueErrorLog("UpdateRenderGraphicContextFor2D", volumeInfoKey);
            return false;
        }
        return UpdateRenderGraphicContext(volInfo.RenderImageInfo2D);
    }
    public static bool UpdateRenderGraphicContextFor3D(int volumeInfoKey)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo) || volInfo.RenderImageInfo3D == null)
        {
            TryGetValueErrorLog("UpdateRenderGraphicContextFor3D", volumeInfoKey);
            return false;
        }
        return UpdateRenderGraphicContext(volInfo.RenderImageInfo3D);
    }
    private static bool UpdateRenderGraphicContext(ThRenderImageInfo targetRenderGc)
    {
        if (!ExecuteRenderGCCommand(new CsCmd { cmdID = (int)ExecCmdType.RenderQueryGC, intParam = targetRenderGc.RenderGC.renderGcId }, out var renderGc)) return false;
        targetRenderGc.RenderGC = renderGc;
        return true;
    }
    private static bool ExecuteRenderGCCommand(CsCmd cmd, out CsThProcLibRenderGC renderGc)
    {
        renderGc = new CsThProcLibRenderGC();

        var result = NativeMethods.RenderGC(ref cmd, ref renderGc);
        if (result != 0)
        {
            Log.Fatal("th_lib_render_GC failed");
            Log.Fatal("  CsCmd(cmdId:{0} strParam1:{1} strParam2:{2} intParam:{3} floatParam:{4})",
                cmd.cmdID, cmd.strParam1, cmd.strParam2, cmd.intParam, cmd.floatParam);
            return false;
        }

        Log.Trace("th_lib_render_GC in(renderGcId:{0} strParam1:{1} strParam2:{2} intParam:{3} floatParam:{4})",
                            cmd.cmdID, cmd.strParam1, cmd.strParam2, cmd.intParam, cmd.floatParam);

        Log.Trace("                 out(renderGcId:{0} renderCmdMajor:{1} renderCmdMinor:{2}",
            renderGc.renderGcId, renderGc.renderCmdMajor, renderGc.renderCmdMinor);
        Log.Trace("                     targeSizeX:{0} targeSizeY:{1} zoom:{2} panX:{3} panY:{4}",
            renderGc.targeSizeX, renderGc.targeSizeY, renderGc.zoom, renderGc.panX, renderGc.panY);
        Log.Trace("                     slicePosition:{0} displayParts:{1} studyCase:{2}",
            renderGc.slicePosition, renderGc.displayParts, renderGc.studyCase);
        Log.Trace("                     renderPreset:{0} windowLevle:{1} windowWidth:{2}",
            renderGc.renderPreset, renderGc.windowLevle, renderGc.windowWidth);
        Log.Trace("                     displayMode:{0} mouseType:{1} mousePosX:{2} mousePosY:{3}",
            renderGc.displayMode, renderGc.mouseType, renderGc.mousePosX, renderGc.mousePosY);

        return true;
    }


    // Volumeデータリリース
    public static bool ReleaseVolume(int volumeInfoKey)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo))
        {
            TryGetValueErrorLog("ReleaseVolume", volumeInfoKey);
            return false;
        }
        if (volInfo.RenderImageInfo2D != null || volInfo.RenderImageInfo3D != null)
        {
            Log.Warn("ReleaseVolume {0} is not yet released.", (volInfo.RenderImageInfo2D is null ? "RenderImageInfo3D" : "RenderImageInfo2D"));
            return false;
        }

        if (ExecuteCommand(new CsCmd { cmdID = (int)ExecCmdType.ReleaseVol3D, intParam = volInfo.VolumeInfo.volID }, out _) != 0)
        {
            Log.Error("ReleaseVolume ExecuteCommand(ReleaseVol3D, volId:{0}) failed.", volInfo.VolumeInfo.volID);
        }

        VolumeRenderInfos.Remove(volumeInfoKey);
        return true;
    }
    private static int GetVolumeInfo(int cmdId, int volId, out CsThProcLibVol3DData output)
    {
        int result;

        var cmd = new CsCmd { cmdID = cmdId, intParam = volId };
        output = new CsThProcLibVol3DData();
        result = NativeMethods.GetVolumeInfo(ref cmd, ref output);
        if (result != 0)
        {
            Log.Error("th_lib_get_vol_info failed.");
            Log.Error("  CsCmd(cmdId:{0} strParam1:{1} strParam2:{2} intParam:{3} floatParam:{4} failed.",
                cmd.cmdID, cmd.strParam1, cmd.strParam2, cmd.intParam, cmd.floatParam);
        }
        Log.Trace("th_lib_get_vol_info in(cmdId:{0} strParam1:{1} strParam2:{2} intParam:{3} floatParam:{4})",
                            cmd.cmdID, cmd.strParam1, cmd.strParam2, cmd.intParam, cmd.floatParam);
        Log.Trace("                    out(volID:{0} sizeX:{1} sizeY:{2} sizeZ:{3} ptichX:{3} ptichY:{4} ptichZ:{5})",
                            output.volID, output.sizeX, output.sizeY, output.sizeZ, output.ptichX, output.ptichY, output.ptichZ);
        return result;
    }
    // Renderingリソース一括実行
    public static bool ReleaseRenderingFor2D(int volumeInfoKey)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo) || volInfo.RenderImageInfo2D == null)
        {
            TryGetValueErrorLog("ReleaseRenderingFor2D", volumeInfoKey);
            return false;
        }

        if (!ExecuteRenderingReleaseImage(
            volInfo.MediastinumWindowInfo.RenderGcId,
            volInfo.LungFieldWindowInfo.RenderGcId))
        {
            Log.Error("ReleaseRenderingFor2D ExecuteRenderingReleaseImage failed");
            return false;
        }

        volInfo.RenderImageInfo2D = null;
        return true;
    }
    public static bool ReleaseRenderingFor3D(int volumeInfoKey)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo) || volInfo.RenderImageInfo3D == null)
        {
            TryGetValueErrorLog("ReleaseRenderingFor3D", volumeInfoKey);
            return false;
        }

        if (!ExecuteRenderingReleaseImage(volInfo.RenderImageInfo3D.RenderGC.renderGcId))
        {
            Log.Error("ReleaseRenderingFor3D ExecuteRenderingReleaseImage failed");
            return false;
        }

        volInfo.RenderImageInfo3D = null;
        return true;
    }
    private static bool ExecuteRenderingReleaseImage(int renderGcId, int renderGcIdSub = -1)
    {
        if (ExecuteCommand(new CsCmd { cmdID = (int)ExecCmdType.ReleaseGC, intParam = renderGcId }, out _) != 0)
        {
            Log.Error("ExecuteRenderingReleaseImage ExecuteCommand(ReleaseGC, renderGcId{0}) failed.", renderGcId);
            return false;
        }

        if (renderGcIdSub >= 0)
        {
            if (ExecuteCommand(new CsCmd { cmdID = (int)ExecCmdType.ReleaseGC, intParam = renderGcIdSub }, out _) != 0)
            {
                Log.Error("ExecuteRenderingReleaseImage ExecuteCommand(ReleaseGC, renderGcIdSub{0}) failed.", renderGcIdSub);
                return false;
            }
        }

        return true;
    }

    // 2DRenderingコマンドの発行
    private static bool ExecuteRender(CsCmd cmd, ThRenderImageInfo renderInfo)
    {
        var gcParam = renderInfo.RenderGC;
        var image = renderInfo.Image2D;
        var result = NativeMethods.ExecuteRender(ref cmd, ref gcParam, ref image);
        if (result != 0)
        {
            Log.Error("th_lib_exe_render failed.");
            Log.Error("  CsCmd(cmdId:{0} strParam1:{1} strParam2:{2} intParam:{3} floatParam:{4})",
                cmd.cmdID, cmd.strParam1, cmd.strParam2, cmd.intParam, cmd.floatParam);
            Log.Error("  CsThProcLibRenderGC(renderGcId:{0} renderCmdMajor:{1} targeSizeX:{2} targeSizeY:{3}",
                gcParam.renderGcId, gcParam.renderCmdMajor, gcParam.targeSizeX, gcParam.targeSizeY);
            Log.Error("                      seriesType:{0} zoom:{1} panX:{2} panY:{3}",
                gcParam.seriesType, gcParam.zoom, gcParam.panX, gcParam.panY);
            Log.Error("                      slicePosition:{0} renderPreset:{1} windowLevle:{2} windowWidth:{3}",
                gcParam.slicePosition, gcParam.renderPreset, gcParam.windowLevle, gcParam.windowWidth);
            Log.Error("                      displayMode:{0} mouseType:{1} mousePosX:{2} mousePosY:{3}",
                gcParam.displayMode, gcParam.mouseType, gcParam.mousePosX, gcParam.mousePosY);

            return false;
        }
        renderInfo.RenderGC = gcParam;
        renderInfo.Image2D = image;


        Log.Trace("th_lib_render_GC in(renderGcId:{0} strParam1:{1} strParam2:{2} intParam:{3} floatParam:{4})",
                            cmd.cmdID, cmd.strParam1, cmd.strParam2, cmd.intParam, cmd.floatParam);
        Log.Trace("                 outGc(renderGcId:{0} renderCmdMajor:{1} renderCmdMinor:{2}",
            gcParam.renderGcId, gcParam.renderCmdMajor, gcParam.renderCmdMinor);
        Log.Trace("                       targeSizeX:{0} targeSizeY:{1} zoom:{2} panX:{3} panY:{4}",
            gcParam.targeSizeX, gcParam.targeSizeY, gcParam.zoom, gcParam.panX, gcParam.panY);
        Log.Trace("                       slicePosition:{0} displayParts:{1} studyCase:{2}",
            gcParam.slicePosition, gcParam.displayParts, gcParam.studyCase);
        Log.Trace("                       renderPreset:{0} windowLevle:{1} windowWidth:{2}",
            gcParam.renderPreset, gcParam.windowLevle, gcParam.windowWidth);
        Log.Trace("                       displayMode:{0} mouseType:{1} mousePosX:{2} mousePosY:{3}",
            gcParam.displayMode, gcParam.mouseType, gcParam.mousePosX, gcParam.mousePosY);
        Log.Trace("                 outImage(imageID:{0} sizeX:{1} sizeY:{2} ptichX:{3} ptichY:{4} format:{5} next_render_cmds:{6})",
            image.imageID, image.sizeX, image.sizeY, image.ptichX, image.ptichY, image.format, image.next_render_cmds);
        return true;
    }

    public static bool StartOverMarker(int volumeInfoKey)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo) ||
            volInfo.RenderImageInfo2D == null)
        {
            TryGetValueErrorLog("StartOverMarker", volumeInfoKey);
            return false;
        }

        if (!ExecuteCTMarkerClearCommand(volInfo.VolumeInfo.volID, volInfo.RenderImageInfo2D))
        {
            Log.Error("StartOverMarker ExecuteCTMarkerClearCommand(volId:{0}) failed.", volInfo.VolumeInfo.volID);
            GetLastError(out var errCode, out var errMessage);
            throw new WarningException(errCode, errMessage);
        }

        var flags = (RequestRenderCmd)volInfo.RenderImageInfo2D.Image2D.next_render_cmds;
        if ((flags & RequestRenderCmd.Dim2Pattern1) == RequestRenderCmd.Dim2Pattern1) WeakReferenceMessenger.Default.Send<ImageUpdate, string>(new ImageUpdate(), "ImageUpdate2D");
        if ((flags & RequestRenderCmd.Dim3) == RequestRenderCmd.Dim3) WeakReferenceMessenger.Default.Send<ImageUpdate, string>(new ImageUpdate(), "ImageUpdate3D");

        return true;
    }
    private static bool ExecuteCTMarkerClearCommand(int volId, ThRenderImageInfo renderInfo)
        => ExecuteRender(new CsCmd { cmdID = (int)ExecCmdType.CTMarkerClear, intParam = volId }, renderInfo);

    public static bool FixedMarker(int volumeInfoKey)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo) ||
            volInfo.RenderImageInfo2D == null)
        {
            TryGetValueErrorLog("FixedMarker", volumeInfoKey);
            return false;
        }

        if (!ExecuteCTMarkerFinalCommand(volInfo.VolumeInfo.volID, volInfo.RenderImageInfo2D))
        {
            GetLastError(out var errCode, out var errMessage);
            throw new WarningException(errCode, errMessage);
        }
        return true;
    }
    private static bool ExecuteCTMarkerFinalCommand(int volId, ThRenderImageInfo renderInfo)
        => ExecuteRender(new CsCmd { cmdID = (int)ExecCmdType.CTMarkerFinal, intParam = volId }, renderInfo);

    private static bool RenderingOperation(int volId, ThRenderImageInfo renderInfo)
        => ExecuteRender(new CsCmd { cmdID = (int)ExecCmdType.RenderOp, intParam = volId }, renderInfo);
    private static bool RenderingImage3D(int volId, ThRenderImageInfo renderInfo)
        => ExecuteRender(new CsCmd { cmdID = (int)ExecCmdType.RenderVol3D, intParam = volId }, renderInfo);
    public static BitmapSource? UpdateImageRoiClearFor2D(int volumeInfoKey)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo) ||
            volInfo.RenderImageInfo2D == null)
        {
            TryGetValueErrorLog("UpdateImageRoiClearFor2D", volumeInfoKey);
            var errMsg = MessageService.GetMessage(MessageCode.VolumeKeyUndefined);
            throw new WarningException("00000", errMsg);
        }

        if (!ExecuteRender(new CsCmd { cmdID = (int)ExecCmdType.RoiClear, intParam = volInfo.VolumeInfo.volID }, volInfo.RenderImageInfo2D))
        {
            Log.Warn("UpdateImageRoiClearFor2D ExecuteRender failed");
            GetLastError(out var errCode, out var errMessage);
            throw new WarningException(errCode, errMessage);
        }

        var flags = (RequestRenderCmd)volInfo.RenderImageInfo2D.Image2D.next_render_cmds;
        var ret = ((flags & RequestRenderCmd.Dim2Pattern1) == RequestRenderCmd.Dim2Pattern1) ? UpdateImage(volInfo.VolumeInfo.volID, volInfo.RenderImageInfo2D, true) : null;
        if ((flags & RequestRenderCmd.Dim3) == RequestRenderCmd.Dim3) WeakReferenceMessenger.Default.Send<ImageUpdate, string>(new ImageUpdate(), "ImageUpdate3D");

        return ret;
    }
    public static void SwitchToRoiPenMode(int volumeInfoKey)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo) || volInfo.RenderImageInfo2D == null)
        {
            TryGetValueErrorLog("SwitchToRoiPenMode", volumeInfoKey);
            var errMsg = MessageService.GetMessage(MessageCode.VolumeKeyUndefined);
            throw new WarningException("00000", errMsg);
        }

        if (!ExecuteRender(new CsCmd { cmdID = (int)ExecCmdType.RoiAdd, intParam = volInfo.VolumeInfo.volID }, volInfo.RenderImageInfo2D))
        {
            GetLastError(out var errCode, out var errMessage);
            throw new WarningException(errCode, errMessage);
        }
    }
    public static void SwitchToRoiEraseMode(int volumeInfoKey)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo) || volInfo.RenderImageInfo2D == null)
        {
            TryGetValueErrorLog("SwitchToRoiEraseMode", volumeInfoKey);
            var errMsg = MessageService.GetMessage(MessageCode.VolumeKeyUndefined);
            throw new WarningException("00000", errMsg);
        }

        if (!ExecuteRender(new CsCmd { cmdID = (int)ExecCmdType.RoiErase, intParam = volInfo.VolumeInfo.volID }, volInfo.RenderImageInfo2D))
        {
            GetLastError(out var errCode, out var errMessage);
            throw new WarningException(errCode, errMessage);
        }
    }
    public static void ExecuteRoiFix(int volumeInfoKey)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo) || volInfo.RenderImageInfo2D == null)
        {
            TryGetValueErrorLog("ExecuteRoiFix", volumeInfoKey);
            var errMsg = MessageService.GetMessage(MessageCode.VolumeKeyUndefined);
            throw new WarningException("00000", errMsg);
        }

        if (!ExecuteRender(new CsCmd { cmdID = (int)ExecCmdType.RoiFinal, intParam = volInfo.VolumeInfo.volID }, volInfo.RenderImageInfo2D))
        {
            GetLastError(out var errCode, out var errMessage);
            throw new WarningException(errCode, errMessage);
        }
    }
    public static void SetRoiPenThickness(PenType penType, int volumeInfoKey)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo) || volInfo.RenderImageInfo2D == null)
        {
            TryGetValueErrorLog("SetRoiPenThickness", volumeInfoKey);
            var errMsg = MessageService.GetMessage(MessageCode.VolumeKeyUndefined);
            throw new WarningException("00000", errMsg);
        }

        if (!ExecuteRender(new CsCmd { cmdID = (int)ExecCmdType.RoiSetPen, intParam = (int)penType }, volInfo.RenderImageInfo2D))
        {
            GetLastError(out var errCode, out var errMessage);
            throw new WarningException(errCode, errMessage);
        }
    }
    public static int GetVolumeZSlice(int volumeInfoKey)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo))
        {
            TryGetValueErrorLog("GetVolumeZSlice", volumeInfoKey);
            return 0;
        }

        return volInfo.VolumeInfo.sizeZ;
    }
    public static BitmapSource? UpdateImageFor2D(int volumeInfoKey)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo) ||
            volInfo.RenderImageInfo2D == null)
        {
            TryGetValueErrorLog("UpdateImageFor2D", volumeInfoKey);
            return new BitmapImage();
        }
        return UpdateImage(volInfo.VolumeInfo.volID, volInfo.RenderImageInfo2D, true);
    }
    public static BitmapSource? UpdateImageFor3D(int volumeInfoKey)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo) ||
            volInfo.RenderImageInfo3D == null)
        {
            TryGetValueErrorLog("UpdateImageFor3D", volumeInfoKey);
            return new BitmapImage();
        }
        return UpdateImage(volInfo.VolumeInfo.volID, volInfo.RenderImageInfo3D, false);
    }

    private static WriteableBitmap? UpdateImage(int volId, ThRenderImageInfo renderInfo, bool is2D)
    {
        var renderGc = renderInfo.RenderGC;
        if (is2D)
        {
            renderGc.renderCmdMajor = (int)RenderingCmdType.Dim2;
            renderInfo.RenderGC = renderGc;
        }
        else
        {
            renderGc.renderCmdMajor = (int)RenderingCmdType.Dim3;
            renderInfo.RenderGC = renderGc;
        }

        if (!ThProc.RenderingImage3D(volId, renderInfo))
        {
            Log.Error("UpdateImage RenderingImage3D failed");
            return null;
        }

        var retImage = ThProc.Get2DImageData(renderInfo.Image2D);

        if (ExecuteCommand(
            new CsCmd { cmdID = (int)ExecCmdType.ReleaseImage2D, intParam = renderInfo.Image2D.imageID }, 
            out _) != 0)
        {
            Log.Error("ReleaseImage2D failed");
            return null;
        }

        return retImage;
    }

    public static BitmapSource? InitializeImage2D(
        int volumeInfoKey, 
        double width, double height,
        RenderingSeriesType type,
        double zoomScale,
        int sliceNumber)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo) || volInfo.RenderImageInfo2D == null)
        {
            TryGetValueErrorLog("InitializeImage2D", volumeInfoKey);
            return new BitmapImage();
        }

        volInfo.LinkedSeriesType = type;

        var retImage = InitializeImage(
            volInfo.VolumeInfo.volID,
            volInfo.RenderImageInfo2D,
            width, height,
            volInfo.LinkedSeriesType,
            zoomScale,
            volInfo.LinkedDisplayMode,
            true);

        if (retImage is null)
        {
            GetLastError(out var errCode, out var errMessage);
            throw new WarningException(errCode, errMessage);
        }

        return retImage;
    }
    public static BitmapSource? InitializeImage3D(
        int volumeInfoKey,
        double width, double height,
        double zoomScale)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo) || volInfo.RenderImageInfo3D == null)
        {
            TryGetValueErrorLog("InitializeImage3D", volumeInfoKey);
            return new BitmapImage();
        }

        var retImage = InitializeImage(
            volInfo.VolumeInfo.volID,
            volInfo.RenderImageInfo3D,
            width, height,
            volInfo.LinkedSeriesType,
            zoomScale,
            volInfo.LinkedDisplayMode,
            false);

        if (retImage is null)
        {
            GetLastError(out var errCode, out var errMessage);
            throw new WarningException(errCode, errMessage);
        }

        return retImage;
    }
    public static bool SetLinkedDisplayMode(int volumeInfoKey, RenderingDisplayMode linkedDisplayMode, bool is2DLinked = true, bool is3DLinked = true)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo))
        {
            TryGetValueErrorLog("SetLinkedDisplayMode", volumeInfoKey);
            return false;
        }
        volInfo.LinkedDisplayMode = linkedDisplayMode;

        if (is2DLinked)
        {
            if (volInfo.RenderImageInfo2D != null)
            {
                var renderInfo = volInfo.RenderImageInfo2D!;
                var renderGc = renderInfo.RenderGC;

                renderGc.displayMode = (int)volInfo.LinkedDisplayMode;
                renderInfo.RenderGC = renderGc;
            }
        }

        if (is3DLinked)
        {
            if (volInfo.RenderImageInfo3D != null)
            {
                var renderInfo = volInfo.RenderImageInfo3D!;
                var renderGc = renderInfo.RenderGC;

                renderGc.displayMode = (int)volInfo.LinkedDisplayMode;
                renderInfo.RenderGC = renderGc;
            }
        }
        return true;
    }
    private static BitmapSource? InitializeImage(
        int volId, ThRenderImageInfo volInfo, 
        double width, double height,
        RenderingSeriesType type,
        double zoomScale,
        RenderingDisplayMode dispMode,
        bool is2D)
    {
        var renderGc = volInfo.RenderGC;

        renderGc.targeSizeX = (int)width;
        renderGc.targeSizeY = (int)height;
        renderGc.seriesType = (int)type;
        renderGc.zoom = (float)zoomScale;
        renderGc.displayMode = (int)dispMode;

        volInfo.RenderGC = renderGc;

        return UpdateImage(volId, volInfo, is2D);
    }

    public static BitmapSource? UpdateImageOnSwitchCTModeFor2DandLinked3D(int volumeInfoKey, RenderingSeriesType type, bool is3DLinked = false)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo)
            || volInfo.RenderImageInfo2D == null)
        {
            TryGetValueErrorLog("UpdateImageOnSwitchCTModeFor2DandLinked3D", volumeInfoKey);
            var errMsg = MessageService.GetMessage(MessageCode.VolumeKeyUndefined);
            throw new WarningException("00000", errMsg);
        }

        volInfo.LinkedSeriesType = type;

        var renderInfo = volInfo.RenderImageInfo2D!;
        var renderGc = renderInfo.RenderGC;

        renderGc.renderCmdMajor = (int)RenderingCmdType.SetupSeries;
        renderGc.seriesType = (int)volInfo.LinkedSeriesType;

        var windowInfo = volInfo.LinkedSeriesType switch
        {
            RenderingSeriesType.Juukaku => volInfo.MediastinumWindowInfo,
            RenderingSeriesType.Haiya => volInfo.LungFieldWindowInfo,
            _ => volInfo.MediastinumWindowInfo,
        };
        renderGc.renderGcId = windowInfo.RenderGcId;
        renderGc.windowLevle = windowInfo.WindowLevel;
        renderGc.windowWidth = windowInfo.WindowWidth;

        volInfo.RenderImageInfo2D.RenderGC = renderGc;

        if (is3DLinked)
        {
            if (volInfo.RenderImageInfo3D != null)
            {
                renderInfo = volInfo.RenderImageInfo3D!;
                renderGc = renderInfo.RenderGC;

                renderGc.seriesType = (int)type;
                // windowLevel/windowWidthについては特に設定しなくてよい仕様。
                //renderGc.windowLevle = windowInfo.WindowLevel;
                //renderGc.windowWidth = windowInfo.WindowWidth;

                volInfo.RenderImageInfo3D.RenderGC = renderGc;
            }
            else
            {
                Log.Warn("UpdateImageOnSwitchCTModeFor2DandLinked3D RenderImageInfo3D is null.");
            }
        }

        if (!ThProc.RenderingOperation(volInfo.VolumeInfo.volID, volInfo.RenderImageInfo2D))
        {
            Log.Warn("UpdateImageOnSwitchCTModeFor2DandLinked3D RenderingOperation failed.");
            GetLastError(out var errCode, out var errMessage);
            throw new WarningException(errCode, errMessage);
        }

        if (volInfo.RenderImageInfo2D.Image2D.imageID != -1)
        {
            Log.Warn("UpdateImageOnSwitchCTModeFor2DandLinked3D RenderingOperation failed.");
        }

        var flags = (RequestRenderCmd)volInfo.RenderImageInfo2D.Image2D.next_render_cmds;
        var ret = ((flags & RequestRenderCmd.Dim2Pattern1) == RequestRenderCmd.Dim2Pattern1) ? UpdateImage(volInfo.VolumeInfo.volID, volInfo.RenderImageInfo2D, true) : null;
        if ((flags & RequestRenderCmd.Dim3) == RequestRenderCmd.Dim3) WeakReferenceMessenger.Default.Send<ImageUpdate, string>(new ImageUpdate(), "ImageUpdate3D");

        return ret;
    }

    public static BitmapSource? UpdateImageOnClickedMarkerFor2D(int volumeInfoKey, double x, double y)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo) ||
            volInfo.RenderImageInfo2D == null)
        {
            TryGetValueErrorLog("UpdateImageOnClickedMarkerFor2D", volumeInfoKey);
            var errMsg = MessageService.GetMessage(MessageCode.VolumeKeyUndefined);
            throw new WarningException("00000", errMsg);
        }

        var renderInfo = volInfo.RenderImageInfo2D!;
        var renderGc = renderInfo.RenderGC;

        renderGc.renderCmdMajor = (int)RenderingCmdType.ProcMouse;
        renderGc.mouseType = (int)MouseEventType.CTMarker2D;
        renderGc.mousePosX = (int)x;
        renderGc.mousePosY = (int)y;

        volInfo.RenderImageInfo2D.RenderGC = renderGc;

        if (!ThProc.RenderingOperation(volInfo.VolumeInfo.volID, volInfo.RenderImageInfo2D))
        {
            Log.Warn("UpdateImageOnClickedMarkerFor2D RenderingOperation failed.");
            GetLastError(out var errCode, out var errMessage);
            throw new WarningException(errCode, errMessage);
        }

        var flags = (RequestRenderCmd)volInfo.RenderImageInfo2D.Image2D.next_render_cmds;
        var ret = ((flags & RequestRenderCmd.Dim2Pattern1) == RequestRenderCmd.Dim2Pattern1) ? UpdateImage(volInfo.VolumeInfo.volID, volInfo.RenderImageInfo2D, true) : null;
        if ((flags & RequestRenderCmd.Dim3) == RequestRenderCmd.Dim3) WeakReferenceMessenger.Default.Send<ImageUpdate, string>(new ImageUpdate(), "ImageUpdate3D");

        return ret;
    }

    public static BitmapSource? UpdateImageOnClickedMarkerFor3D(int volumeInfoKey, double x, double y)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo) ||
            volInfo.RenderImageInfo3D == null)
        {
            TryGetValueErrorLog("UpdateImageOnClickedMarkerFor3D", volumeInfoKey);
            var errMsg = MessageService.GetMessage(MessageCode.VolumeKeyUndefined);
            throw new WarningException("00000", errMsg);
        }

        var renderInfo = volInfo.RenderImageInfo3D!;
        var renderGc = renderInfo.RenderGC;

        renderGc.renderCmdMajor = (int)RenderingCmdType.ProcMouse;
        renderGc.mouseType = (int)MouseEventType.CTMarker3D;
        renderGc.mousePosX = (int)x;
        renderGc.mousePosY = (int)y;

        volInfo.RenderImageInfo3D.RenderGC = renderGc;

        if (!ThProc.RenderingOperation(volInfo.VolumeInfo.volID, volInfo.RenderImageInfo3D))
        {
            Log.Warn("UpdateImageOnClickedMarkerFor3D RenderingOperation failed.");
            GetLastError(out var errCode, out var errMessage);
            throw new WarningException(errCode, errMessage);
        }

        var flags = (RequestRenderCmd)volInfo.RenderImageInfo3D.Image2D.next_render_cmds;
        var ret = ((flags & RequestRenderCmd.Dim3) == RequestRenderCmd.Dim3) ? UpdateImage(volInfo.VolumeInfo.volID, volInfo.RenderImageInfo3D, false) : null;
        if ((flags & RequestRenderCmd.Dim2Pattern1) == RequestRenderCmd.Dim2Pattern1) WeakReferenceMessenger.Default.Send<ImageUpdate, string>(new ImageUpdate(), "ImageUpdate2D");

        return ret;
    }

    public static void NotifyLeftButtonDownFor3D(int volumeInfoKey, double x, double y)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo) ||
            volInfo.RenderImageInfo3D == null)
        {
            TryGetValueErrorLog("NotifyLeftButtonDownFor3D", volumeInfoKey);
            return;
        }

        var renderInfo = volInfo.RenderImageInfo3D!;
        var renderGc = renderInfo.RenderGC;

        renderGc.renderCmdMajor = (int)RenderingCmdType.ProcMouse;
        renderGc.mouseType = (int)MouseEventType.LeftButtonDown;
        renderGc.mousePosX = (int)x;
        renderGc.mousePosY = (int)y;

        volInfo.RenderImageInfo3D.RenderGC = renderGc;

        if (!ThProc.RenderingOperation(volInfo.VolumeInfo.volID, volInfo.RenderImageInfo3D))
        {
            Log.Error("NotifyLeftButtonDownFor3D RenderingOperation failed.");
        }
    }

    public static BitmapSource? UpdateImageRotationFor3D(int volumeInfoKey, double x, double y)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo) ||
            volInfo.RenderImageInfo3D == null)
        {
            TryGetValueErrorLog("UpdateImageRotationFor3D", volumeInfoKey);
            return new BitmapImage();
        }

        var renderInfo = volInfo.RenderImageInfo3D!;
        var renderGc = renderInfo.RenderGC;

        renderGc.renderCmdMajor = (int)RenderingCmdType.ProcMouse;
        renderGc.mouseType = (int) MouseEventType.Rotation3D;
        renderGc.mousePosX = (int)x;
        renderGc.mousePosY = (int)y;

        volInfo.RenderImageInfo3D.RenderGC = renderGc;

        if (!ThProc.RenderingOperation(volInfo.VolumeInfo.volID, volInfo.RenderImageInfo3D))
        {
            Log.Error("UpdateImageRotationFor3D RenderingOperation failed.");
        }

        return UpdateImage(volInfo.VolumeInfo.volID, volInfo.RenderImageInfo3D, false);
    }

    public static BitmapSource? UpdateImageOnPaintLeftUpDownFor2D(
        int volumeInfoKey, double x, double y, bool isUp)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo) ||
            volInfo.RenderImageInfo2D == null)
        {
            TryGetValueErrorLog("UpdateImageOnPaintLeftUpDownFor2D", volumeInfoKey);
            return new BitmapImage();
        }

        var renderInfo = volInfo.RenderImageInfo2D;
        var renderGc = renderInfo.RenderGC;

        renderGc.renderCmdMajor = (int)RenderingCmdType.ProcMouse;
        renderGc.mouseType = isUp ? (int)MouseEventType.LeftButtonDown : (int)MouseEventType.LeftButtonUp;
        renderGc.mousePosX = (int)x;
        renderGc.mousePosY = (int)y;

        volInfo.RenderImageInfo2D.RenderGC = renderGc;

        if (!ThProc.RenderingOperation(volInfo.VolumeInfo.volID, volInfo.RenderImageInfo2D))
        {
            Log.Error("UpdateImageOnPaintLeftUpDownFor2D RenderingOperation failed.");
        }

        var flags = (RequestRenderCmd)volInfo.RenderImageInfo2D.Image2D.next_render_cmds;
        var ret = ((flags & RequestRenderCmd.Dim2Pattern1) == RequestRenderCmd.Dim2Pattern1) ? UpdateImage(volInfo.VolumeInfo.volID, volInfo.RenderImageInfo2D, true) : null;
        if ((flags & RequestRenderCmd.Dim3) == RequestRenderCmd.Dim3) WeakReferenceMessenger.Default.Send<ImageUpdate, string>(new ImageUpdate(), "ImageUpdate3D");

        return ret;
    }

    public static BitmapSource? UpdateImageOnPaintFor2D(int volumeInfoKey, double x, double y, bool isPen)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo) ||
            volInfo.RenderImageInfo2D == null)
        {
            TryGetValueErrorLog("UpdateImageOnPaintFor2D", volumeInfoKey);
            return new BitmapImage();
        }

        var renderInfo = volInfo.RenderImageInfo2D;
        var renderGc = renderInfo.RenderGC;

        renderGc.renderCmdMajor = (int)RenderingCmdType.ProcMouse;
        renderGc.mouseType = isPen ? (int)MouseEventType.RoiPen : (int)MouseEventType.RoiEraser;
        renderGc.mousePosX = (int)x;
        renderGc.mousePosY = (int)y;

        volInfo.RenderImageInfo2D.RenderGC = renderGc;

        if (!ThProc.RenderingOperation(volInfo.VolumeInfo.volID, volInfo.RenderImageInfo2D))
        {
            Log.Error("UpdateImageOnPaintFor2D RenderingOperation failed.");
        }

        var flags = (RequestRenderCmd)volInfo.RenderImageInfo2D.Image2D.next_render_cmds;
        var ret = ((flags & RequestRenderCmd.Dim2Pattern1) == RequestRenderCmd.Dim2Pattern1) ? UpdateImage(volInfo.VolumeInfo.volID, volInfo.RenderImageInfo2D, true) : null;
        if ((flags & RequestRenderCmd.Dim3) == RequestRenderCmd.Dim3) WeakReferenceMessenger.Default.Send<ImageUpdate, string>(new ImageUpdate(), "ImageUpdate3D");

        return ret;
    }

    public static BitmapSource? UpdateImageOnDisplayPartsChangedFor3D(int volumeInfoKey, DisplayPartsType parts, bool isOn)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo) ||
            volInfo.RenderImageInfo3D == null)
        {
            TryGetValueErrorLog("UpdateImageOnDisplayPartsChangedFor3D", volumeInfoKey);
            var errMsg = MessageService.GetMessage(MessageCode.VolumeKeyUndefined);
            throw new WarningException("00000", errMsg);
        }

        var renderInfo = volInfo.RenderImageInfo3D;
        var renderGc = renderInfo.RenderGC;

        renderGc.renderCmdMajor = (int)RenderingCmdType.Dim3SetupObj;
        renderGc.displayParts = (int)parts;

        volInfo.RenderImageInfo3D.RenderGC = renderGc;

        if (!ThProc.RenderingOperation(volInfo.VolumeInfo.volID, volInfo.RenderImageInfo3D))
        {
            Log.Warn("UpdateImageOnDisplayPartsChangedFor3D RenderingOperation failed.");
            GetLastError(out var errCode, out var errMessage);
            throw new WarningException(errCode, errMessage);
        }

        var flags = (RequestRenderCmd)volInfo.RenderImageInfo3D.Image2D.next_render_cmds;
        var ret = ((flags & RequestRenderCmd.Dim3) == RequestRenderCmd.Dim3) ? UpdateImage(volInfo.VolumeInfo.volID, volInfo.RenderImageInfo3D, false) : null;
        if ((flags & RequestRenderCmd.Dim2Pattern1) == RequestRenderCmd.Dim2Pattern1) WeakReferenceMessenger.Default.Send<ImageUpdate, string>(new ImageUpdate(), "ImageUpdate2D");

        return ret;
    }

    public static BitmapSource? UpdateImageOnStudyCaseChangedFor3D(int volumeInfoKey, StudyCaseType type)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo) ||
            volInfo.RenderImageInfo3D == null)
        {
            TryGetValueErrorLog("UpdateImageOnStudyCaseChangedFor3D", volumeInfoKey);
            var errMsg = MessageService.GetMessage(MessageCode.VolumeKeyUndefined);
            throw new WarningException("00000", errMsg);
        }

        var renderInfo = volInfo.RenderImageInfo3D;
        var renderGc = renderInfo.RenderGC;

        renderGc.renderCmdMajor = (int)RenderingCmdType.ChangeCase;
        renderGc.studyCase = (int)type;

        volInfo.RenderImageInfo3D.RenderGC = renderGc;

        if (!ThProc.RenderingOperation(volInfo.VolumeInfo.volID, volInfo.RenderImageInfo3D))
        {
            Log.Warn("UpdateImageOnStudyCaseChangedFor3D RenderingOperation failed.");
            GetLastError(out var errCode, out var errMessage);
            throw new WarningException(errCode, errMessage);
        }

        var flags = (RequestRenderCmd)volInfo.RenderImageInfo3D.Image2D.next_render_cmds;
        var ret = ((flags & RequestRenderCmd.Dim3) == RequestRenderCmd.Dim3) ? UpdateImage(volInfo.VolumeInfo.volID, volInfo.RenderImageInfo3D, false) : null;
        if ((flags & RequestRenderCmd.Dim2Pattern1) == RequestRenderCmd.Dim2Pattern1) WeakReferenceMessenger.Default.Send<ImageUpdate, string>(new ImageUpdate(), "ImageUpdate2D");

        return ret;
    }

    public static BitmapSource? UpdateImageZoomFor2D(int volumeInfoKey, double zoom)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo) ||
            volInfo.RenderImageInfo2D == null)
        {
            TryGetValueErrorLog("UpdateImageZoomFor2D", volumeInfoKey);
            var errMsg = MessageService.GetMessage(MessageCode.VolumeKeyUndefined);
            throw new WarningException("00000", errMsg);
        }

        return UpdateImageZoom(volInfo.VolumeInfo.volID, volInfo.RenderImageInfo2D, zoom, true);
    }

    public static BitmapSource? UpdateImageZoomFor3D(int volumeInfoKey, double zoom)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo) ||
            volInfo.RenderImageInfo3D == null)
        {
            TryGetValueErrorLog("UpdateImageZoomFor3D", volumeInfoKey);
            var errMsg = MessageService.GetMessage(MessageCode.VolumeKeyUndefined);
            throw new WarningException("00000", errMsg);
        }

        return UpdateImageZoom(volInfo.VolumeInfo.volID, volInfo.RenderImageInfo3D, zoom, false);
    }

    private static BitmapSource? UpdateImageZoom(int volId, ThRenderImageInfo renderInfo, double zoom, bool is2D)
    {
        var renderGc = renderInfo.RenderGC;

        renderGc.renderCmdMajor = (int)RenderingCmdType.Dim2;
        renderGc.zoom = (float)zoom;

        renderInfo.RenderGC = renderGc;

        var retImage = UpdateImage(volId, renderInfo, is2D);
        if (retImage == null)
        {
            Log.Warn("UpdateImageZoom UpdateImage(volId:{0}, is2D:{1}} is null.", volId, is2D);
            GetLastError(out var errCode, out var errMessage);
            throw new WarningException(errCode, errMessage);
        }
        return retImage;
    }

    public static BitmapSource? UpdateImageSliceNumberFor2D(int volumeInfoKey, int sliceNumber)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo) ||
            volInfo.RenderImageInfo2D == null)
        {
            TryGetValueErrorLog("UpdateImageSliceNumberFor2D", volumeInfoKey);
            var errMsg = MessageService.GetMessage(MessageCode.VolumeKeyUndefined);
            throw new WarningException("00000", errMsg);
        }

        var renderInfo = volInfo.RenderImageInfo2D!;
        var renderGc = renderInfo.RenderGC;

        renderGc.renderCmdMajor = (int)RenderingCmdType.Dim2;
        renderGc.slicePosition = (float)sliceNumber;

        volInfo.RenderImageInfo2D.RenderGC = renderGc;

        var retImage = UpdateImage(volInfo.VolumeInfo.volID, volInfo.RenderImageInfo2D, true);
        if (retImage == null)
        {
            Log.Warn("UpdateImageZoom UpdateImage(volId:{0}, is2D:{true}} is null.", volInfo.VolumeInfo.volID);
            GetLastError(out var errCode, out var errMessage);
            throw new WarningException(errCode, errMessage);
        }

        return retImage;
    }
    public static WriteableBitmap? UpdateImagePanChangedFor2D(int volumeInfoKey, double deltaX, double deltaY)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo) ||
            volInfo.RenderImageInfo2D == null)
        {
            TryGetValueErrorLog("UpdateImagePanChangedFor2D", volumeInfoKey);
            var errMsg = MessageService.GetMessage(MessageCode.VolumeKeyUndefined);
            throw new WarningException("00000", errMsg);
        }

        return UpdateImagePanChanged(volInfo.VolumeInfo.volID, volInfo.RenderImageInfo2D, deltaX, deltaY, true);
    }

    public static WriteableBitmap? UpdateImagePanChangedFor3D(int volumeInfoKey, double deltaX, double deltaY)
    {
        if (!VolumeRenderInfos.TryGetValue(volumeInfoKey, out var volInfo) ||
            volInfo.RenderImageInfo3D == null)
        {
            TryGetValueErrorLog("UpdateImagePanChangedFor3D", volumeInfoKey);
            var errMsg = MessageService.GetMessage(MessageCode.VolumeKeyUndefined);
            throw new WarningException("00000", errMsg);
        }

        return UpdateImagePanChanged(volInfo.VolumeInfo.volID, volInfo.RenderImageInfo3D, deltaX, deltaY, false);
    }

    private static WriteableBitmap? UpdateImagePanChanged(int volId, ThRenderImageInfo renderInfo, double deltaX, double deltaY, bool is2D)
    {
        var renderGc = renderInfo.RenderGC;

        renderGc.renderCmdMajor = (int)RenderingCmdType.Dim2;
        renderGc.panX += (float)deltaX;
        renderGc.panY += (float)deltaY;

        renderInfo.RenderGC = renderGc;

        Log.Debug("UpdateImagePanChanged panX:{0} panY{1} deltaX:{2} deltaY:{3} is2D:{4}", renderGc.panX, renderGc.panY, deltaX, deltaY, is2D);

        var retImage = UpdateImage(volId, renderInfo, is2D);
        if (retImage == null)
        {
            Log.Warn("UpdateImagePanChanged UpdateImage(volId:{0}, is2D:{1}} is null.", volId, is2D);
            GetLastError(out var errCode, out var errMessage);
            throw new WarningException(errCode, errMessage);
        }
        return retImage;
    }

    private static WriteableBitmap? Get2DImageData(CsThProcLibImage2DData imageData)
    {
        var formatByteSize = (RenderImageFormat)imageData.format switch
        {
            RenderImageFormat.Gray8 => 1,
            RenderImageFormat.Gray16 => 2,
            RenderImageFormat.RGB24 => 3,
            RenderImageFormat.ARGB32 => 4,
            _ => 1,
        };

        var pixelFormat = (RenderImageFormat)imageData.format switch
        {
            RenderImageFormat.Gray8 => PixelFormats.Gray8,
            RenderImageFormat.Gray16 => PixelFormats.Gray16,
            RenderImageFormat.RGB24 => PixelFormats.Rgb24,
            RenderImageFormat.ARGB32 => PixelFormats.Bgra32,
            _ => PixelFormats.Gray8,
        };

        if (Get2DImageData(imageData.imageID, imageData.sizeX * imageData.sizeY * formatByteSize, out var outBuffer) == 0)
        {
            var wb = new WriteableBitmap(imageData.sizeX, imageData.sizeY, 96, 96, pixelFormat, null);
            wb.WritePixels(new Int32Rect(0, 0, imageData.sizeX, imageData.sizeY), outBuffer, imageData.sizeX * formatByteSize, 0);

            return wb;
        }
        return null;
    }

    // 2D画像イメージの取得
    private static int Get2DImageData(int imageId, int bufferSize, out byte[] outBuffer)
    {
        var cmd = new CsCmd { cmdID = (int)ExecCmdType.RenderImage2D, intParam = imageId };
        outBuffer = new byte[bufferSize];
        var result = NativeMethods.GetData(ref cmd, outBuffer, bufferSize);
        if (result != 0)
        {
            Log.Error("th_lib_get_data failed.");
            Log.Error("  CsCmd(cmdId:{0} strParam1:{1} strParam2:{2} intParam:{3} floatParam:{4}), bufferSize:{5} failed.", 
                cmd.cmdID, cmd.strParam1, cmd.strParam2, cmd.intParam, cmd.floatParam, bufferSize);
        }
        Log.Trace("th_lib_get_data in(renderGcId:{0} strParam1:{1} strParam2:{2} intParam:{3} bufferSize:{5} )",
                            cmd.cmdID, cmd.strParam1, cmd.strParam2, cmd.intParam, cmd.floatParam, bufferSize);
        return result;
    }

    private static void TryGetValueErrorLog(string methodName, int volumeInfoKey)
    {
        Log.Error("{0} VolumeRenderInfos TryGetValue failed", methodName);
        Log.Error("  TryGetKey:{0} VolumeRenderInfos.Count:{1}", volumeInfoKey, VolumeRenderInfos.Count);
        foreach (var vol in VolumeRenderInfos.Keys)
        {
            Log.Error("  key:{0}", vol);
        }
    }

    [GeneratedRegex(@"^[A-Za-z]:(?:[\\/])?$")]
    private static partial Regex DriveRootPathRegex();
}
