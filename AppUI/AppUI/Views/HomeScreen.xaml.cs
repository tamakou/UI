using System.Windows;
using System.Windows.Controls;
using AppUI.ViewModels;

namespace AppUI.Views
{
    /// <summary>
    /// HomeScreen.xaml の相互作用ロジック
    /// </summary>
    public partial class HomeScreen : UserControl
    {
        public HomeScreen()
        {
            InitializeComponent();
        }

        private void PatientListColumnHeader_Click(object sender, RoutedEventArgs e)
        {
            if (sender is not GridViewColumnHeader header) return;

            if (header.Column == null) return;
            if (DataContext is HomeScreenViewModels vm)
            {
                var sortBy = header.Tag?.ToString() ?? "";
                vm.OnPatientListSort(sortBy);
            }
        }
        private void TransportListColumnHeader_Click(object sender, RoutedEventArgs e)
        {
            if (sender is not GridViewColumnHeader header) return;

            if (header.Column == null) return;
            if (DataContext is HomeScreenViewModels vm)
            {
                var sortBy = header.Tag?.ToString() ?? "";
                vm.OnTransportListSort(sortBy);
            }

        }

    }
}
