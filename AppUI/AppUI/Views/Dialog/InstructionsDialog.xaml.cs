using System.Windows;
using AppUI.ViewModels;
using CommunityToolkit.Mvvm.Messaging;

namespace AppUI.Views.Dialog;

/// <summary>
/// InstructionsDialog.xaml の相互作用ロジック
/// </summary>
public partial class InstructionsDialog : Window
{
    public InstructionsDialog()
    {
        InitializeComponent();

        WeakReferenceMessenger.Default.Register<CloseMessage, string>(this, "InstructionsDialog", (s, e) =>
        {
            this.Close();
        });

        this.Closed += (s, e) =>
        {
            WeakReferenceMessenger.Default.Unregister<CloseMessage, string>(this, "InstructionsDialog");
        };

    }

}
