using CommunityToolkit.Mvvm.ComponentModel;


namespace AppUI.ViewModels;

public partial class SliceViewerWindowViewModel: ObservableObject
{

    [ObservableProperty]
    private ObservableObject currentViewModel = null!;

    public SliceViewerWindowViewModel() { }
}