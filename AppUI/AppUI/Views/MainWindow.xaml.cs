using System.Windows;
using AppUI.ViewModels;
using CommunityToolkit.Mvvm.Messaging;

namespace AppUI.Views;

/// <summary>
/// Interaction logic for MainWindow.xaml
/// </summary>
public partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();
        this.Loaded += OnLoaded;
        WeakReferenceMessenger.Default.Register<TransitionMessage, string>(this, "WindowMinimize", (s, e) => { Minimize(); });

    }

    private void OnLoaded(object sender, RoutedEventArgs e)
    {
        // ディスプレイ情報を取得（WPFではプライマリ画面の情報しか取得できない)
        // 作業ディスプレイ情報を取得するにはWinFormsを参照する必要があるため保留
        var workingArea = SystemParameters.WorkArea;

        // 最大サイズで収まる16:9のウィンドウサイズを計算
        double screenWidth = workingArea.Width;
        double screenHeight = workingArea.Height;

        double maxRatioWidth = screenHeight * 16.0 / 9.0;
        double maxRatioHeight = screenWidth * 9.0 / 16.0;

        if (maxRatioWidth <= screenWidth)
        {
            this.Width = maxRatioWidth;
            this.Height = screenHeight;
        }
        else
        {
            this.Width = screenWidth;
            this.Height = maxRatioHeight;
        }

        // 中央に配置
        this.Left = (screenWidth - this.Width) / 2 + workingArea.Left;
        this.Top = (screenHeight - this.Height) / 2 + workingArea.Top;
    }

    private void Minimize()
    {
        this.WindowState = WindowState.Minimized;
    }
}