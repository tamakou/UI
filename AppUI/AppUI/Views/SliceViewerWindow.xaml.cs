using System.Windows;
using System.Windows.Input;
using AppUI.Services;
using AppUI.ViewModels;
using CommunityToolkit.Mvvm.Messaging;

namespace AppUI.Views;

/// <summary>
/// SliceViewerWindow.xaml の相互作用ロジック
/// </summary>
public partial class SliceViewerWindow : Window
{
    public SliceViewerWindow()
    {
        InitializeComponent();

        this.Loaded += OnLoaded;

        WeakReferenceMessenger.Default.Register<CloseMessage>(this, (s, e) =>
        {
            this.Close();
        });

        this.Closed += (s, e) =>
        {
            WeakReferenceMessenger.Default.Unregister<CloseMessage>(this);
        };

    }
    private void OnLoaded(object sender, RoutedEventArgs e)
    {
        var workingArea = SystemParameters.WorkArea;
        double workWidth = workingArea.Width;
        double workHeight = workingArea.Height;

        double size = Math.Min(workWidth, workHeight * 0.9);

        this.Width = size;
        this.Height = size;

        this.Left = workingArea.Left + (workWidth - this.Width) / 2;
        this.Top = workingArea.Top + (workHeight - this.Height) / 2;
    }
}
