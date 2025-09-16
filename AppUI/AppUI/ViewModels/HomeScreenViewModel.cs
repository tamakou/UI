using System.Collections.ObjectModel;
using System.Windows;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using CommunityToolkit.Mvvm.Messaging;
using AppUI.Core;
using AppUI.Services;
using AppUI.ViewModels.Dialog;
using AppUI.Services.Models;
using Microsoft.Extensions.DependencyInjection;
using System.ComponentModel;
using System.IO;
using System.Windows.Threading;
using System.Windows.Data;
using System.Windows.Controls;

namespace AppUI.ViewModels;

public partial class HomeScreenViewModels : ObservableObject, IDisposable
{
    private readonly IDialogService _dialogService;
    private readonly IServiceProvider _serviceProvider;
    private readonly AppStateService _appStateService;

    public ObservableObject HedaerControl { get; private set; }

    public HomeScreenViewModels(
        IDialogService dialogService,
        IServiceProvider serviceProvider,
        AppStateService appStateService)
    {
        HedaerControl = new MarkerPageHeaderViewModel(appStateService);

        _dialogService = dialogService;
        _serviceProvider = serviceProvider;
        _appStateService = appStateService;

        _appStateService.PropertyChanged += OnAppStateChanged;
        StateProgressViewModel = new StateProgressViewModel(_appStateService);

        OnNewDataButtonCommand = new AsyncRelayCommand(OnNewDataButton);
        OnUsbTransportButtonCommand = new AsyncRelayCommand(OnUsbTransportButton);
        OnDeleteTransportListButtonCommand = new AsyncRelayCommand(OnDeleteTransportListButton);

        PatientListView = CollectionViewSource.GetDefaultView(PatientList);
        TransportListView = CollectionViewSource.GetDefaultView(TransportList);

        WeakReferenceMessenger.Default.Register<CloseMessage>(this, (s, e) =>
        {
            Log.Debug("MainWindow ViewModel");
        });
        _serviceProvider = serviceProvider;
        _appStateService = appStateService;

        Initialize();
    }

    public void Initialize()
    {
        try
        {
            ThProc.Initialize(0); // #142,#145 DLL内部リセットのため呼び出しが必要
        }
        catch
        {
            Log.Warn("HomeScreenViewModel Initialize() ThProc.Initialize(0) failed.");
            throw;
        }

        UpdatePatientList();
        UpdateTransportList();
    }

    private void OnAppStateChanged(object? sender, PropertyChangedEventArgs? e)
    {
        if (e?.PropertyName == nameof(AppStateService.StateType))
        {
            ActiveNewDataStatus.IsActive = _appStateService.StateType == MainStateType.AddNewData;
            ActiveUsbTransportStatus.IsActive = _appStateService.StateType == MainStateType.UsbTransport;
            ActiveXrealStatus.IsActive = _appStateService.StateType == MainStateType.Xreal;
            ActiveXrealiPhoneStatus.IsActive = _appStateService.StateType == MainStateType.XrealiPhone;
            ActiveSettingStatus.IsActive = _appStateService.StateType switch
            {
                MainStateType.Setting => true,
                MainStateType.Version => true,
                MainStateType.Inquiry => true,
                MainStateType.Manual => true,
                _ => false,
            };
        }
    }

    private string _patientListSortProperty = "StudyDate";

    private ListSortDirection _patientListDir = ListSortDirection.Descending;
    public ICollectionView PatientListView { get; set; }
    public ObservableCollection<ThProcPatientInfo> PatientList { get; set; } = [];

    [ObservableProperty]
    public ThProcPatientInfo? selectedItem;

    private string _transportListSortProperty = "Patient.StudyDate";

    private ListSortDirection _transportListDir = ListSortDirection.Descending;
    public ICollectionView TransportListView { get; set; }
    public ObservableCollection<ThProcTransportPatientInfo> TransportList { get; set; } = [];

    [ObservableProperty]
    public ThProcTransportPatientInfo? transportSelectedItem;

    [ObservableProperty]
    public int transportSelectedItem2 = -1;

    [ObservableProperty]
    public ActiveContentsViewModel activeNewDataStatus = new();

    [ObservableProperty]
    public ActiveContentsViewModel activeUsbTransportStatus = new();

    [ObservableProperty]
    public ActiveContentsViewModel activeXrealStatus = new();

    [ObservableProperty]
    public ActiveContentsViewModel activeXrealiPhoneStatus = new();

    [ObservableProperty]
    public ActiveContentsViewModel activeSettingStatus = new();

    public StateProgressViewModel StateProgressViewModel { get; } = null!;
    public Action? ShowMarkerRoiSettings { get; set; }

    private void UpdatePatientList()
    {
        PatientList.Clear();
        var size = ThProc.GetCTDataListSize();
        for (var i = 0; i < size; i++)
        {
            if (ThProc.GetCTDataList(i, out var patientInfo))
            {
                var thProcPatientInfo = CreatePatientInfoFromNativeInfo(patientInfo);
                PatientList.Add(thProcPatientInfo);
            }
            else
            {
                ThProc.GetLastError(out var errCode, out var errMessage);
                throw new WarningException(errCode, errMessage);
            }
        }

        if (_patientListSortProperty != "")
        {
            PatientListView.SortDescriptions.Clear();
            PatientListView.SortDescriptions.Add(new SortDescription(_patientListSortProperty, _patientListDir));
            PatientListView.Refresh();
        }
    }

    private void UpdateTransportList()
    {
        TransportList.Clear();
        var size = ThProc.GetTransportDataListSize();
        for (var i = 0; i < size; i++)
        {
            if (ThProc.GetTransportDataList(i, out var patientInfo))
            {
                if (patientInfo.OutputType == (int)CTDataTransportType.Xreal)
                {
                    var thProcPatientInfo = new ThProcTransportPatientInfo()
                    {
                        Patient = CreatePatientInfoFromNativeInfo(patientInfo),
                        TransportType = CTDataTransportType.Xreal,
                    };
                    TransportList.Add(thProcPatientInfo);
                }
                else if (patientInfo.OutputType == (int)CTDataTransportType.iPhone)
                {
                    var thProcPatientInfoiPhone = new ThProcTransportPatientInfo()
                    {
                        Patient = CreatePatientInfoFromNativeInfo(patientInfo),
                        TransportType = CTDataTransportType.iPhone
                    };
                    TransportList.Add(thProcPatientInfoiPhone);
                }
                if (patientInfo.OutputType == (int)CTDataTransportType.XrealIPhone)
                {
                    // XREAL/IPhoneの場合はリストを2つに分けて表示する必要がある
                    var thProcPatientInfoXreal = new ThProcTransportPatientInfo()
                    {
                        Patient = CreatePatientInfoFromNativeInfo(patientInfo),
                        TransportType = CTDataTransportType.Xreal
                    };
                    var thProcPatientInfoiPhone = new ThProcTransportPatientInfo()
                    {
                        Patient = CreatePatientInfoFromNativeInfo(patientInfo),
                        //      TransportType = CTDataTransportType.Xreal
                        //#32_fixed_2025_05_30 N.Furutsuki
                        TransportType = CTDataTransportType.iPhone
                    };
                    TransportList.Add(thProcPatientInfoXreal);
                    TransportList.Add(thProcPatientInfoiPhone);
                }
            }
            else
            {
                ThProc.GetLastError(out var errCode, out var errMessage);
                throw new WarningException(errCode, errMessage);
            }
        }

        if (_transportListSortProperty != "")
        {
            TransportListView.SortDescriptions.Clear();
            TransportListView.SortDescriptions.Add(new SortDescription(_transportListSortProperty, _transportListDir));
            TransportListView.Refresh();
        }
    }

    private static ThProcPatientInfo CreatePatientInfoFromNativeInfo(CsPatientInfo ctListInfo)
    {
        return new ThProcPatientInfo()
        {
            PatientName = ctListInfo.PatientName,
            PatientID = ctListInfo.PatientID,
            Birthday = ctListInfo.Birthday,
            Age = ctListInfo.Age.ToString(),
            Gender = ctListInfo.Gender,
            StudyDate = ctListInfo.StudyDate,
            StudyTime = ctListInfo.StudyTime,
            StudyDateTime = $"{ctListInfo.StudyDate} {ctListInfo.StudyTime}",
            SliceThickness = ctListInfo.SliceThickness.ToString(),
            ProcessStatus = ctListInfo.ProcessStatus == (int)RoiType.NoRoi ? "" : "〇",
            //CTMode = ctListInfo.CTMode,
            UpdateDate = ctListInfo.UpdateDate,
            UpdateTime = ctListInfo.UpdateTime,
            StudyPattern = ctListInfo.StudyPattern.ToString(),  // 番号？
            StudDescription = ctListInfo.StudDescription,
            StudyUID = ctListInfo.StudyUID,
        };
    }

    public void OnPatientListSort(string propertyName)
    {
        if (_patientListSortProperty == propertyName)
        {
            _patientListSortProperty = "";
            _patientListDir = ListSortDirection.Descending;
        }
        else
        {
            _patientListSortProperty = propertyName;
            _patientListDir = ListSortDirection.Descending;
        }

        PatientListView.SortDescriptions.Clear();
        if (_patientListSortProperty != "")
        {
            PatientListView.SortDescriptions.Add(new SortDescription(propertyName, _patientListDir));
            PatientListView.Refresh();
        }
    }
    public void OnTransportListSort(string propertyName)
    {
        if (_transportListSortProperty == propertyName)
        {
            _transportListSortProperty = "";
            _transportListDir = ListSortDirection.Descending;
        }
        else
        {
            _transportListSortProperty = propertyName;
            _transportListDir = ListSortDirection.Descending;
        }

        TransportListView.SortDescriptions.Clear();
        if (_transportListSortProperty != "")
        {
            TransportListView.SortDescriptions.Add(new SortDescription(propertyName, _transportListDir));
            TransportListView.Refresh();
        }
    }

    public IAsyncRelayCommand OnNewDataButtonCommand { get; }
    private async Task OnNewDataButton()
    {
        _appStateService.StateType = MainStateType.AddNewData;

        var importFlg = ThProc.IsImportFromCD(out var importPath);
        if (importFlg)
        {
            var dlgMessage = MessageService.GetMessage(MessageCode.DlgMsgImportRomSetting);
            var dialogVm = new GenericDialogViewModel(DialogPreset.CloseOnly)
            {
                MessageLines = [new(dlgMessage)],
            };

            var cdRomDrives = DriveInfo.GetDrives()
            .Where(d => d.DriveType == DriveType.CDRom && d.IsReady);

            if (!cdRomDrives.Any())
            {
                var timer = new DispatcherTimer
                {
                    Interval = TimeSpan.FromSeconds(0.05)
                };
                timer.Tick += (sender, e) =>
                {
                    var test = DriveInfo.GetDrives();
                    var drivesResult = DriveInfo.GetDrives()
                        .Where(d => d.DriveType == DriveType.CDRom && d.IsReady).Any();

                    if (drivesResult)
                    {
                        WeakReferenceMessenger.Default.Send<CloseMessage>();
                        timer?.Stop();
                        timer = null;
                    }
                };
                timer.Start();
                _dialogService.ShowDialog(dialogVm);
            }

            cdRomDrives = DriveInfo.GetDrives().Where(d => d.DriveType == DriveType.CDRom && d.IsReady);

            if (!cdRomDrives.Any())
            {
                dlgMessage = MessageService.GetMessage(MessageCode.DlgMsgImportRomNotFound);
                dialogVm.MessageLines = [new(dlgMessage)];
                _dialogService.ShowDialog(dialogVm);

                _appStateService.StateType = MainStateType.Normal;
                return;
            }
            else
            {
                var cnt = 0;
                Log.Info("ImportPath:{0}", importPath);
                foreach (var drive in cdRomDrives)
                {
                    Log.Info("cdRomDrive{0} Name:{1} Type:{2} IsReady:{3} DriveFormat:{4} RootDirectory:{5} VolumeLabel:{6}",
                        cnt, drive.Name, drive.DriveType, drive.IsReady, drive.DriveFormat, drive.RootDirectory, drive.VolumeLabel);
                }
            }
        }

        if (!System.IO.Directory.Exists(importPath))
        {
            var dlgMessage = MessageService.GetMessage(importFlg ? MessageCode.DlgMsgImportDriveNoCDRom : MessageCode.DlgMsgImportDataNotFound);
            var dialogVm = new GenericDialogViewModel(DialogPreset.CloseOnly)
            {
                MessageLines = [new(dlgMessage)],
            };

            _dialogService.ShowDialog(dialogVm);

            _appStateService.StateType = MainStateType.Normal;
            return;
        }

        var cts = new CancellationTokenSource();
        var pollingResult = new ThProcCmdStatus();
        try
        {
            var poolingTask = ThProc.ExecuteImportCommand(out ThProcCmdOutput importResutl,
                cts,
                onProgress: statusResult => { WeakReferenceMessenger.Default.Send<ProgressUpdateMessage>(new(statusResult.CmdProgress)); },
                onSucceeded: statusResult => { WeakReferenceMessenger.Default.Send<CloseMessage>(); });

            if (poolingTask.Status != TaskStatus.Canceled ||
                poolingTask.Status != TaskStatus.Faulted)
            {
                var dlgMessage = MessageService.GetMessage(MessageCode.DlgMsgImportDataCopying);
                var copydialogVm = new GenericDialogViewModel(DialogPreset.ProgressCancelable)
                {
                    MessageLines = [new(dlgMessage)],
                    CancelAction = new Action(() => { cts.Cancel(); }),
                };
                _dialogService.ShowDialog(copydialogVm);
            }
            pollingResult = await poolingTask;
        }
        catch (OperationCanceledException)
        {
            Log.Info("CTData import canceled.");
            WeakReferenceMessenger.Default.Send<CloseMessage>();
            return;
        }
        catch (Exception ex)
        {
            Log.Error(ex.Message);
            throw;
        }
        finally
        {
            _appStateService.StateType = MainStateType.Normal;
        }

        UpdatePatientList(); // リストを更新

        if (!ThProc.ExecuteCheckImageServerCommand())
        {
            var dlgMesage = MessageService.GetMessage(MessageCode.DLLImageServerError);
            Log.Fatal(dlgMesage);
            throw new FatalException(dlgMesage);
        }

        var storageService = new ImageStorageService();

        var sliceContentVM = _serviceProvider.GetRequiredService<SliceViewer2DControlViewModel>();

        var studyUid = pollingResult.StrParam1;
        var patientInfo = PatientList.Where(i => i.StudyUID == studyUid);
        if (!patientInfo.Any())
        {
            Log.Error("StudyUid:{0} is not found in PatheintList.", studyUid);
            patientInfo = PatientList;
        }

        try
        {
            var initTask = sliceContentVM.Initialize(studyUid, true);

            if (initTask.Status != TaskStatus.Faulted &&
                initTask.Status != TaskStatus.Canceled)
            {
                var sliceDialogVm = _serviceProvider.GetRequiredService<SliceViewerWindowViewModel>();
                sliceDialogVm.CurrentViewModel = sliceContentVM;
                _dialogService.ShowDialog(sliceDialogVm);
            }

            await initTask;
        }
        catch
        {
            throw;
        }
        finally
        {
            sliceContentVM.ReleaseRendering();
            _appStateService.StateType = MainStateType.Normal;
        }

        UpdatePatientList();

        try
        {
            ThProc.Initialize(0); // #142,#145 DLL内部リセットのため呼び出しが必要
        }
        catch (ExpectedException ex)
        {
            throw new WarningException(ex.ErrCode, ex.Message);
        }
        catch
        {
            Log.Warn("HomeScreenViewModel OnNewDataButton() ThProc.Initialize(0) failed.");
        }
    }

    [RelayCommand]
    private void MarkerRoiButton()
    {
        ShowMarkerRoiSettings?.Invoke();
    }

    [RelayCommand]
    private void CTDataListSelectDeleteButton()
    {
        const string PatientName = "患者名：";
        const string PatientID = "患者ID：";
        const string StudyDate = "撮影日：";

        if (SelectedItem == null) return;
        var patient = SelectedItem;

        var isDelete = false;
        var dlgMesage = MessageService.GetMessage(MessageCode.DlgMsgDeleteThisPatient);
        _dialogService.ShowDialog(new GenericDialogViewModel(DialogPreset.YesNo)
        {
            MessageLines = [new(PatientName, patient.PatientName), new(PatientID, patient.PatientID), new(StudyDate, patient.StudyDate), new(dlgMesage)],
            YesAction = new Action(() => { isDelete = true; }),
            CancelAction = new Action(() => { isDelete = false; }),
        });

        if (isDelete)
        {
#if DEMO_PROC
            PatientList.Remove(SelectedItem);
#else
            if (!ThProc.ExecuteCTDataListDeleteCommand(patient.StudyUID, out ThProcCmdOutput output))
            {
                Log.Error("CTDataList delete failed. StudyUID:{0}.", patient.StudyUID);
                ThProc.GetLastError(out var errCode, out var errMessage);
                throw new WarningException(errCode, errMessage);
            }

            UpdatePatientList();
#endif
            SelectedItem = null;

            dlgMesage = MessageService.GetMessage(MessageCode.DlgMsgDeletedPatient);
            _dialogService.ShowDialog(new GenericDialogViewModel(DialogPreset.CloseOnly)
            {
                MessageLines = [new(dlgMesage)],
            });
        }
    }

    [RelayCommand]
    private void CTDataListAllDeleteButton()
    {
        if (PatientList.Count <= 0) return;
        var isDelete = false;

        var dlgMessage = MessageService.GetMessage(MessageCode.DlgMsgDeleteAllPatient);
        var dialogVm = new GenericDialogViewModel(DialogPreset.YesNo)
        {
            MessageLines = [new(dlgMessage)],
            YesAction = new Action(() => { isDelete = true; }),
            CancelAction = new Action(() => { isDelete = false; }),
        };
        var result = _dialogService.ShowDialog(dialogVm);

        if (isDelete == true)
        {

#if DEMO_PROC
            PatientList.Clear();
#else
            var errCode = "";
            var errMessage = "";
            var succeeded = true;
            foreach (var patient in PatientList)
            {
                if (!ThProc.ExecuteCTDataListDeleteCommand(patient.StudyUID, out ThProcCmdOutput output))
                {
                    Log.Warn("Patient list delete failed StudyUID:{0}.", patient.StudyUID);
                    ThProc.GetLastError(out errCode, out errMessage);
                    Log.Warn("[{0}] {1}", errCode, errMessage);
                    succeeded = false;
                    break;
                }
            }

            UpdatePatientList();

            if (!succeeded) throw new WarningException(errCode, errMessage);

#endif
            SelectedItem = null;

            dlgMessage = MessageService.GetMessage(MessageCode.DlgMsgDeletedAllPatient);
            _dialogService.ShowDialog(new GenericDialogViewModel(DialogPreset.CloseOnly)
            {
                MessageLines = [new(dlgMessage)],
            });
        }
    }
    [RelayCommand]
    private void CTDataListContextMenu1()
    {
        /*
         * CTデータリストのコンテキストメニュー(1段目)の処理はここに記述します。
         */
        MarkerRoiButton();
    }
    [RelayCommand]
    private void CTDataListContextMenu2()
    {
        /*
         * CTデータリストのコンテキストメニュー(2段目)の処理はここに記述します。
         */
        CTDataListSelectDeleteButton();
    }

    public IAsyncRelayCommand OnUsbTransportButtonCommand { get; }
    private async Task OnUsbTransportButton()
    {
        if (TransportSelectedItem == null) return;
        var transport = TransportSelectedItem;

        _appStateService.StateType = MainStateType.UsbTransport;

        var homeFolderPath = "";
        try
        {
            ThProc.GetEnvironment(out var env);
            homeFolderPath = env.HomeFolder.Replace('/', '\\');
        }
        catch(ExpectedException ex)
        {
            Log.Error("HomeScreenViewModel OnUsbTransportButton() GetEnvironment failed.");
            Log.Warn("[{0}] {1}", ex.ErrCode, ex.Message);
        }
        catch
        {
            _appStateService.StateType = MainStateType.Normal;
            throw;
        }

        try
        {
#if DEMO_PROC
            var meshFolderPath = $@"{homeFolderPath}\Model";
#else
            var meshFolderPath = ThProc.ExecuteGetMeshFolder(transport.Patient.StudyUID, transport.TransportType);
            if (meshFolderPath == "")
            {
                ThProc.GetLastError(out var errCode, out var errMessage);
                Log.Warn("[{0}] {1}", errCode, errMessage);
            }
            meshFolderPath = meshFolderPath.Replace('/', '\\');
#endif

            var dispMessage = MessageService.GetMessage(FolderPusherResult.BatchFileNotExists);
            if (FolderPusherService.BatchFileExists(homeFolderPath))                
            {
                dispMessage = MessageService.GetMessage(FolderPusherResult.SourceFileNotFound);
                if (Directory.Exists(meshFolderPath))
                {
                    var poolingTask = FolderPusherService.PushFolderToDeviceAsync(
                        homeFolderPath,
                        meshFolderPath,
                        transport.TransportType,
                        onSuccess: new Action(() => { WeakReferenceMessenger.Default.Send<CloseMessage>(); }),
                        onFailed: new Action(() => { WeakReferenceMessenger.Default.Send<CloseMessage>(); }));

                    var dlgMessage = MessageService.GetMessage(MessageCode.DlgMsgUSBDataTransfer);
                    _dialogService.ShowDialog(new GenericDialogViewModel(DialogPreset.ProgressDeterminate)
                    {
                        MessageLines = [new(dlgMessage)],
                    });
                    FolderPusherService.KillProcess();

                    var poolingResult = await poolingTask;

                    dispMessage = MessageService.GetMessage(poolingResult);
                }
            }

            _dialogService.ShowDialog(new GenericDialogViewModel(DialogPreset.CloseOnly)
            {
                MessageLines = [new(dispMessage)],
            });

        }
        catch (Exception ex)
        {
            Log.Warn(ex.Message);
            throw;
        }
        finally
        {
            _appStateService.StateType = MainStateType.Normal;
            FolderPusherService.KillProcess();
        }
    }

    public IAsyncRelayCommand OnDeleteTransportListButtonCommand { get; }
    private async Task OnDeleteTransportListButton()
    {
        if (TransportSelectedItem == null) return;
        var transport = TransportSelectedItem;

        var cts = new CancellationTokenSource();
        var pollingResult = new ThProcCmdStatus();
        try
        {
            var poolingTask = ThProc.ExecuteOutputRemoveCommand(
                transport.Patient.StudyUID,
                transport.TransportType,
                out ThProcCmdOutput exportResult,
                cts);

            pollingResult = await poolingTask;

        }
        catch (WarningException ex)
        {
            Log.Warn("[{0}] {1}", ex.ErrCode, ex.Message);
            throw;
        }
        finally
        {
            UpdateTransportList();
        }

        var dlgMessage = MessageService.GetMessage(MessageCode.DlgMsgDeletedTrasport);
        var dialogVm = new GenericDialogViewModel(DialogPreset.CloseOnly)
        {
            MessageLines = [new(dlgMessage)],
        };
        _dialogService.ShowDialog(dialogVm);
    }
    [RelayCommand]
    private void TransportListContextMenu1()
    {
        /*
         * CTデータリストのコンテキストメニュー(1段目)の処理はここに記述します。
         */
        TransportListContextMenu1Dummy();
    }
    private async void TransportListContextMenu1Dummy()
    {
        await OnUsbTransportButton();
    }

    [RelayCommand]
    private void TransportListContextMenu2()
    {
        /*
         * CTデータリストのコンテキストメニュー(1段目)の処理はここに記述します。
         */
        TransportListContextMenu2Dummy();
    }
    private async void TransportListContextMenu2Dummy()
    {
        await OnDeleteTransportListButton();
    }

    [RelayCommand]
    private void XRealButton()
    {
        _appStateService.StateType = MainStateType.Xreal;
        ExecuteTransportEvent(CTDataTransportType.Xreal);
        _appStateService.StateType = MainStateType.Normal;
    }

    [RelayCommand]
    private void XRealiPhoneButton()
    {
        _appStateService.StateType = MainStateType.XrealiPhone;
        ExecuteTransportEvent(CTDataTransportType.XrealIPhone);
        _appStateService.StateType = MainStateType.Normal;
    }

    async void ExecuteTransportEvent(CTDataTransportType type)
    {
        if (SelectedItem == null) return;
        var trgetPatient = SelectedItem;

        var dlgMessage = MessageService.GetMessage(MessageCode.DlgMsgTransportCreating);
        var dialogVm = new GenericDialogViewModel(DialogPreset.ProgressCancelable)
        {
            MessageLines = [new(dlgMessage)],
        };

        var cts = new CancellationTokenSource();
        try
        {
            var poolingTask = ThProc.ExecuteOutputAddCommand(
                trgetPatient.StudyUID,
                type,
                out ThProcCmdOutput exportResult,
                cts,
                onProgress: statusResult => { WeakReferenceMessenger.Default.Send<ProgressUpdateMessage>(new(statusResult.CmdProgress)); },
                onSucceeded: statusResult => { WeakReferenceMessenger.Default.Send<CloseMessage>(); });

            if (poolingTask.Status <= TaskStatus.Running)
            {
                _dialogService.ShowDialog(dialogVm);
            }

            await poolingTask;

        }
        catch (Exception ex)
        {
            Log.Warn(ex.Message);
            throw;
        }

        UpdateTransportList();

        dlgMessage = MessageService.GetMessage(MessageCode.DlgMsgTransportCreated);
        dialogVm = new GenericDialogViewModel(DialogPreset.CloseOnly)
        {
            MessageLines = [new(dlgMessage)],
        };
        var result = _dialogService.ShowDialog(dialogVm);
    }

    [RelayCommand]
    private void SettingButton(object parameter)
    {
        var preStatus = _appStateService.StateType;
        _appStateService.StateType = MainStateType.Setting;

        var dialog = _dialogService as DialogService;
        var settingService = new SettingTextJson();
        var dialogService = new DialogService();
        var viewModel = new SettingMenuDialogViewModel(settingService, dialogService, _appStateService);

        if (parameter is Button btn)
        {
            var topLeft = btn.PointToScreen(new Point(0, 0));
            var left = topLeft.X;
            var top = topLeft.Y;
            viewModel.Top = top;
            viewModel.Left = left;
        }

        dialog?.ShowDialog(viewModel, WindowStartupLocation.Manual);

        _appStateService.StateType = preStatus;

    }
    public void Dispose()
    {
        WeakReferenceMessenger.Default.UnregisterAll(this);
        _appStateService.PropertyChanged -= OnAppStateChanged;
        GC.SuppressFinalize(this);
    }
}
