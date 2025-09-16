using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using CommunityToolkit.Mvvm.Messaging;
using AppUI.Services;

namespace AppUI.ViewModels.Dialog;

public partial class InstructionsDialogViewModel : ObservableObject
{
    [ObservableProperty]
    public string text = "";
    public InstructionsDialogViewModel() 
    {
    }
    [RelayCommand]
    private void CloseButton()
    {
        Log.Debug("InstructionsDialogViewModel OnCloseButton");
        WeakReferenceMessenger.Default.Send<CloseMessage, string>(new CloseMessage(), "InstructionsDialog");
    }

}
