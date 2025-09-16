using System.Windows;

namespace AppUI.Services;

public interface IDialogService
{
    bool? ShowDialog(object viewModel);
}

public class DialogService : IDialogService
{
    public bool? ShowDialog(object viewModel)
    {
        Window dialog = CreateDialogFromViewModel(viewModel) ?? throw new InvalidOperationException("create dialog failed.");
        dialog.WindowStartupLocation = WindowStartupLocation.CenterOwner;
        dialog.ResizeMode = ResizeMode.NoResize;
        dialog.Owner = Application.Current.MainWindow;
        dialog.WindowStyle = WindowStyle.None;
        dialog.ShowInTaskbar = false;
        dialog.DataContext = viewModel;
        return dialog.ShowDialog();
    }

    public bool? ShowDialog(object viewModel, WindowStartupLocation location)
    {
        Window dialog = CreateDialogFromViewModel(viewModel) ?? throw new InvalidOperationException("create dialog failed.");
        dialog.WindowStartupLocation = location;
        dialog.ResizeMode = ResizeMode.NoResize;
        dialog.Owner = Application.Current.MainWindow;
        dialog.WindowStyle = WindowStyle.None;
        dialog.ShowInTaskbar = false;
        dialog.DataContext = viewModel;
        return dialog.ShowDialog();
    }

    private static Window? CreateDialogFromViewModel(object viewModel)
    {
        var vmType = viewModel.GetType();
        var vmFullName = vmType.FullName ?? throw new InvalidOperationException("ViewModel has no FullName");

        var lastDotIndex = vmFullName.LastIndexOf('.');
        if (lastDotIndex < 0)
            throw new InvalidOperationException("Invalid ViewModel name format.");
        var ns = vmFullName.Substring(0, lastDotIndex).Replace("ViewModels", "Views");
        var className = vmFullName.Substring(lastDotIndex + 1);
        if (!className.EndsWith("ViewModel"))
            throw new InvalidOperationException("class name must end with 'ViewModel'.");
        var viewClassName = className.Replace("ViewModel", "");
        var viewTypeName = $"{ns}.{viewClassName}, {vmType.Assembly.FullName}";

        var viewType = Type.GetType(viewTypeName)
                       ?? throw new InvalidOperationException($"View not found: {viewTypeName}");
        var window = Activator.CreateInstance(viewType) as Window;
        if (window != null)
        {
            window.DataContext = viewModel;
        }
        return window;
    }

    public static void ShowError(string message, string title)
    {
        MessageBox.Show(message, title, MessageBoxButton.OK, MessageBoxImage.Error);
    }
}