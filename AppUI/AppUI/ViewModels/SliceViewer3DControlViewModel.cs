using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using AppUI.Core;
using AppUI.Services;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using CommunityToolkit.Mvvm.Messaging;

namespace AppUI.ViewModels;

// 
public enum StudyCaseViewType
{
    Pneumothorax = 1,
    PleuralEffusion = 2,
    Empyema = 3,
    Hemothorax = 4,
    Other = 5,
};

public class PointViewModel : ObservableObject
{
    public double X { get; set; }
    public double Y { get; set; }
}

public partial class SliceViewer3DControlViewModel : ObservableObject, IDisposable
{
    private readonly IImageStorageService _storage;
    private readonly MarkerRoiSettingsStateService _appState;

    public SliceViewer3DControlViewModel(IImageStorageService storage, MarkerRoiSettingsStateService markerRoiSettingStateService)
    {
        _storage = storage;
        _appState = markerRoiSettingStateService;

        _appState.PropertyChanged += OnMarkerRoiSettingPageStateChanged;

#if DEMO_PROC
        DemoPointsVisible = true;
#endif

        WeakReferenceMessenger.Default.Register<SliceViewer3DControlViewModel, VolumeInitialize>(this, (s, e) =>
        {
            if (e.Value >= 0)
            {
                var comma = new string('.', e.Value);
                LoadingMessage = $"Loading {comma}";
            }
            else
            {
                LoadingMessage = "";
            }
        });

        WeakReferenceMessenger.Default.Register<ImageUpdate, string>(this, "ImageUpdate3D", (s, e) =>
        {
            if (_appState.VolumeInfoKey == 0) return;

            Log.Debug("SliceViewer3DControlViewModel ImageUpdate3D Regist.");
            var tempImage = ThProc.UpdateImageFor3D(_appState.VolumeInfoKey);
            ImageSource = tempImage ?? ImageSource;
        });
    }
    private void OnMarkerRoiSettingPageStateChanged(object? sender, PropertyChangedEventArgs? e)
    {
        if (e?.PropertyName == nameof(MarkerRoiSettingsStateService.IsRoiSettingExecutable))
        {
            if (!_appState.IsRoiSettingExecutable)
            {
                NewPoints.Clear();
            }
        }
        else if (e?.PropertyName == nameof(MarkerRoiSettingsStateService.IsRoiSetting))
        {
            IsRoiSetting = _appState.IsRoiSetting;
            if (IsRoiSetting)
            {
                NewPoints.Clear();
            }

            if (_appState.VolumeInfoKey != 0)
            {
                ImageSource = ThProc.UpdateImageFor3D(_appState.VolumeInfoKey) ?? ImageSource;
            }
            else
            {
                Log.Warn("SliceViewer3DControlViewModel OnMarkerRoiSettingPageStateChanged({0}) VolumeInfoKey UnInitialize.", e?.PropertyName!);
            }
        }
    }

    public void InitializeRendering(ThProcPatientInfo targetPatientInfo)
    {
        ZoomScale = 1.0;
        // Volumeデータのロード
        try
        {
            // Renderingコマンド用のGCを新規作成
            if (!ThProc.GenerateRenderGraphicContextFor3D(_appState.VolumeInfoKey))
            {
                Log.Error("GenerateRenderGraphicContextFor3D(volKey:{0}", _appState.VolumeInfoKey);
                return;
            }

            if (CanvasWidth > 0 && CanvasHeight > 0)
            {
                OnCanvasSizeChanged(CanvasWidth, CanvasHeight);
            }
        }
        catch (Exception ex)
        {
            Log.Error("SliceViewer3DControlViewModel InitializeRendering {0}", ex.Message);
            throw;
        }
    }

    public void ReleaseRendering()
    {
        if (_appState.VolumeInfoKey != 0)
        {
            // Volumeデータリリース
            if (!ThProc.ReleaseRenderingFor3D(_appState.VolumeInfoKey))
            {
                Log.Error("SliceViewer3DControlViewModel ReleaseRenderingFor3D(VolID:{0}) failed.", _appState.VolumeInfoKey);
                return;
            }
        }
    }

    [ObservableProperty]
    private ImageSource imageSource = new BitmapImage();

    [ObservableProperty]
    private bool isRoiSetting = false;

    [ObservableProperty]
    private double zoomScale = 1.0;

    private const double ZoomStep = 1.5;
    private const double MinZoom = 0.15;
    private const double MaxZoom = 50.0;

    [ObservableProperty]
    private string loadingMessage = "";

    public ObservableCollection<PointViewModel> NewPoints { get; } = [];
    [ObservableProperty]
    private bool demoPointsVisible = false;

    private double CanvasWidth = 0.0;
    private double CanvasHeight = 0.0;

    [ObservableProperty]
    public Cursor currentCursor = Cursors.Arrow;
    public void OnCanvasClicked(Point point)
    {
        Log.Info("SliceViewer3DControlViewModel OnCanvasClicked volKey:{0} X:{1} Y:{2} MarkerFlg:{3} RoiFlg:{4}",
            _appState.VolumeInfoKey, point.X, point.Y, _appState.IsMarkerSetting && !_appState.IsRoiSettingExecutable, _appState.IsRoiSetting);
        if (_appState.IsMarkerSetting && !_appState.IsRoiSettingExecutable)
        {
            if (_appState.VolumeInfoKey == 0) return;

            try
            {
                var tempImage = ThProc.UpdateImageOnClickedMarkerFor3D(_appState.VolumeInfoKey, point.X, point.Y);
                ImageSource = tempImage ?? ImageSource;
            }
            catch
            {
                throw;
            }

            _appState.MarkerCount++;

            NewPoints.Add(new PointViewModel { X = point.X, Y = point.Y });
            WeakReferenceMessenger.Default.Send(new MarkerSelectMessage(_appState.MarkerCount));
        }
        else if(_appState.IsRoiSetting)
        {
            if (_appState.VolumeInfoKey == 0) return;

            ThProc.NotifyLeftButtonDownFor3D(_appState.VolumeInfoKey, point.X, point.Y);
        }
    }

    public void OnMouseMove(Point point)
    {
        Log.Debug("SliceViewer3DControlViewModel OnMouseMove volKey:{0} X:{1} Y:{2}", _appState.VolumeInfoKey, point.X, point.Y);
        if (_appState.IsRoiSetting)
        {
            if (_appState.VolumeInfoKey == 0) return;

            ImageSource = ThProc.UpdateImageRotationFor3D(_appState.VolumeInfoKey, point.X, point.Y) ?? ImageSource;
        }
    }
    public void OnMouseLeftButtonUp(double posX, double posY)
    {
        if (_appState.VolumeInfoKey == 0) return;
        Log.Info("SliceViewer3DControlViewModel OnMouseLeftButtonUp (volKey:{0} X:{1} Y:{2})", _appState.VolumeInfoKey, posX, posY);
    }
    public void OnOptionClicked(double startX, double startY)
    {
        CurrentCursor = Cursors.SizeAll;
        Log.Info("SliceViewer3DControlViewModel OnOptionClicked(startX:{0} startY:{1})", startX, startY);
    }
    public void OnOptionMouseMove(double deltaX, double deltaY)
    {
        if (_appState.VolumeInfoKey == 0) return;

        ImageSource = ThProc.UpdateImagePanChangedFor3D(_appState.VolumeInfoKey, deltaX, deltaY) ?? ImageSource;
        Log.Debug("SliceViewer3DControlViewModel OnOptionMouseMove UpdateImagePanChangedFor3D(volKey:{0} deltaX:{1} deltaY:{2})", _appState.VolumeInfoKey, deltaX, deltaY);
    }
    public void OnOptionUp(double currentPosX, double currentPosY, double deltaX, double deltaY)
    {
        CurrentCursor = Cursors.Arrow;
        Log.Info("SliceViewer3DControlViewModel OnOptionUp(currentPosX:{0} currentPosY:{1} deltaX:{2} deltaY:{3})", currentPosX, currentPosY, deltaX, deltaY);
    }

    public void OnCanvasSizeChanged(double width, double height)
    {
        CanvasWidth = width;
        CanvasHeight = height;

        if (_appState.VolumeInfoKey == 0) return;

        ImageSource = ThProc.InitializeImage3D(
            _appState.VolumeInfoKey,
            width, height,
            ZoomScale) ?? ImageSource;
    }

    [ObservableProperty]
    bool isBodySurfaceChecked = false;

    [ObservableProperty]
    bool isLungChecked = true;

    [ObservableProperty]
    bool isHeartChecked = true;

    [ObservableProperty]
    bool isDispOtherChecked = false;

    [ObservableProperty]
    private StudyCaseViewType studyCase = StudyCaseViewType.Other;

    private DisplayPartsType GetDisplayPartsBit(bool isBodySurface, bool isLung, bool isHeart, bool isOther)
    {
        DisplayPartsType result = 0;

        if (isBodySurface) result |= DisplayPartsType.Body;
        if (isLung) result |= DisplayPartsType.Lung;
        if (isHeart) result |= DisplayPartsType.Heart;
        if (isOther) result |= DisplayPartsType.Other;

        return result;
    }
    private void DisplayPartsChanged()
    {
        var displayPartsBit = GetDisplayPartsBit(IsBodySurfaceChecked, IsLungChecked, IsHeartChecked, IsDispOtherChecked);
        Log.Info("SliceViewer3DControlViewModel DisplayPartsChanged volKey:{0} GetDisplayPartsBit:{1}", 
            _appState.VolumeInfoKey, displayPartsBit);
        if (_appState.VolumeInfoKey == 0) return;

        try
        {
            var tempImage = ThProc.UpdateImageOnDisplayPartsChangedFor3D(_appState.VolumeInfoKey, displayPartsBit, IsBodySurfaceChecked);
            if (tempImage != null)
            {
                Log.Warn("SliceViewer3DControlViewModel PneumothoraxButton update image failed.");
            }
            ImageSource = tempImage ?? ImageSource;
        }
        catch (WarningException)
        {
            throw;
        }
    }

    [RelayCommand]
    private void BodySurfaceButton()
    {
        DisplayPartsChanged();
    }
    [RelayCommand]
    private void LungButton()
    {
        DisplayPartsChanged();
    }
    [RelayCommand]
    private void HeartButton()
    {
        DisplayPartsChanged();
    }
    [RelayCommand]
    private void DisplaysOtherButton()
    {
        DisplayPartsChanged();
    }

    [RelayCommand]
    private void PneumothoraxButton()
    {
        Log.Info("SliceViewer3DControlViewModel PneumothoraxButton volKey:{0}", _appState.VolumeInfoKey);
        if (_appState.VolumeInfoKey == 0) return;

        try
        {
            var tempImageSource = ThProc.UpdateImageOnStudyCaseChangedFor3D(_appState.VolumeInfoKey, StudyCaseType.Pneumothorax);
            if (tempImageSource != null)
            {
                Log.Warn("SliceViewer3DControlViewModel PneumothoraxButton update image failed.");
            }
            ImageSource = tempImageSource ?? ImageSource;
        }
        catch (WarningException)
        {
            throw;
        }
    }
    [RelayCommand]
    private void PleuralEffusionButton()
    {
        Log.Info("SliceViewer3DControlViewModel PleuralEffusionButton volKey:{0}", _appState.VolumeInfoKey);
        if (_appState.VolumeInfoKey == 0) return;

        try
        {
            var tempImageSource = ThProc.UpdateImageOnStudyCaseChangedFor3D(_appState.VolumeInfoKey, StudyCaseType.PleuralEffusion);
            if (tempImageSource != null)
            {
                Log.Warn("SliceViewer3DControlViewModel PleuralEffusionButton update image failed.");
            }
            ImageSource = tempImageSource ?? ImageSource;
        }
        catch (WarningException)
        {
            throw;
        }
    }
    [RelayCommand]
    private void EmpyemaButton()
    {
        Log.Info("SliceViewer3DControlViewModel EmpyemaButton volKey:{0}", _appState.VolumeInfoKey);
        if (_appState.VolumeInfoKey == 0) return;

        try
        {
            var tempImageSource = ThProc.UpdateImageOnStudyCaseChangedFor3D(_appState.VolumeInfoKey, StudyCaseType.Empyema);
            if (tempImageSource != null)
            {
                Log.Warn("SliceViewer3DControlViewModel EmpyemaButton update image failed.");
            }
            ImageSource = tempImageSource ?? ImageSource;
        }
        catch (WarningException)
        {
            throw;
        }
    }
    [RelayCommand]
    private void HemothoraxButton()
    {
        Log.Info("SliceViewer3DControlViewModel HemothoraxButton volKey:{0}", _appState.VolumeInfoKey);
        if (_appState.VolumeInfoKey == 0) return;

        try
        {
            var tempImageSource = ThProc.UpdateImageOnStudyCaseChangedFor3D(_appState.VolumeInfoKey, StudyCaseType.Hemothorax);
            if (tempImageSource != null)
            {
                Log.Warn("SliceViewer3DControlViewModel HemothoraxButton update image failed.");
            }
            ImageSource = tempImageSource ?? ImageSource;
        }
        catch (WarningException)
        {
            throw;
        }
    }
    [RelayCommand]
    private void CasesOtherButton()
    {
        Log.Info("SliceViewer3DControlViewModel CasesOtherButton volKey:{0}", _appState.VolumeInfoKey);
        if (_appState.VolumeInfoKey == 0) return;

        try
        {
            var tempImageSource = ThProc.UpdateImageOnStudyCaseChangedFor3D(_appState.VolumeInfoKey, StudyCaseType.Other);
            if (tempImageSource != null)
            {
                Log.Warn("SliceViewer3DControlViewModel HemothoraxButton update image failed.");
            }
            ImageSource = tempImageSource ?? ImageSource;
        }
        catch (WarningException)
        {
            throw;
        }
    }

    [RelayCommand]
    private void ZoomInButton()
    {
        if (_appState.VolumeInfoKey == 0) return;

        ZoomScale *= ZoomStep;
        if (ZoomScale > MaxZoom) ZoomScale = MaxZoom;

        Log.Info("SliceViewer3DControlViewModel ZoomInButton volKey:{0} ZoomScale:{1}", _appState.VolumeInfoKey, ZoomScale);
        var tempImage = ThProc.UpdateImageZoomFor3D(_appState.VolumeInfoKey, ZoomScale);
        ImageSource = tempImage ?? ImageSource;
    }
    [RelayCommand]
    private void ZoomOutButton()
    {
        if (_appState.VolumeInfoKey == 0) return;

        ZoomScale /= ZoomStep;
        if (ZoomScale < MinZoom) ZoomScale = MinZoom;

        Log.Info("SliceViewer3DControlViewModel ZoomOutButton volKey:{0} ZoomScale:{1}", _appState.VolumeInfoKey, ZoomScale);
        var tempImage = ThProc.UpdateImageZoomFor3D(_appState.VolumeInfoKey, ZoomScale);
        ImageSource = tempImage ?? ImageSource;
    }

    public void Dispose()
    {
        _appState.PropertyChanged -= OnMarkerRoiSettingPageStateChanged;
        WeakReferenceMessenger.Default.UnregisterAll(this);
        GC.SuppressFinalize(this);
    }
}
