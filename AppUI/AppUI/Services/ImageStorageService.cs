using System.Diagnostics;
using System.IO;
using AppUI.Core;
using System.Windows.Media.Imaging;
using System.Windows.Media;
using System.Windows;

namespace AppUI.Services;
public interface IImageStorageService
{
    string RootPath { get; set; }
    public string StudyDate { get; set; }
    public string Time { get; set; }
    public string StudyUID { get; set; }

    void CreateImageStorage();
    void DeleteImageStorage();
    int GetMediastinumMaxFileNum();
    int GetLungFieldMaxFileNum();
    void SaveImageMediastinum(int sliceNumber);
    void SaveImageLungField(int sliceNumber);
    string GetMediastinumFilePath(int sliceNumber);
    string GetLungFieldFilePath(int sliceNumber);
    void DeleteTempImage(int sliceNumber);
    public BitmapSource LoadImage(string path, ThProcLibDicomImageInfo dicomInfo);

}

public class ImageStorageService : IImageStorageService
{
    private readonly string _tempDir = Path.Combine(Path.GetTempPath(), "Zeus_PC_APP", "TempImages");
    public readonly string mediastinumFolderName = "haiya";   // 肺野フォルダ
    public readonly string lungFieldFolderName = "juukaku";   // 横隔フォルダ
    public readonly string imageExtension = "raw";

    public string RootPath { get; set; } = "";

    // ThProcPatientInfoの値を指定する
    public string StudyDate { get; set; } = "";
    public string Time { get; set; } = "";
    public string StudyUID { get; set; } = "";

    public ImageStorageService()
    {
    }
    public void CreateImageStorage()
    {
        Directory.CreateDirectory(_tempDir);
        Directory.CreateDirectory(GetDirPath(true));
        Directory.CreateDirectory(GetDirPath(false));
    }
    public void DeleteImageStorage()
    {
        try
        {
            Directory.Delete(_tempDir, true);
        }
        catch 
        {
            throw;
        }
    }

    public int GetMediastinumMaxFileNum()
    {
        return GetMaxFileNum(GetDirPath(isMediastinum: true));
    }
    public int GetLungFieldMaxFileNum()
    {
        return GetMaxFileNum(GetDirPath(isMediastinum: false));
    }
    private static int GetMaxFileNum(string path)
    {
        try
        {
            return Directory.GetFiles(path).Length;
        }
        catch (Exception e)
        {
            Debug.WriteLine($"GetFiles filed. PATH={path} {e.Message}");
            return 0;
        }
    }

    public void SaveImageMediastinum(int sliceNumber)
    {
        var path = GetFilePath(isMediastinum: true, sliceNumber);
        SaveImage(path, mediastinumFolderName, sliceNumber);
    }
    public void SaveImageLungField(int sliceNumber)
    {
        var path = GetFilePath(isMediastinum: false, sliceNumber);
        SaveImage(path, lungFieldFolderName, sliceNumber);
    }
    private void SaveImage(string path, string folderType, int sliceNumber)
    {
        var srcFileName = sliceNumber % 2 == 0 ? "slice0000" : "slice0001";
        var srcPath = $@"{Environment.CurrentDirectory}\Resource\{folderType}\{srcFileName}.{imageExtension}";

        try
        {
            File.Copy(srcPath, path, true);
        }
        catch
        {
            throw;
        }
    }

    public string GetMediastinumFilePath(int sliceNumber)
    {
        return GetFilePath(isMediastinum: true, sliceNumber);
    }
    public string GetLungFieldFilePath(int sliceNumber)
    {
        return GetFilePath(isMediastinum: false, sliceNumber);
    }
    private string GetFilePath(bool isMediastinum, int sliceNumber)
    {
        var dirPath = GetDirPath(isMediastinum);
        return @$"{dirPath}\slice{sliceNumber.ToString("D4")}.{imageExtension}";
    }
    private string GetDirPath(bool isMediastinum)
    {
        var field = isMediastinum ? mediastinumFolderName : lungFieldFolderName;
        return @$"{RootPath}\{StudyDate}_{Time}_{StudyUID}\{field}";
    }
    public void DeleteTempImage(int sliceNumber)
    {
        return;
    }

    private static ushort[] ReadRawData(string path)
    {
        try
        {
            var fs = new FileStream(path, FileMode.Open, FileAccess.Read);
            var reader = new BinaryReader(fs);

            long fileLength = fs.Length;
            int elementCount = (int)(fileLength / sizeof(ushort));

            ushort[] buffer = new ushort[elementCount];

            for (int i = 0; i < elementCount; i++)
            {
                buffer[i] = reader.ReadUInt16();
            }
            return buffer;
        }
        catch (Exception e)
        {
            Debug.WriteLine($"ReadRawUshortData. {e.Message}");
            return [];
        }
    }
    public BitmapSource LoadImage(string path, ThProcLibDicomImageInfo dicomInfo)
    {
        var width = dicomInfo.SizeX;
        var height = dicomInfo.SizeY;
        var windowCenter = dicomInfo.WindowCenter;
        var windowWidth = dicomInfo.WindowWidth;

        var slice16 = ReadRawData(path);

        var slice8 = new byte[width * height];
        var wMin = windowCenter - windowWidth / 2;
        var wMax = windowCenter + windowWidth / 2;
        for (int i = 0; i < slice16.Length; i++)
        {
            var y = 255.0 * (slice16[i] - wMin) / windowWidth;
            if (y < 0) slice8[i] = 0;
            else if (y >= 255) slice8[i] = 255;
            else slice8[i] = (byte)y;
        }
        // WriteableBitmapに書き出し（Gray8フォーマットで表示用）
        var wb = new WriteableBitmap(width, height, 96, 96, PixelFormats.Gray8, null);
        wb.WritePixels(new Int32Rect(0, 0, width, height), slice8, width, 0);

        return wb;

    }
}
