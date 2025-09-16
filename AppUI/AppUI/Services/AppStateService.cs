using CommunityToolkit.Mvvm.ComponentModel;

namespace AppUI.Services;

public enum MainStateType
{
    Normal,
    AddNewData,
    SetMarker,
    SetRoi,
    TransportData,
    Setting,
    Manual,
    Version,
    Inquiry,
    Xreal,
    XrealiPhone,
    UsbTransport,
};

public partial class AppStateService : ObservableObject
{
    [ObservableProperty]
    private MainStateType stateType = MainStateType.Normal;
}

public partial class ActiveContentsViewModel : ObservableObject
{
    [ObservableProperty]
    private bool isActive = false;
}