using System.Collections.ObjectModel;
using System.Windows;
using System.Windows.Input;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using CommunityToolkit.Mvvm.Messaging;
using CommunityToolkit.Mvvm.Messaging.Messages;
using AppUI.Services;
using System.Globalization;
using System.Windows.Data;

namespace AppUI.ViewModels.Dialog;

public enum DialogPreset
{
    CloseOnly,
    ProgressCancelable,
    ProgressDeterminate,
    YesNo,
    CloseYesNo
}
internal class ProgressUpdateMessage : ValueChangedMessage<int>
{
    public ProgressUpdateMessage(int value) : base(value) { }
}

public partial class GenericDialogViewModel : ObservableObject, IDisposable
{
    public ObservableCollection<MessageLine> MessageLines { get; set; } = new();
    public ObservableCollection<DialogButton> Buttons { get; } = new();

    public ICommand OnCloseButtonCommand { get; }
    private void OnCloseButton()
    {
        WeakReferenceMessenger.Default.Send<CloseMessage>();
    }

    [ObservableProperty]
    bool isProgressVisible = false;

    [ObservableProperty]
    bool isProgressDetermin = false;

    [ObservableProperty]
    double progressValue = 0;

    [ObservableProperty]
    bool isCloseVisible = false;

    [ObservableProperty]
    GridLength row0Length = new GridLength(2, GridUnitType.Star);

    [ObservableProperty]
    GridLength row1Length = new GridLength(6, GridUnitType.Star);

    [ObservableProperty]
    GridLength row2Length = new GridLength(3, GridUnitType.Star);

    [ObservableProperty]
    int dinamicFontSize = 20;

    public GenericDialogViewModel(DialogPreset preset): this(preset, null) { }
    public GenericDialogViewModel(DialogPreset preset, ObservableCollection<MessageLine>? messagelines)
    {
        OnCloseButtonCommand = new RelayCommand(OnCloseButton);

        if (messagelines != null)
        {
            MessageLines = messagelines;
            if(MessageLines.Count() >= 10) DinamicFontSize = 16;
            else if (MessageLines.Count() >= 8) DinamicFontSize = 18;
        }

        switch (preset)
        {
            case DialogPreset.CloseOnly:
                IsCloseVisible = true;
                Row1Length = new GridLength(9, GridUnitType.Star);
                Row2Length = new GridLength(0, GridUnitType.Star);
                break;

            case DialogPreset.YesNo:
                Buttons.Add(new DialogButton("はい", new RelayCommand(() => { YesAction?.Invoke(); OnCloseButton(); })));
                Buttons.Add(new DialogButton("いいえ", new RelayCommand(() => { NoAction?.Invoke(); OnCloseButton(); })));
                break;

            case DialogPreset.ProgressCancelable:
                IsProgressVisible = true;
                Buttons.Add(new DialogButton("キャンセル", new RelayCommand(() => { CancelAction?.Invoke(); OnCloseButton(); })));
                break;

            case DialogPreset.ProgressDeterminate:
                IsProgressVisible = true;
                IsProgressDetermin = true;
                Buttons.Add(new DialogButton("キャンセル", new RelayCommand(() => { CancelAction?.Invoke(); OnCloseButton(); })));
                break;

            case DialogPreset.CloseYesNo:
                IsCloseVisible = true;
                Buttons.Add(new DialogButton("はい", new RelayCommand(() => { YesAction?.Invoke(); OnCloseButton(); })));
                Buttons.Add(new DialogButton("いいえ", new RelayCommand(() => { NoAction?.Invoke(); OnCloseButton(); })));
                break;
        }

        WeakReferenceMessenger.Default.Register<ProgressUpdateMessage>(this, (s, e) =>
        {
            Application.Current.Dispatcher.Invoke(() => {
                ProgressValue = e.Value;
            });
            Log.Debug("Update Progress {0}", e.Value);
        });

    }
    public void Dispose()
    {
        WeakReferenceMessenger.Default.UnregisterAll(this);
        GC.SuppressFinalize(this);
    }

    public Action? CloseAction { get; set; }
    public Action? YesAction { get; set; }
    public Action? NoAction { get; set; }
    public Action? CancelAction { get; set; }

}

public class DialogButton
{
    public string Label { get; }
    public ICommand Command { get; }

    public DialogButton(string label, ICommand command)
    {
        Label = label;
        Command = command;
    }
}

public class MessageLine
{
    public MessageLine(string column1)
    {
        Column1 = column1;
    }
    public MessageLine(string column1, string column2, Thickness? setMargin=null)
    {
        Column1 = column1;
        Column2 = column2;
        Margin = setMargin;
    }

    public string Column1 { get; set; } = "";
    public string? Column2 { get; set; }
    public Thickness? Margin { get; set; }
}

public class NullToVisibilityConverter : IValueConverter
{
    public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
    {
        if (value is null) return Visibility.Collapsed;
        return Visibility.Visible;
    }

    public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
    {
        throw new NotImplementedException();
    }
}

public class SharedSizeGroupConverter : IValueConverter
{
    public object? Convert(object value, Type targetType, object parameter, CultureInfo culture)
        => value == null ? null : "LabelColumn";
    public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        => throw new NotImplementedException();
}