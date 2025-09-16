using System.Diagnostics;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using CommunityToolkit.Mvvm.Messaging;
using AppUI.Services;
using AppUI.Core;
using System.Windows;
using Microsoft.Extensions.DependencyInjection;
using AppUI.ViewModels.Dialog;
using System.Globalization;

namespace AppUI.ViewModels;

public partial class MainWindowViewModel : ObservableObject, IDisposable
{
    private readonly IImageStorageService _storgaeService;
    private readonly IDialogService _dialogService;
    private readonly IServiceProvider _serviceProvider;
    private readonly AppStateService _appStateService;
    private readonly MarkerRoiSettingsStateService _markerRoiSettingStateService;

    public MainWindowViewModel(
        IImageStorageService storageService, 
        IDialogService dialogService, 
        IServiceProvider serviceProvider,
        AppStateService appStateService,
        MarkerRoiSettingsStateService markerRoiSettingStateService)
    {
        _storgaeService = storageService;
        _dialogService = dialogService;
        _serviceProvider = serviceProvider;
        _appStateService = appStateService;
        _markerRoiSettingStateService = markerRoiSettingStateService;


        try
        {
            ThProc.Initialize(0);
            ThProc.GetEnvironment(out var getenv);
            Log.Initialize(getenv.HomeFolder);
        }
        catch
        {
            Log.Initialize("");
            Log.Warn("ThProc.GetEnvironment failed.");
            throw;
        }
        Log.Info("Logger Initialized");

        CurrentViewModel = _serviceProvider.GetRequiredService<HomeScreenViewModels>();

        WeakReferenceMessenger.Default.Register<CloseMessage>(this, (s, e) =>
        {
            Log.Debug("MainWindow CloseMessage register.");
        });

        var cultureCode = "ja-JP"; // DLLのGetEnvironmentで取得できるのが望ましい
        var culture = new CultureInfo(cultureCode);
        Thread.CurrentThread.CurrentCulture = culture;
        Thread.CurrentThread.CurrentUICulture = culture;
    }

    [ObservableProperty]
    private ObservableObject currentViewModel;

    [RelayCommand]
    private void ShowMarkerRoiSettingsScreen()
    {
        var homeScreenVm = CurrentViewModel as HomeScreenViewModels;

        var patientInfo = homeScreenVm?.SelectedItem;

        homeScreenVm?.Dispose();

        _markerRoiSettingStateService.TargetPatientInfo = patientInfo;
        var RoiSettingViewModel = new MarkerRoiSettingViewModel(this, 
            _storgaeService, _dialogService, _serviceProvider, 
            _appStateService, _markerRoiSettingStateService);

        _appStateService.StateType = MainStateType.SetMarker;

        if (!ThProc.ExecuteCheckImageServerCommand())
        {
            var errMessage = MessageService.GetMessage(MessageCode.DLLImageServerError);
            Log.Fatal(errMessage);
            throw new FatalException(errMessage);
        }

        if (RoiSettingViewModel == null) return;

        try
        {
            RoiSettingViewModel.Initialize();
        }
        catch
        {
            ShowHomeScreen();
            throw;
        }

        CurrentViewModel = RoiSettingViewModel;
    }

    [RelayCommand]
    private void ShowHomeScreen()
    {
        _appStateService.StateType = MainStateType.Normal;
        var markerRoiVM = CurrentViewModel as MarkerRoiSettingViewModel;
        markerRoiVM?.Dispose();
        CurrentViewModel = _serviceProvider.GetRequiredService<HomeScreenViewModels>();
        (CurrentViewModel as HomeScreenViewModels)?.Initialize();
    }
    public void Dispose()
    {
        WeakReferenceMessenger.Default.UnregisterAll(this);
        GC.SuppressFinalize(this);
    }
}
