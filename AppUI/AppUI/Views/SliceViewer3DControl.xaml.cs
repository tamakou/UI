using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using AppUI.ViewModels;

namespace AppUI.Views
{
    /// <summary>
    /// SliceViewer3DControl.xaml の相互作用ロジック
    /// </summary>
    public partial class SliceViewer3DControl : UserControl
    {
        private bool _isDragging = false;
        private bool _isOptionDragging = false;
        private Point _startPoint;

        public SliceViewer3DControl()
        {
            InitializeComponent();
        }
        private void Canvas_MouseWheel(object sender, MouseWheelEventArgs e)
        {
            if (DataContext is SliceViewer3DControlViewModel vm)
            {
                if (e.Delta > 0)
                    vm.ZoomInButtonCommand.Execute(null);
                else
                    vm.ZoomOutButtonCommand.Execute(null);
            }
        }

        private void Canvas_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            _startPoint = e.GetPosition(ImageArea);
            ImageArea.CaptureMouse();
            if (DataContext is SliceViewer3DControlViewModel vm)
            {
                vm.OnCanvasClicked(_startPoint);
            }
            _isDragging = true;
        }
        private void Canvas_MouseMove(object sender, MouseEventArgs e)
        {
            if (_isDragging)
            {
                var currentPosition = e.GetPosition(ImageArea);
                var delta = currentPosition - _startPoint;
                _startPoint = currentPosition; // 更新して連続ドラッグ対応
                if (DataContext is SliceViewer3DControlViewModel vm)
                {
                    vm.OnMouseMove(currentPosition);
                }
            }
            else if (_isOptionDragging)
            {
                var currentPosition = e.GetPosition(ImageArea);
                var delta = currentPosition - _startPoint;

                if (DataContext is SliceViewer3DControlViewModel vm)
                {
                    vm.OnOptionMouseMove(delta.X, delta.Y);
                    _startPoint = currentPosition;//reset to current position
                }
            }
        }
        private void Canvas_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (_isDragging)
            {
                var currentPosition = e.GetPosition(ImageArea);
                _isDragging = false;
                ImageArea.ReleaseMouseCapture();
                if (DataContext is SliceViewer3DControlViewModel vm)
                {
                    vm.OnMouseLeftButtonUp(currentPosition.X, currentPosition.Y);
                }
            }
        }
        private void Canvas_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            if (DataContext is SliceViewer3DControlViewModel vm)
            {
                vm.OnCanvasSizeChanged(e.NewSize.Width, e.NewSize.Height);
            }
        }

        private void Canvas_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (_isDragging || _isOptionDragging) return;
            if (e.ChangedButton == MouseButton.Middle || e.ChangedButton == MouseButton.Right)
            {
                _startPoint = e.GetPosition(ImageArea);
                ImageArea.CaptureMouse();
                if (DataContext is SliceViewer3DControlViewModel vm)
                {
                    vm.OnOptionClicked(_startPoint.X, _startPoint.Y);
                }
                _isOptionDragging = true;
            }

        }

        private void Canvas_MouseUp(object sender, MouseButtonEventArgs e)
        {
            if (_isOptionDragging)
            {
                if (e.ChangedButton == MouseButton.Middle || e.ChangedButton == MouseButton.Right)
                {
                    var currentPosition = e.GetPosition(ImageArea);
                    _isOptionDragging = false;
                    ImageArea.ReleaseMouseCapture();
                    var delta = currentPosition - _startPoint;
                    if (DataContext is SliceViewer3DControlViewModel vm)
                    {
                        vm.OnOptionUp(currentPosition.X, currentPosition.Y, delta.X, delta.Y);
                    }

                }
            }
        }
    }
}
