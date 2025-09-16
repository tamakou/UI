using System.Text.Encodings.Web;
using System.Text.Unicode;
using System.Text.Json;

namespace AppUI.Services;
public interface IJsonService<T>
{
    T? Deserialize(string jsonText, JsonSerializerOptions? specifyOption = null);

    string Serialize(T obj, JsonSerializerOptions? specifyOption = null);
}

public class JsonService<T> : IJsonService<T>
{
    readonly static JsonSerializerOptions options = new()
    {
        Encoder = JavaScriptEncoder.Create(UnicodeRanges.All), // 日本語変換するためのエンコード設定
        
        WriteIndented = true                                   // インデント
    };

    public T? Deserialize(string jsonText, JsonSerializerOptions? specifyOption = null)
    {
        var localOptions = specifyOption ?? options;
        return JsonSerializer.Deserialize<T>(jsonText, localOptions);
    }

    public string Serialize(T obj, JsonSerializerOptions? specifyOption = null)
    {
        if (obj == null) return "";
        var localOptions = specifyOption ?? options;
        return JsonSerializer.Serialize(obj, localOptions);
    }
}
