using AppUI.ViewModels.Dialog;
using System.Collections.ObjectModel;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using System.Diagnostics;
using CommunityToolkit.Mvvm.Messaging;
using AppUI.Services;
using System.ComponentModel;
using AppUI.Core;

namespace AppUI.ViewModels;

public enum MarkerRoiConfirmType
{
    MarkerCorrect,
    MarkerSetting,
    RoiSetting,
};

public partial class MarkerPageFooterViewModel : ObservableObject, IDisposable
{
    private readonly AppStateService _appState;
    private readonly MarkerRoiSettingsStateService _roiMarkerState;
    private readonly IDialogService _dialogService;

    public ObservableCollection<DialogButton> Buttons { get; } = [];

    [ObservableProperty]
    private string confirmMessage = "";

    [ObservableProperty]
    private MarkerRoiConfirmType panelTemplateSelect = MarkerRoiConfirmType.MarkerCorrect;

    [ObservableProperty]
    private string markerProgressMessage = "マーカ：0/3";
    private int MarkerProgress { get; set; } = 0;

    [ObservableProperty]
    private bool isMarkerSetting = false;

    [ObservableProperty]
    private bool isRoiSettingExecutable = false;
    public MarkerPageFooterViewModel(IDialogService dialogService, AppStateService appStateService, MarkerRoiSettingsStateService roiMarkerState)
    {
        _appState = appStateService;
        _roiMarkerState = roiMarkerState;
        _dialogService = dialogService;

        _roiMarkerState.PropertyChanged += OnMarkerRoiSettingPageStateChanged;

        var dlgMessage = MessageService.GetMessage(MessageCode.MarkerRoiMsgMarkerCorrect);
        confirmMessage = dlgMessage;
        WeakReferenceMessenger.Default.Register<MarkerPageFooterViewModel, MarkerSelectMessage> (this, (s, e) =>
        {
            MarkerProgress = e.Value;
            MarkerProgressMessage = $"マーカ：{MarkerProgress}/3";
            if(MarkerProgress >= 3)
            {
                _roiMarkerState.IsRoiSettingExecutable = true;
            }
            Log.Debug("Update MarkerNum Count:{0}", e.Value);
        });
    }

    private void OnMarkerRoiSettingPageStateChanged(object? sender, PropertyChangedEventArgs? e)
    {
        if (e?.PropertyName == nameof(MarkerRoiSettingsStateService.IsMarkerSetting))
        {
            IsMarkerSetting = _roiMarkerState.IsMarkerSetting;
        }
        else if(e?.PropertyName == nameof(MarkerRoiSettingsStateService.IsRoiSettingExecutable))
        {
            IsRoiSettingExecutable = _roiMarkerState.IsRoiSettingExecutable;
        }
    }

    [RelayCommand]
    private void MarkerCorrect()
    {
        GoToRoiSetting();
    }
    [RelayCommand]
    private void MarkerUnCorrect()
    {
        if (_roiMarkerState.VolumeInfoKey == 0) return;

        var footerMessage = MessageService.GetMessage(MessageCode.MarkerRoiMsgConrimMarker);
#if DEMO_PROC
        try
        {
            ThProc.StartOverMarker(_roiMarkerState.VolumeInfoKey);
        }
        catch (WarningException ex)
        {
            Log.Warn("[{0}] {1}", ex.ErrCode, ex.Message);
            throw;
        }
        finally
        {
            ConfirmMessage = footerMessage;
            _roiMarkerState.IsMarkerSetting = true;
            _roiMarkerState.IsRoiSettingExecutable = false;
            _roiMarkerState.MarkerCount = 0;
            PanelTemplateSelect = MarkerRoiConfirmType.MarkerSetting;
            Log.Debug("Called MarkerUnCorrect");
        }
#else

        try
        {
            if (ThProc.StartOverMarker(_roiMarkerState.VolumeInfoKey))
            {
                ConfirmMessage = footerMessage;
                _roiMarkerState.IsMarkerSetting = true;
                _roiMarkerState.IsRoiSettingExecutable = false;
                _roiMarkerState.MarkerCount = 0;
                PanelTemplateSelect = MarkerRoiConfirmType.MarkerSetting;
            }
            else
            {
                Log.Warn("StartOverMarker failed");
            }
        }
        catch(WarningException ex)
        {
            Log.Warn("[{0}] {1}", ex.ErrCode, ex.Message);
            throw;
        }
#endif
    }

    [RelayCommand]
    private void MarkerStartOver()
    {
        MarkerProgress = 0;
        MarkerProgressMessage = $"マーカ：{MarkerProgress}/3";
        MarkerUnCorrect();
        Log.Debug("Called MarkerStartOver");
    }

    [RelayCommand]
    private void GoToRoiSetting()
    {
        try
        {
#if DEMO_PROC
#else
            if (!ThProc.FixedMarker(_roiMarkerState.VolumeInfoKey))
            {
                var exception = new WarningException("FixedMarker failed.");
                Log.Warn(exception.Message);
                throw exception;
            }
#endif
            if (!ThProc.SetLinkedDisplayMode(_roiMarkerState.VolumeInfoKey, RenderingDisplayMode.RoiSettings))
            {
                var exception = new WarningException("SetLinkedDisplayMode failed.");
                Log.Warn(exception.Message);
                throw exception;
            }
            _appState.StateType = MainStateType.SetRoi;
            _roiMarkerState.IsMarkerSetting = false;
            _roiMarkerState.IsRoiSetting = true;
            _roiMarkerState.IsRoiSettingExecutable = true;

            var footerMessage = MessageService.GetMessage(MessageCode.MarkerRoiMsgConfirmRoi);
            ConfirmMessage = footerMessage;
            PanelTemplateSelect = MarkerRoiConfirmType.RoiSetting;
            Log.Debug("Called GoToRoiSetting");

        }
        catch(WarningException)
        {
            Log.Error("CTMarkerFinalCommand failed ");
            throw;
        }
    }

    [RelayCommand]
    private void RoiSettingCorrect()
    {
        var isClose = false;
        var dlgMessage = MessageService.GetMessage(MessageCode.MarkerRoiMsgSave);
        _dialogService.ShowDialog(new GenericDialogViewModel(DialogPreset.CloseYesNo)
        {
            MessageLines = [new(dlgMessage)],
            YesAction = new Action(() => YesAction()),
            NoAction = new Action(() => { isClose = true; /* 処理なしでいいか確認 */ }),
        });
        if (isClose)
        {
            WeakReferenceMessenger.Default.Send<TransitionMessage, string>(new TransitionMessage(), "GoToHomeScreen");
        }
        Log.Info("Called RoiSettingCorrect");

        void YesAction()
        {
            isClose = true;
            if (_roiMarkerState.VolumeInfoKey == 0) return;

            try
            {
                ThProc.ExecuteRoiFix(_roiMarkerState.VolumeInfoKey);
            }
            catch(WarningException ex)
            {
                Log.Warn("[{0}] {1}", ex.ErrCode, ex.Message);
                throw;
            }
        }
    }

    public void Dispose()
    {
        WeakReferenceMessenger.Default.UnregisterAll(this);
        _roiMarkerState.PropertyChanged -= OnMarkerRoiSettingPageStateChanged;
        GC.SuppressFinalize(this);
    }
}
