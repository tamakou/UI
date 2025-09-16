using System.ComponentModel;
using AppUI.Services;
using CommunityToolkit.Mvvm.ComponentModel;

namespace AppUI.ViewModels;

public partial class StateProgressViewModel : ObservableObject
{
    private readonly AppStateService _appStateService;

    [ObservableProperty]
    public ActiveContentsViewModel activeNewDataStatus = new();

    [ObservableProperty]
    public ActiveContentsViewModel activeSetMarkerStateStatus = new();

    [ObservableProperty]
    public ActiveContentsViewModel activeSetRoiStateStatus = new();

    [ObservableProperty]
    public ActiveContentsViewModel activeTransportDataStatus = new();

    public StateProgressViewModel(AppStateService appStateService)
    {
        _appStateService = appStateService;
        _appStateService.PropertyChanged += OnAppStateChanged;
    }
    private void OnAppStateChanged(object? sender, PropertyChangedEventArgs? e)
    {
        if (e?.PropertyName == nameof(AppStateService.StateType))
        {
            ActiveNewDataStatus.IsActive = _appStateService.StateType == MainStateType.AddNewData;
            ActiveSetMarkerStateStatus.IsActive = _appStateService.StateType == MainStateType.SetMarker;
            ActiveSetRoiStateStatus.IsActive = _appStateService.StateType == MainStateType.SetRoi;
            ActiveTransportDataStatus.IsActive = _appStateService.StateType switch
            {
                MainStateType.Xreal => true,
                MainStateType.XrealiPhone => true,
                MainStateType.UsbTransport => true,
                _ => false,
            };
        }
    }
}
