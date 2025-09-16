using System.Diagnostics;
using System.IO;
using AppUI.Core;
using AppUI.ViewModels;
using CommunityToolkit.Mvvm.Messaging;

namespace AppUI.Services;

public enum FolderPusherResult
{
    Success = 0,
    InvalidFolderPath,       // 引数エラー
    ADBCommandNotFound,
    ADBBootFailed,
    USBUnConnected,
    FolderNotExist,
    ExtractFolderNameFailed, // 引数エラー
    TargetDirCreateFailed,
    TransportFailed,
    InvalidPlatformName,     // 引数エラー
    AppleDevLaunchFailed,
    // ↑Batchファイルから返却される値↑
    // ↓以下はUI側定義↓
    UsersCanncel = 99,     // ユーザのキャンセル操作
    BatchFileNotExists,
    SourceFileNotFound,
    UnknownError = 999,
};

public class FolderPusherService
{
    readonly static string batchFileName = "push_folder_to_device.bat";
    readonly static string ScriptSubFolder = "Script";

    private static Process? BatchProcess;
    private static bool UserCanceld = false;

    public static bool BatchFileExists(string homeFolderPath)
    {
        return File.Exists($@"{homeFolderPath}\{ScriptSubFolder}\{batchFileName}");
    }
    public static async Task<FolderPusherResult> PushFolderToDeviceAsync(
        string homeFolderPath, 
        string meshFolderPath, 
        CTDataTransportType type,
        Action? onSuccess = null,
        Action? onFailed = null
        )
    {
        var platform = type switch
        {
            CTDataTransportType.iPhone => "iPhone",
            CTDataTransportType.Xreal => "Android",
            _ => throw new NotImplementedException()
        };

        UserCanceld = false;

        try
        {
            var workingDir = $@"{homeFolderPath}\{ScriptSubFolder}";
            BatchProcess = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = $@"{workingDir}\{batchFileName}",
                    Arguments = $"\"{meshFolderPath}\" \"{platform}\"",
                    WorkingDirectory = Path.GetDirectoryName(workingDir),
                    UseShellExecute = false,
                    CreateNoWindow = true,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true
                }
            };

            var outputBuilder = new System.Text.StringBuilder();
            BatchProcess.OutputDataReceived += (sender, e) =>
            {
                if (!string.IsNullOrEmpty(e.Data))
                {
                    outputBuilder.AppendLine(e.Data);
                    Log.Info(e.Data);
                }
            };

            BatchProcess.ErrorDataReceived += (sender, e) =>
            {
                if (!string.IsNullOrEmpty(e.Data))
                {
                    Log.Error($"{e.Data}");
                }
            };

            if (!BatchFileExists(homeFolderPath))
            {
                await Task.Delay(500);
                BatchProcess.Dispose();
                BatchProcess = null;
                onFailed?.Invoke();
                return FolderPusherResult.BatchFileNotExists;
            }

            BatchProcess.Start();
            BatchProcess.BeginOutputReadLine();
            BatchProcess.BeginErrorReadLine();

            await Task.Run(() => BatchProcess.WaitForExit());

            onSuccess?.Invoke();

            return (FolderPusherResult)BatchProcess.ExitCode;
        }
        catch(Exception)
        {
            if (UserCanceld == true) return FolderPusherResult.UsersCanncel;
            onFailed?.Invoke();
            throw;
        }
    }

    public static void KillProcess()
    {
        try
        {
            if (BatchProcess != null && !BatchProcess.HasExited)
            {
                UserCanceld = true;
                BatchProcess.Kill();
                BatchProcess.Dispose();
                BatchProcess = null;
            }
        }
        catch (Exception)
        {
            throw;
        }
    }
}
