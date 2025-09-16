using System.Windows;
using CommunityToolkit.Mvvm.Messaging;
using AppUI.ViewModels;

namespace AppUI.Views.Dialog;

/// <summary>
/// GenericDialog.xaml の相互作用ロジック
/// </summary>
public partial class GenericDialog : Window
{
    public GenericDialog()
    {

        InitializeComponent();
        WeakReferenceMessenger.Default.Register<CloseMessage>(this, (s, e) =>
        {
            this.Close();
        });
        // Close時に購読解除
        this.Closed += (s, e) =>
        {
            WeakReferenceMessenger.Default.Unregister<CloseMessage>(this);
        };
    }
}
