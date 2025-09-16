using System.Diagnostics;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Messaging;
using CommunityToolkit.Mvvm.Input;
using AppUI.Services.Models;
using AppUI.Services;
using System.ComponentModel;
using AppUI.Core;
using System.Reflection;
using System.Collections.ObjectModel;
using System.Windows;

namespace AppUI.ViewModels.Dialog;

public partial class SettingMenuDialogViewModel : ObservableObject
{
    private readonly ISettingTextJson _jsonService;
    private readonly IDialogService _dialogService;
    private readonly AppStateService _appStateService;

    [ObservableProperty]
    public double top;

    [ObservableProperty]
    public double left;

    [ObservableProperty]
    public ActiveContentsViewModel activeManualStatus = new();

    [ObservableProperty]
    public ActiveContentsViewModel activeVersionStatus = new();

    [ObservableProperty]
    public ActiveContentsViewModel activeInquiryStatus = new();

    public SettingMenuDialogViewModel(ISettingTextJson jsonService, IDialogService dialogService, AppStateService appStateService)
    {
        _jsonService = jsonService;
        _dialogService = dialogService;
        _appStateService = appStateService;

        _appStateService.PropertyChanged += OnAppStateChanged;

    }
    private void OnAppStateChanged(object? sender, PropertyChangedEventArgs? e)
    {
        if (e?.PropertyName == nameof(AppStateService.StateType))
        {
            ActiveManualStatus.IsActive = _appStateService.StateType == MainStateType.Manual;
            ActiveVersionStatus.IsActive = _appStateService.StateType == MainStateType.Version;
            ActiveInquiryStatus.IsActive = _appStateService.StateType == MainStateType.Inquiry;
        }
    }

    [RelayCommand]
    private void CloseButton()
    {
        WeakReferenceMessenger.Default.Send<CloseMessage>();
        //_appStateService.PropertyChanged -= OnAppStateChanged;
    }

    [RelayCommand]
    private void ManualButton()
    {
        var preStatus = _appStateService.StateType;
        _appStateService.StateType = MainStateType.Manual;

        Log.Debug("OnManualButton");
        ShowTextDialog(SettingTextType.Manual);

        _appStateService.StateType = preStatus;
    }
    [RelayCommand]
    private void VersionInformationButton()
    {
        var preStatus = _appStateService.StateType;
        _appStateService.StateType = MainStateType.Version;

        //var version = Assembly.GetExecutingAssembly().GetCustomAttribute<AssemblyInformationalVersionAttribute>()?.InformationalVersion ?? "N/A";
        var fileVer = FileVersionInfo.GetVersionInfo(Assembly.GetExecutingAssembly().Location).FileVersion ?? "N/A";
        var assemblyVer = Assembly.GetExecutingAssembly().GetName().Version?.ToString() ?? "N/A";

        Log.Info("VersionInformationButton");

        var thickness = new Thickness(100, 0, 0, 0);
        //var versonStr = @$"バージョン       <{fileVer}>";
        //var serialStr = @$"製造番号         <{assemblyVer}>";

        try
        {
            var messages = new ObservableCollection<MessageLine>();

            ThProc.GetEnvironment(out var output);
            var lines = output.ModelVers.Split('\n');

            foreach (string line in lines)
            {
                var temp = line.TrimEnd('\r');
                var columns = temp.Split(':');
                if (columns.Length >= 2) messages.Add(new(columns[0], columns[1], thickness));
                else messages.Add(new(columns[0]));
            }

            var dialogVm = new GenericDialogViewModel(DialogPreset.CloseOnly, messages)
            {
                MessageLines = messages,
            };
            _dialogService.ShowDialog(dialogVm);
        }
        catch (ExpectedException ex)
        {
            throw new WarningException(ex.ErrCode, ex.Message);
        }
        finally
        {
            _appStateService.StateType = preStatus;
        }
    }
    [RelayCommand]
    private void InquiryButton()
    {
        var preStatus = _appStateService.StateType;
        _appStateService.StateType = MainStateType.Inquiry;

        Log.Debug("OnInquiryButton");
        ShowTextDialog(SettingTextType.Inquiry);

        _appStateService.StateType = preStatus;
    }

    private void ShowTextDialog(SettingTextType type)
    {
        var text = _jsonService.ImportSettingText(type);
        var InstructionsDialogVM = new InstructionsDialogViewModel
        {
            Text = text
        };
        _dialogService.ShowDialog(InstructionsDialogVM);
    }
}