using NLog;
using NLog.Targets;

namespace AppUI.Services;

public interface ILogService
{
    void Initialize(string filePath);
    void Trace(string message, params object[] args);
    void Debug(string message, params object[] args);
    void Info(string message, params object[] args);
    void Warn(string message, params object[] args);
    void Error(string message, params object[] args);
    void Fatal(string message, params object[] args);
}

public class Log
{
    private static readonly Logger _logger = LogManager.GetCurrentClassLogger();
    private static readonly string SubFolderPath = "Log";
    private static readonly string LogFileName = "ThAppUI.log";

    public static void Trace(string message, params object[] args)
    {
        _logger.Trace(message, args);
    }
    public static void Debug(string message, params object[] args)
    {
        _logger.Debug(message, args);
    }
    public static void Info(string message, params object[] args)
    {
        _logger.Info(message, args);
    }
    public static void Warn(string message, params object[] args)
    {
        _logger.Warn(message, args);
    }
    public static void Error(string message, params object[] args)
    {
        _logger.Error(message, args);
    }
    public static void Fatal(string message, params object[] args)
    {
        _logger.Fatal(message, args);
    }
    public static void Initialize(string homeFolder)
    {
        var config = new NLog.Config.LoggingConfiguration();

        var baseFolder = (homeFolder == "") ? AppDomain.CurrentDomain.BaseDirectory : homeFolder;

        var logfile = new FileTarget("logfile")
        { 
            FileName = @$"{baseFolder}\{SubFolderPath}\{LogFileName}",
            Layout = "${longdate} ${level:uppsercase=true:padding=5} ${message}",
            KeepFileOpen = false,
            ArchiveFileName = @$"{baseFolder}\{SubFolderPath}\{LogFileName}",
            ArchiveSuffixFormat = "{0:0}",
            MaxArchiveFiles = 1,
            ArchiveAboveSize = 20 * 1024 * 1024// 20MB
        };

#if DEBUG
        config.AddRule(LogLevel.Debug, LogLevel.Fatal, logfile);
#else
        config.AddRule(LogLevel.Info, LogLevel.Fatal, logfile);
#endif
        NLog.LogManager.Configuration = config;

    }
}
