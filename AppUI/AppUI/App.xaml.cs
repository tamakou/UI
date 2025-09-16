using System.Windows;
using AppUI.Services;
using AppUI.Views;
using AppUI.ViewModels;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using AppUI.ViewModels.Dialog;
using AppUI.Views.Dialog;
using AppUI.Services.Models;
using System.Windows.Threading;
using AppUI.Core;
using System.Globalization;

namespace AppUI
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        private static Mutex? _mutex;
        public static IHost AppHost { get; private set; } = null!;
        protected override async void OnStartup(StartupEventArgs e)
        {
            const string mutexName = "ZeusPcUIMutex";

            _mutex = new Mutex(true, mutexName, out var isNewInstance);

            if (!isNewInstance)
            {
                MessageBox.Show(MessageService.GetMessage(MessageCode.MultipleAppLaunch), "", MessageBoxButton.OK, MessageBoxImage.Information);
                Shutdown();
                return;
            }

            AppDomain.CurrentDomain.UnhandledException += OnUnhandledException;
            DispatcherUnhandledException += OnDispatcherUnhandledException;
            //TaskScheduler.UnobservedTaskException += OnUnobservedTaskException;

            AppHost = Host.CreateDefaultBuilder()
                .ConfigureServices((context, services) =>
                {
                    services.AddSingleton<IDialogService, DialogService>();
                    services.AddSingleton<IImageStorageService, ImageStorageService>();
                    //services.AddSingleton<ISettingTextJson, SettingTextJson>();
                    services.AddSingleton<AppStateService>();
                    services.AddSingleton<MainWindowViewModel>();
                    services.AddSingleton<HomeScreenViewModels>(sp =>
                    {
                        var dialog = sp.GetRequiredService<IDialogService>();
                        var state = sp.GetRequiredService<AppStateService>();

                        return new HomeScreenViewModels(
                            dialog,
                            sp,
                            state
                        );
                    });
                    services.AddSingleton<MainWindowViewModel>(sp =>
                    {
                        var mainVM = new MainWindowViewModel(
                            sp.GetRequiredService<IImageStorageService>(),
                            sp.GetRequiredService<IDialogService>(),
                            sp,
                            sp.GetRequiredService<AppStateService>(),
                            sp.GetRequiredService<MarkerRoiSettingsStateService>()
                        );

                        var homeVM = sp.GetRequiredService<HomeScreenViewModels>();
                        homeVM.ShowMarkerRoiSettings = () =>
                            mainVM.ShowMarkerRoiSettingsScreenCommand.Execute(null);

                        return mainVM;
                    });
                    services.AddTransient<GenericDialogViewModel>();
                    services.AddTransient<SliceViewerWindowViewModel>();
                    services.AddTransient<SliceViewer2DControlViewModel>();
                    services.AddTransient<SliceViewer3DControlViewModel>();
                    services.AddSingleton<MarkerRoiSettingsStateService>();
                    services.AddSingleton<SettingMenuDialog>();
                    services.AddTransient<MarkerPageFooterViewModel>();
                    services.AddSingleton<StateProgressViewModel>();

                })
                .Build();

            await AppHost.StartAsync();

            var mainWindow = new MainWindow
            {
                DataContext = AppHost.Services.GetRequiredService<MainWindowViewModel>()
            };
            mainWindow.Show();

            base.OnStartup(e);
        }
        private void OnDispatcherUnhandledException(object sender, DispatcherUnhandledExceptionEventArgs e)
        {
            ShowExceptionDialog(e.Exception);
            e.Handled = true;
        }

        private void OnUnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            if (e.ExceptionObject is Exception ex)
            {
                ShowExceptionDialog(ex);
            }
        }

        private void OnUnobservedTaskException(object? sender, UnobservedTaskExceptionEventArgs e)
        {
            ShowExceptionDialog(e.Exception);
            e.SetObserved();
        }

        private static void ShowExceptionDialog(Exception ex)
        {
            var test = ex.GetBaseException();
            if (ex is ExpectedException expectedEx)
            {
                var unknownErrMsg = MessageService.GetMessage(MessageCode.UnknownError);
                var errMessage = string.IsNullOrEmpty(expectedEx.ErrCode) ?
                                    expectedEx.Message :
                                    $"[{expectedEx.ErrCode}] {expectedEx.Message}";
                errMessage = string.IsNullOrEmpty(errMessage) ? unknownErrMsg : errMessage;

                if (Current.MainWindow.DataContext != null)
                {
                    var dialogVm = new GenericDialogViewModel(DialogPreset.CloseOnly)
                    {
                        MessageLines = [new(errMessage)],
                        YesAction = new Action(() => { }),
                        CancelAction = new Action(() => { }),
                    };

                    var dialogService = AppHost.Services.GetRequiredService<IDialogService>();
                    dialogService.ShowDialog(dialogVm);

                    var type = ex.GetType().Name;
                    switch (type)
                    {
                        case "WarningException":
                            return; // Warningエラーの場合はアプリ終了させないためreturn

                        case "FatalException":
                        default:
                            break;
                    }
                }
                else
                {
                    DialogService.ShowError(expectedEx.Message, expectedEx.ErrCode);
                }
            }
            //else if(ex is ExpectedException expectedEx)
            //{
            //    DialogService.ShowError(expectedEx.Message, expectedEx.ErrCode);
            //}
            else
            {
                DialogService.ShowError(ex.Message, ex.GetType().ToString());
            }

            Current.Shutdown();
        }
        protected override async void OnExit(ExitEventArgs e)
        {
            ThProc.Terminaite();

            AppDomain.CurrentDomain.UnhandledException -= OnUnhandledException;
            DispatcherUnhandledException -= OnDispatcherUnhandledException;
            //TaskScheduler.UnobservedTaskException -= OnUnobservedTaskException;

            await AppHost.StopAsync();
            AppHost.Dispose();

            base.OnExit(e);
        }
    }
    public class ExpectedException(string code, string message) : Exception(message)
    {
        public string ErrCode { get; set; } = code;
    }
    public class WarningException : ExpectedException
    {
        public WarningException(string code, string message) : base(code, message) { }
        public WarningException(string message) : base("00000", message) { }
    }
    public class FatalException : ExpectedException
    {
        public FatalException(string code, string message) : base(code, message) { }
        public FatalException(string message) : base("00000", message) { }
    }

}


