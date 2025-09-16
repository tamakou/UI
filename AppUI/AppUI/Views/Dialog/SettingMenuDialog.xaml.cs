using System.Windows;
using AppUI.ViewModels;
using AppUI.ViewModels.Dialog;
using CommunityToolkit.Mvvm.Messaging;

namespace AppUI.Views.Dialog;

/// <summary>
/// SettingMenuDialog.xaml の相互作用ロジック
/// </summary>
public partial class SettingMenuDialog : Window
{
    public SettingMenuDialog()
    {
        InitializeComponent();

        WeakReferenceMessenger.Default.Register<CloseMessage>(this, (s, e) =>
        {
            this.Close();
        });

        this.Closed += (s, e) =>
        {
            WeakReferenceMessenger.Default.Unregister<CloseMessage>(this);
        };

        this.Loaded += OnLoaded;
    }
    private void OnLoaded(object sender, RoutedEventArgs e)
    {
        const double margin = 15.0;
        if (DataContext is SettingMenuDialogViewModel vm)
        {
            this.Top = vm.Top - (this.ActualHeight + margin);
            this.Left = vm.Left;
        }
    }
}
