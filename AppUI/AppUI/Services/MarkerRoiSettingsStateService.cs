using CommunityToolkit.Mvvm.ComponentModel;
using AppUI.Core;

namespace AppUI.Services;

public partial class MarkerRoiSettingsStateService : ObservableObject
{
    [ObservableProperty]
    private bool isMarkerSetting;

    [ObservableProperty]
    private bool isRoiSettingExecutable;

    [ObservableProperty]
    private bool isRoiSetting;

    [ObservableProperty]
    private int markerCount;

    public ThProcPatientInfo? TargetPatientInfo { get; set; }
    public int VolumeInfoKey { get; set; } = 0;
}