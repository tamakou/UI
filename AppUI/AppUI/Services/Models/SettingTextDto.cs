using System.Text.Json.Serialization;
using AppUI.Core;

namespace AppUI.Services.Models;

public interface ISettingTextJson
{
    string ImportSettingText(SettingTextType textType);
}

public enum SettingTextType
{
    Manual,
    Version,
    Inquiry,
};
#if false // JSON形式は不採用 (後で整理)
public class SettingTextJson : JsonService<SettingTextDto>, ISettingTextJson
#endif
public class SettingTextJson : ISettingTextJson
{
    public SettingTextJson() { }
    public string ImportSettingText(SettingTextType textType)
    {
        var output = new ThProcCmdOutput();
        var commandResult = textType switch
        {
            SettingTextType.Manual => ThProc.ExecuteImportManualTextCommand(out output),
            SettingTextType.Version => ThProc.ExecuteImportVersionTextCommand(out output),
            SettingTextType.Inquiry => ThProc.ExecuteImportInquiryTextCommand(out output),
            _ => throw new WarningException("", ""),
        };

        if (!commandResult)
        {
            ThProc.GetLastError(out var errCode, out var errMessage);
            throw new WarningException(errCode, errMessage);
        }

        return output.StrParam1;

#if false // JSON形式は不採用 (後で整理)
        try {
            return Deserialize(output.StrParam1)?.Text ?? "";
        }
        catch
        {
            return "DummyText";
        }
#endif
    }
}

#if false // JSON形式は不採用 (後で整理)
public class SettingTextDto
{
    [JsonPropertyName("Text")]
    public string Text { get; set; } = "";
}
#endif