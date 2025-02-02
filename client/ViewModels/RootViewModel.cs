// Ignore Spelling: Bluetooth

using System;
using System.Collections.ObjectModel;
using System.Linq;
using System.Threading.Tasks;
using AstroClient.Models;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Microsoft.UI.Dispatching;
using Windows.Devices.Bluetooth.Advertisement;

namespace AstroClient.ViewModels;

public partial class RootViewModel : ObservableRecipient
{
    private readonly BluetoothLEAdvertisementWatcher _watcher;
    private readonly DispatcherQueue _dispatcher;

    public ObservableCollection<Models.BluetoothDevice> BluetoothDevices { get; } = [];

    [ObservableProperty]
    [NotifyPropertyChangedFor(nameof(IsNotScanning))]
    [NotifyPropertyChangedFor(nameof(IsScanningAndNoItems))]
    [NotifyPropertyChangedFor(nameof(IsNotScanningAndNoItems))]
    public partial bool IsScanning { get; private set; }

    [ObservableProperty]
    [NotifyPropertyChangedFor(nameof(IsNotScanningAndNoItems))]
    public partial bool HasFirstScan { get; private set; }

    public bool IsNotScanning => !IsScanning;

    public bool IsScanningAndNoItems => IsScanning && !BluetoothDevices.Any();

    public bool IsNotScanningAndNoItems => HasFirstScan && !IsScanning && !BluetoothDevices.Any();

    [ObservableProperty]
    [NotifyPropertyChangedFor(nameof(HasSelectedDevice))]
    [NotifyPropertyChangedFor(nameof(HasNotSelectedDevice))]
    public partial CameraDevice? SelectedDevice { get; set; }

    public bool HasSelectedDevice => SelectedDevice is not null;

    public bool HasNotSelectedDevice => SelectedDevice is null;

    public RootViewModel()
    {
        _dispatcher = DispatcherQueue.GetForCurrentThread();

        _watcher = new BluetoothLEAdvertisementWatcher();
        _watcher.SignalStrengthFilter.InRangeThresholdInDBm = -70;
        _watcher.SignalStrengthFilter.OutOfRangeThresholdInDBm = -75;
        _watcher.SignalStrengthFilter.OutOfRangeTimeout = TimeSpan.FromMilliseconds(2000);
        _watcher.ScanningMode = BluetoothLEScanningMode.Active;
        _watcher.AllowExtendedAdvertisements = true;

        _watcher.Received += Watcher_Received;
        _watcher.Stopped += Watcher_Stopped;
    }

    [RelayCommand]
    private void OnStartScanning()
    {
        IsScanning = true;
        HasFirstScan = true;

        BluetoothDevices.Clear();
        _watcher.Start();
    }

    [RelayCommand]
    private void OnStopScanning()
    {
        _watcher.Stop();
    }

    [RelayCommand]
    private async Task ConnectToDeviceAsync(Models.BluetoothDevice selectedDevice)
    {
        OnStopScanning();

        try
        {
            var cameraDevice = new CameraDevice(selectedDevice, _dispatcher);
            await cameraDevice.ConnectAsync();

            SelectedDevice = cameraDevice;
        }
        catch (Exception ex)
        {
            await App.Current.Window!.ShowMessageDialogAsync(ex.Message, "Error");
        }
    }

    // ---- Watcher Events ---- //

    private void Watcher_Received(BluetoothLEAdvertisementWatcher sender, BluetoothLEAdvertisementReceivedEventArgs args)
    {
        _dispatcher.TryEnqueue(
            DispatcherQueuePriority.Normal,
            () =>
            {
                lock (this)
                {
                    if (sender != _watcher)
                        return;

                    var existingDevice = BluetoothDevices.FirstOrDefault(device => device.Id == args.BluetoothAddress);
                    if (existingDevice != null)
                    {
                        existingDevice.Update(args);
                    }
                    else
                    {
                        BluetoothDevices.Add(new Models.BluetoothDevice(args));

                        OnPropertyChanged(nameof(IsScanningAndNoItems));
                        OnPropertyChanged(nameof(IsNotScanningAndNoItems));
                    }
                }
            }
        );
    }

    private void Watcher_Stopped(BluetoothLEAdvertisementWatcher sender, BluetoothLEAdvertisementWatcherStoppedEventArgs args)
    {
        _dispatcher.TryEnqueue(
            DispatcherQueuePriority.Normal,
            () =>
            {
                lock (this)
                {
                    if (sender != _watcher)
                        return;

                    IsScanning = false;
                }
            }
        );
    }
}
