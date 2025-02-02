using AstroClient.Models;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace AstroClient.Views;

public sealed partial class CameraControllerView : UserControl
{
    public static readonly DependencyProperty DeviceProperty = DependencyProperty.Register(nameof(Device), typeof(CameraDevice), typeof(CameraControllerView), null);

    public CameraDevice? Device
    {
        get => (CameraDevice?)GetValue(DeviceProperty);
        set => SetValue(DeviceProperty, value);
    }

    public CameraControllerView()
    {
        InitializeComponent();
    }

    private void Refresh_Config(object sender, RoutedEventArgs e)
    {
        if (sender is Button b)
        {
            Device?.RequestConfigValueCommand.Execute(b.Tag.ToString());
        }
    }

    private async void Write_Config(object sender, RoutedEventArgs e)
    {
        if (Device == null) return;

        if (sender is Button { DataContext: CameraConfig config })
        {
            await Device.WriteConfigValueAsync(config.Id, config.Value);
        }
    }

    private void Capture_Image(object sender, RoutedEventArgs e)
    {
        Device?.CaptureImageCommand.Execute(null);
    }
}
