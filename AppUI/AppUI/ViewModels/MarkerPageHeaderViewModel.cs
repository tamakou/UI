using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using System.Diagnostics;
using System.Windows;
using CommunityToolkit.Mvvm.Messaging;
using AppUI.Services;

namespace AppUI.ViewModels;
public partial class MarkerPageHeaderViewModel : ObservableObject
{
    public bool IsProgressVisible { get; }
    public bool IsPatientInfoVisible { get; }
    public bool IsHomeButtonVisible { get; }
    public StateProgressViewModel StateProgressViewModel { get; } = null!;

    public MarkerPageHeaderViewModel(AppStateService appStateService, bool isHomePage = true)
    {
        StateProgressViewModel = new StateProgressViewModel(appStateService);

        if (isHomePage)
        {
            IsProgressVisible = false;
            IsPatientInfoVisible = false;
            IsHomeButtonVisible = false;
        }
        else
        {
            IsProgressVisible = true;
            IsPatientInfoVisible = true;
            IsHomeButtonVisible = true;
        }
    }

    [ObservableProperty]
    private string patientId = "";

    [ObservableProperty]
    private string patientName = "";

    [ObservableProperty]
    private string studyDate = "";

    [RelayCommand]
    private void HomeButton()
    {
        Log.Info("MarkerPageHeaderViewModel HomeButton");
        WeakReferenceMessenger.Default.Send<TransitionMessage, string>(new TransitionMessage(), "GoToHomeScreen");
    }

    [RelayCommand]
    private void MinimizeButton()
    {
        Log.Info("MarkerPageHeaderViewModel MinimizeButton");
        WeakReferenceMessenger.Default.Send<TransitionMessage, string>(new TransitionMessage(), "WindowMinimize");
    }
    [RelayCommand]
    private void EndButton()
    {
        Log.Info("MarkerPageHeaderViewModel EndButton");
        Application.Current.Shutdown();
    }

}

