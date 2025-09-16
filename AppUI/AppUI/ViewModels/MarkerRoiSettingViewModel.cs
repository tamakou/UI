using System.Windows.Media.Imaging;
using System.Windows.Media;
using System.Diagnostics;
using AppUI.Services;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Microsoft.Extensions.DependencyInjection;
using CommunityToolkit.Mvvm.Messaging;
using AppUI.Core;

namespace AppUI.ViewModels;

public partial class MarkerRoiSettingViewModel : ObservableObject, IDisposable
{
    private readonly IServiceProvider _serviceProvider;
    private readonly MarkerRoiSettingsStateService _appState;

    public ObservableObject HedaerControl { get; private set; }
    public ObservableObject MainLeft { get; private set; }
    public ObservableObject MainRight { get; private set; }
    public ObservableObject FooterControl { get; private set; }

    private readonly MainWindowViewModel main;

    [ObservableProperty]
    private ImageSource imageSource = new BitmapImage();

    public MarkerRoiSettingViewModel(MainWindowViewModel main, IImageStorageService storage, IDialogService dialogService, IServiceProvider serviceProvider, 
        AppStateService appStateService, MarkerRoiSettingsStateService markerRoiSettingsStateService)
    {
        _serviceProvider = serviceProvider;
        _appState = markerRoiSettingsStateService;


        WeakReferenceMessenger.Default.Register<TransitionMessage, string>(this, "GoToHomeScreen", (s, e) =>
        {
            GoToHomeScreen();
        });

        this.main = main;
        HedaerControl = new MarkerPageHeaderViewModel(appStateService, false);
        MainLeft = _serviceProvider.GetRequiredService<SliceViewer2DControlViewModel>();
        MainRight = _serviceProvider.GetRequiredService<SliceViewer3DControlViewModel>();

        FooterControl = _serviceProvider.GetRequiredService<MarkerPageFooterViewModel>();
    }
    public async void Initialize()
    {
        if (_appState.TargetPatientInfo is null) return;
        var patientInfo = _appState.TargetPatientInfo;
        var studyUid = patientInfo.StudyUID;

        var markerPageHeaderVm = HedaerControl as MarkerPageHeaderViewModel;
        if (markerPageHeaderVm == null) return;

        markerPageHeaderVm.PatientId = patientInfo.PatientID;
        markerPageHeaderVm.PatientName = patientInfo.PatientName;
        var studyTime = (patientInfo.StudyTime.Length >= 5) ? patientInfo.StudyTime[..5] : "00:00";
        markerPageHeaderVm.StudyDate = $"{_appState.TargetPatientInfo.StudyDate} {studyTime}";

        // Volumeデータのロード 
        try
        {
            var cnt = 0;
            var cts = new CancellationTokenSource();
            var volumeKey = await ThProc.LoadRenderingVolumeFor3D(
                studyUid, 
                cts,
                onProgress: poolingResult =>
                {
                    if (++cnt > 3) cnt = 0;
                    WeakReferenceMessenger.Default.Send(new VolumeInitialize(cnt));
                },
                onSucceeded: poolingResult => { WeakReferenceMessenger.Default.Send(new VolumeInitialize(-1)); });

            _appState.VolumeInfoKey = volumeKey;

            var mainLeft = (MainLeft as SliceViewer2DControlViewModel)!;
            await mainLeft.Initialize(studyUid, false);

            var mainRight = MainRight as SliceViewer3DControlViewModel;
            mainRight?.InitializeRendering(patientInfo);
        }
        catch (Exception ex)
        {
            Log.Error("MarkerRoiSettingViewModel {0}", ex.Message);
            GoToHomeScreen();
            throw;
        }
    }

    void ReleaseVolume()
    {
        if (_appState.VolumeInfoKey != 0)
        {
            // Volumeデータリリース
            if (!ThProc.ReleaseVolume(_appState.VolumeInfoKey))
            {
                Log.Error("ReleaseVolume(VolID:{0}) failed.", _appState.VolumeInfoKey);
                return;
            }
            _appState.VolumeInfoKey = 0;
        }
    }

    [RelayCommand]
    private void GoToHomeScreen()
    {
        WeakReferenceMessenger.Default.Unregister<TransitionMessage, string>(this, "GoToHomeScreen");
        (MainRight as SliceViewer3DControlViewModel)?.ReleaseRendering();
        (MainLeft as SliceViewer2DControlViewModel)?.ReleaseRendering();
        ReleaseVolume();
        main.ShowHomeScreenCommand.Execute(null);
    }
    public void Dispose()
    {
        (MainRight as SliceViewer3DControlViewModel)?.Dispose();
        (MainLeft as SliceViewer2DControlViewModel)?.Dispose();
        WeakReferenceMessenger.Default.UnregisterAll(this);
        GC.SuppressFinalize(this);
    }

}
