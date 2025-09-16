using System.Windows.Media;
using System.Windows.Media.Imaging;
using AppUI.Core;
using AppUI.Services;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using CommunityToolkit.Mvvm.Messaging;
using AppUI.ViewModels.Dialog;
using System.ComponentModel;
using System.Globalization;
using System.Windows.Data;
using System.Collections.ObjectModel;
using System.Windows.Input;

namespace AppUI.ViewModels;

public enum PenThickness
{ 
    Small,
    Mediam,
    Large,
    XLarge,
};

public partial class SliceViewer2DControlViewModel : ObservableObject, IDisposable
{
    private readonly IImageStorageService _storage;
    private readonly IDialogService _dialogService;
    private readonly MarkerRoiSettingsStateService _appState;

    public SliceViewer2DControlViewModel(IImageStorageService storage, IDialogService dialogService, MarkerRoiSettingsStateService markerRoiSettingStateService)
    {
        _storage = storage;
        _dialogService = dialogService;
        _appState = markerRoiSettingStateService;

        _appState.PropertyChanged += OnMarkerRoiSettingPageStateChanged;

#if DEMO_PROC
        DemoPointsVisible = true;
#endif
        WeakReferenceMessenger.Default.Register<SliceViewer2DControlViewModel, VolumeInitialize>(this, (s, e) =>
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

        WeakReferenceMessenger.Default.Register<ImageUpdate, string>(this, "ImageUpdate2D", (s, e) =>
        {
            if (_appState.VolumeInfoKey == 0) return;
            Log.Debug("SliceViewer2DControlViewModel ImageUpdate2D Regist.");
            ImageSource = ThProc.UpdateImageFor2D(_appState.VolumeInfoKey) ?? ImageSource;
        });
    }

    public async Task Initialize(string studyUid, bool isImportData = false)
    {
        StudyUID = studyUid;

        ZoomScale = 1.0;


        if (isImportData)
        {
            StatusMessage = processing3DMessage;
            IsConverting3D = true;
            IsStatusPopup = true;

            await LoadVolume();
            await InitializeAsync();
        }
        else
        {
            _appState.IsRoiSetting = false;
            IsConverting3D = false;
            IsStatusPopup = false;
            InitializeRendering(false);
        }
    }

    private async Task LoadVolume()
    {
        var cnt = 0;
        var cts = new CancellationTokenSource();

        try
        {
            var volumeKey = await ThProc.LoadRenderingVolumeFor2D(
                StudyUID,
                cts,
                onProgress: poolingResult =>
                {
                    if (++cnt > 3) cnt = 0;
                    WeakReferenceMessenger.Default.Send(new VolumeInitialize(cnt));
                },
                onSucceeded: poolingResult =>
                {
                    WeakReferenceMessenger.Default.Send(new VolumeInitialize(-1));
                    Log.Info("SliceViewer2DControlViewModel LoadRenderingVolumeFor2D succeeded cmdID:{0}", poolingResult.CmdID);
                });
            _appState.VolumeInfoKey = volumeKey;

            InitializeRendering(true);
        }
        catch(WarningException)
        {
            WeakReferenceMessenger.Default.Send<CloseMessage>();
            throw;
        }
    }
    private void InitializeRendering(bool isImport)
    {
        if (_appState.VolumeInfoKey == 0)
        {
            Log.Error("SliceViewer2DControlViewModel InitializeRendering VolumeInfo is uninitialized.");
            return;
        }
        // Volumeデータのロード
        try
        {
            if (!ThProc.GenerateRenderGraphicContextFor2D(_appState.VolumeInfoKey, IsMediastinum ? RenderingSeriesType.Juukaku : RenderingSeriesType.Haiya))
            {
                Log.Warn("SliceViewer2DControlViewModel GenerateRenderGraphicContextFor2D(volKey:{0}, IsMediastinum:{1})",
                    _appState.VolumeInfoKey, IsMediastinum);
                return;
            }
            if (!ThProc.SetLinkedDisplayMode(_appState.VolumeInfoKey, isImport ? RenderingDisplayMode.CTDataImport : RenderingDisplayMode.CTMarkerSettings))
            {
                Log.Error("SliceViewer2DControlViewModel SetLinkedDisplayMode(volKey:{0}, isImport:{1})",
                    _appState.VolumeInfoKey, isImport);
                return;
            }

            if (CanvasWidth > 0 && CanvasHeight > 0)
            {
                OnCanvasSizeChanged(CanvasWidth, CanvasHeight);
            }

            MaxSliceNumber = ThProc.GetVolumeZSlice(_appState.VolumeInfoKey);

            TargetSliceNumber = MaxSliceNumber / 2;
            SliceNavigator = $"{TargetSliceNumber}/{MaxSliceNumber}";
        }
        catch (Exception ex)
        {
            Log.Error("SliceViewer2DControlViewModel {0}", ex.Message);
            throw;
        }
    }
    public void ReleaseRendering()
    {
        if (_appState.VolumeInfoKey != 0)
        {
            // Volumeデータリリース
            if (!ThProc.ReleaseRenderingFor2D(_appState.VolumeInfoKey))
            {
                Log.Error("SliceViewer2DControlViewModel ReleaseRenderingFor2D(volKey:{0})", _appState.VolumeInfoKey);
                return;
            }
            if (ThProc.ReleaseVolume(_appState.VolumeInfoKey))
            {
                _appState.VolumeInfoKey = 0;
            }
            else
            {
                Log.Error("SliceViewer2DControlViewModel ReleaseVolume(volKey:{0})", _appState.VolumeInfoKey);
            }
        }
    }

    private void OnMarkerRoiSettingPageStateChanged(object? sender, PropertyChangedEventArgs? e)
    {
        if (e?.PropertyName == nameof(MarkerRoiSettingsStateService.IsRoiSettingExecutable))
        {
            if(!_appState.IsRoiSettingExecutable)
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
                ImageSource = ThProc.UpdateImageFor2D(_appState.VolumeInfoKey) ?? ImageSource;
            }
            else
            {
                Log.Warn("SliceViewer2DControlViewModel OnMarkerRoiSettingPageStateChanged({0}) VolumeInfoKey UnInitialize.", e?.PropertyName!);
            }
        }
    }

    [ObservableProperty]
    public bool isStatusPopup = false;

    [ObservableProperty]
    public string statusMessage = "";

    private readonly static string processing3DMessage = "3D化中";
    private readonly static string complete3DMessage = "3D化完了";

    [ObservableProperty]
    public bool isConverting3D;

    [ObservableProperty]
    public string sliceNavigator = "0/0";

    [ObservableProperty]
    private ImageSource? imageSource = new BitmapImage();

    [ObservableProperty]
    public bool isRoiSetting;

    [ObservableProperty]
    private bool isMediastinum = true;

    [ObservableProperty]
    private bool isLungField = false;

    [ObservableProperty]
    private double zoomScale = 1.0;

    private const double ZoomStep = 1.5;
    private const double MinZoom = 0.15;
    private const double MaxZoom = 50.0;

    [ObservableProperty]
    private string loadingMessage = "";

    [ObservableProperty]
    private PenThickness thickness = PenThickness.Mediam;

    [ObservableProperty]
    private bool isPenSelected = true;

    [ObservableProperty]
    private int maxSliceNumber = 0;

    [ObservableProperty]
    private int targetSliceNumber = 0;

    public string StudyUID { get; set; } = "";

    public ObservableCollection<PointViewModel> NewPoints { get; } = [];
    [ObservableProperty]
    private bool demoPointsVisible = false;

    private double CanvasWidth = 0.0;
    private double CanvasHeight = 0.0;

    [ObservableProperty]
    public Cursor currentCursor = Cursors.Arrow;

    private async Task InitializeAsync()
    {
        StatusMessage = processing3DMessage;

        await Processing3DImage();

        StatusMessage = complete3DMessage;
        IsConverting3D = false;
    }

    private async Task Processing3DImage()
    {
        var cts = new CancellationTokenSource();
        var pollingResult = new ThProcCmdStatus();
        try
        {
            var cnt = 0;
            var poolingTask = ThProc.ExecuteAISegmentatorCommand(StudyUID, 
                out ThProcCmdOutput exportResult,
                cts,
                onProgress: pollingResult =>
                {
                    if (++cnt > 3) cnt = 0;
                    StatusMessage = processing3DMessage;
                    for (var i = 0; i < cnt; i++)
                    {
                        StatusMessage += ("・");
                    }
                });

            pollingResult = await poolingTask;

        }
        catch (Exception ex)
        {
            Log.Error("SliceViewer2DControlViewModel Processing3DImage {0}", ex.Message);
            WeakReferenceMessenger.Default.Send<CloseMessage>();
            throw;
        }
    }

    public void OnCanvasSizeChanged(double width, double height)
    {
        CanvasWidth = width;
        CanvasHeight = height;

        if (_appState.VolumeInfoKey == 0) return;

        ImageSource = ThProc.InitializeImage2D(
            _appState.VolumeInfoKey, 
            width, height,
            RenderingSeriesType.Juukaku,
            ZoomScale,
            TargetSliceNumber) ?? ImageSource;
    }

    public void OnMouseLeftButtonDown(double posX, double posY)
    {
        if (_appState.VolumeInfoKey == 0) return;

        if (_appState.IsMarkerSetting && !_appState.IsRoiSettingExecutable)
        {
            Log.Info("SliceViewer2DControlViewModel OnMouseLeftButtonDown UpdateImageOnClickedMarkerFor2D(volKey:{0} X:{1} Y:{2})", 
                _appState.VolumeInfoKey, posX, posY);
            try
            {
                var tempImage = ThProc.UpdateImageOnClickedMarkerFor2D(_appState.VolumeInfoKey, posX, posY);
                ImageSource = tempImage ?? ImageSource;
            }
            catch
            {
                throw;
            }

            _appState.MarkerCount++;
            NewPoints.Add(new PointViewModel { X = posX, Y = posY });
            WeakReferenceMessenger.Default.Send(new MarkerSelectMessage(_appState.MarkerCount));
        }
        else if(_appState.IsRoiSetting)
        {
            ImageSource = ThProc.UpdateImageOnPaintLeftUpDownFor2D(_appState.VolumeInfoKey, posX, posY, true) ?? ImageSource;
            ImageSource = ThProc.UpdateImageOnPaintFor2D(_appState.VolumeInfoKey, posX, posY, IsPenSelected) ?? ImageSource;
            Log.Info("SliceViewer2DControlViewModel OnMouseLeftButtonDown UpdateImageOnPaintFor2D(volKey:{0} X:{1} Y:{2} IsPenSelected:{3})",
                _appState.VolumeInfoKey, posX, posY, IsPenSelected);
        }

    }
    public void OnMouseMove(double posX, double posY)
    {
        if (_appState.VolumeInfoKey == 0) return;

        if (_appState.IsRoiSetting)
        {
            ImageSource = ThProc.UpdateImageOnPaintFor2D(_appState.VolumeInfoKey, posX, posY, IsPenSelected) ?? ImageSource;
            Log.Debug("SliceViewer2DControlViewModel OnMouseMove UpdateImageOnPaintFor2D (volKey:{0} X:{1} Y:{2} IsPenSelected:{3})", _appState.VolumeInfoKey, posX, posY, IsPenSelected);
        }
    }
    public void OnMouseLeftButtonUp(double posX, double posY)
    {
        if (_appState.VolumeInfoKey == 0) return;
        ImageSource = ThProc.UpdateImageOnPaintLeftUpDownFor2D(_appState.VolumeInfoKey, posX, posY, false) ?? ImageSource;
        Log.Info("SliceViewer2DControlViewModel OnMouseLeftButtonUp (volKey:{0} X:{1} Y:{2} IsPenSelected:{3})", _appState.VolumeInfoKey, posX, posY, IsPenSelected);
    }
    public void OnOptionClicked(double startX, double startY)
    {
        CurrentCursor = Cursors.SizeAll;
        Log.Info("SliceViewer2DControlViewModel OnOptionClicked(startX:{0} startY:{1})", startX, startY);
    }
    public void OnOptionMouseMove(double deltaX, double deltaY)
    {
        if (_appState.VolumeInfoKey == 0) return;

        ImageSource = ThProc.UpdateImagePanChangedFor2D(_appState.VolumeInfoKey, deltaX, deltaY) ?? ImageSource;
        Log.Debug("SliceViewer2DControlViewModel OnOptionMouseMove UpdateImagePanChangedFor2D(volKey:{0} deltaX:{1} deltaY:{2})", _appState.VolumeInfoKey, deltaX, deltaY);
    }
    public void OnOptionUp(double currentPosX, double currentPosY, double deltaX, double deltaY)
    {
        CurrentCursor = Cursors.Arrow;
        Log.Info("SliceViewer2DControlViewModel OnOptionUp(X:{0} Y:{1} deltaX:{2} deltaY:{3})", currentPosX, currentPosY, deltaX, deltaY);
    }

    public void OnSliderValueChanged(int newValue)
    {
        if (_appState.VolumeInfoKey == 0) return;

        TargetSliceNumber = newValue;
        SliceNavigator = $"{TargetSliceNumber}/{MaxSliceNumber}";

        var tempImage = ThProc.UpdateImageSliceNumberFor2D(_appState.VolumeInfoKey, TargetSliceNumber);
        ImageSource = tempImage ?? ImageSource;
    }

    [RelayCommand]
    private void CloseButton()
    {
        Log.Info("SliceViewer2DControlViewModel CloseButton");
        WeakReferenceMessenger.Default.Send<CloseMessage>();
    }
    [RelayCommand]
    private void MediastinumButton()
    {
        Log.Info("SliceViewer2DControlViewModel MediastinumButton volKey:{0} type:{1} 3Dlink:true", _appState.VolumeInfoKey, RenderingSeriesType.Juukaku);

        IsMediastinum = true;
        IsLungField = false;

        if (_appState.VolumeInfoKey == 0) return;

        try
        {
            var tempImage = ThProc.UpdateImageOnSwitchCTModeFor2DandLinked3D(_appState.VolumeInfoKey, RenderingSeriesType.Juukaku, true);
            if (tempImage != null)
            {
                Log.Warn("SliceViewer3DControlViewModel UpdateImageOnSwitchCTModeFor2DandLinked3D update image failed.");
            }
            ImageSource = tempImage ?? ImageSource;
        }
        catch (WarningException)
        {
            throw;
        }
    }
    [RelayCommand]
    private void LungFieldButton()
    {
        Log.Info("SliceViewer2DControlViewModel LungFieldButton volKey:{0} type:{1} 3Dlink:true", _appState.VolumeInfoKey, RenderingSeriesType.Haiya);

        IsMediastinum = false;
        IsLungField = true;

        if (_appState.VolumeInfoKey == 0) return;

        try
        {
            var tempImage = ThProc.UpdateImageOnSwitchCTModeFor2DandLinked3D(_appState.VolumeInfoKey, RenderingSeriesType.Haiya, true);
            if (tempImage != null)
            {
                Log.Warn("SliceViewer3DControlViewModel UpdateImageOnSwitchCTModeFor2DandLinked3D update image failed.");
            }
            ImageSource = tempImage ?? ImageSource;
        }
        catch (WarningException)
        {
            throw;
        }
    }
    [RelayCommand]
    private void PreviousButton()
    {
        if (_appState.VolumeInfoKey == 0) return;

        if (--TargetSliceNumber < 1) TargetSliceNumber = 1;
        SliceNavigator = $"{TargetSliceNumber}/{MaxSliceNumber}";

        Log.Info("SliceViewer2DControlViewModel PreviousButton volKey:{0} TargetSliceNumber:{1}", _appState.VolumeInfoKey, TargetSliceNumber);

        var tempImage = ThProc.UpdateImageSliceNumberFor2D(_appState.VolumeInfoKey, TargetSliceNumber);
        ImageSource = tempImage ?? ImageSource;
    }
    [RelayCommand]
    private void NextButton()
    {
        if (_appState.VolumeInfoKey == 0) return;

        if (++TargetSliceNumber > MaxSliceNumber) TargetSliceNumber = MaxSliceNumber;
        SliceNavigator = $"{TargetSliceNumber}/{MaxSliceNumber}";

        Log.Info("SliceViewer2DControlViewModel NextButton volKey:{0} TargetSliceNumber:{1} MaxSliceNumber:{2}", _appState.VolumeInfoKey, TargetSliceNumber, MaxSliceNumber);

        var tempImage = ThProc.UpdateImageSliceNumberFor2D(_appState.VolumeInfoKey, TargetSliceNumber);
        ImageSource = tempImage ?? ImageSource;
    }
    [RelayCommand]
    private void ZoomInButton()
    {
        if (_appState.VolumeInfoKey == 0) return;

        ZoomScale *= ZoomStep;
        if (ZoomScale > MaxZoom) ZoomScale = MaxZoom;

        Log.Info("SliceViewer2DControlViewModel ZoomInButton(volKey:{0} ZoomScale:{1})", _appState.VolumeInfoKey, ZoomScale);

        var tempImage = ThProc.UpdateImageZoomFor2D(_appState.VolumeInfoKey, ZoomScale);
        ImageSource = tempImage ?? ImageSource;
    }
    [RelayCommand]
    private void ZoomOutButton()
    {
        if (_appState.VolumeInfoKey == 0) return;

        ZoomScale /= ZoomStep;
        if (ZoomScale < MinZoom) ZoomScale = MinZoom;

        Log.Info("SliceViewer2DControlViewModel ZoomOutButton(volKey:{0} ZoomScale:{1})", _appState.VolumeInfoKey, ZoomScale);

        var tempImage = ThProc.UpdateImageZoomFor2D(_appState.VolumeInfoKey, ZoomScale);
        ImageSource = tempImage ?? ImageSource;
    }
    [RelayCommand]
    private void RoundRadioButtonS()
    {
        if (_appState.VolumeInfoKey == 0) return;

        Log.Info("SliceViewer2DControlViewModel RoundRadioButtonS");
        ThProc.SetRoiPenThickness(PenType.Pen1, _appState.VolumeInfoKey);
    }
    [RelayCommand]
    private void RoundRadioButtonM()
    {
        if (_appState.VolumeInfoKey == 0) return;

        Log.Info("SliceViewer2DControlViewModel RoundRadioButtonM");
        ThProc.SetRoiPenThickness(PenType.Pen2, _appState.VolumeInfoKey);
    }
    [RelayCommand]
    private void RoundRadioButtonL()
    {
        if (_appState.VolumeInfoKey == 0) return;

        Log.Info("SliceViewer2DControlViewModel RoundRadioButtonL");
        ThProc.SetRoiPenThickness(PenType.Pen3, _appState.VolumeInfoKey);
    }
    [RelayCommand]
    private void RoundRadioButtonX()
    {
        if (_appState.VolumeInfoKey == 0) return;

        Log.Info("SliceViewer2DControlViewModel RoundRadioButtonX");
        ThProc.SetRoiPenThickness(PenType.Pen4, _appState.VolumeInfoKey);
    }
    [RelayCommand]
    private void EditButton()
    {
        if (_appState.VolumeInfoKey == 0) return;

        Log.Info("SliceViewer2DControlViewModel EditButton");
        ThProc.SwitchToRoiPenMode(_appState.VolumeInfoKey);
    }
    [RelayCommand]
    private void EraseButton()
    {
        if (_appState.VolumeInfoKey == 0) return;

        Log.Info("SliceViewer2DControlViewModel EraseButto");
        ThProc.SwitchToRoiEraseMode(_appState.VolumeInfoKey);
    }
    [RelayCommand]
    private void AllEraseButton()
    {
        var dlgMessage = MessageService.GetMessage(MessageCode.DlgMsgDeleteRoiClear);
        _dialogService.ShowDialog(new GenericDialogViewModel(DialogPreset.CloseYesNo)
        {
            MessageLines = [new(dlgMessage)],
            YesAction = new Action(() => {
                if (_appState.VolumeInfoKey == 0) return;

                try
                {
                    var tempImage = ThProc.UpdateImageRoiClearFor2D(_appState.VolumeInfoKey);
                    if (tempImage != null)
                    {
                        Log.Warn("SliceViewer3DControlViewModel UpdateImageRoiClearFor2D update image failed.");
                    }
                    ImageSource = tempImage ?? ImageSource;
                }
                catch (WarningException)
                {
                    throw;
                }
                Log.Info("ExecuteRoiClear(volKey:{0})", _appState.VolumeInfoKey);
            }),
            NoAction = new Action(() => { /*処理なし*/ }),
        });
    }
    public void Dispose()
    {
        _appState.PropertyChanged -= OnMarkerRoiSettingPageStateChanged;
        WeakReferenceMessenger.Default.UnregisterAll(this);
        GC.SuppressFinalize(this);
    }
}

public class InverseBooleanConverter : IValueConverter
{
    public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
    {
        if (value is bool b)
            return !b;
        return false;
    }

    public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
    {
        if (value is bool b)
            return !b;
        return false;
    }
}

public class EnumToBooleanConverter : IValueConverter
{
    public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
    {
        return value?.ToString() == parameter?.ToString();
    }

    public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
    {
        if ((bool)value)
            return Enum.Parse(targetType, parameter.ToString() ?? "");
        return Binding.DoNothing;
    }
}

public class HeightReductionConverter : IValueConverter
{
    public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
    {
        double originalHeight = (double)value;
        double reduceBy = parameter != null ? double.Parse(parameter.ToString() ?? "0") : 0;
        return originalHeight - reduceBy;
    }

    public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
    {
        throw new NotImplementedException();
    }
}