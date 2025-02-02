// Ignore Spelling: GATT

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Data;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;
using System.Threading.Tasks;
using AstroClient.Interop;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Microsoft.UI;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Xaml.Media;
using Windows.Devices.Bluetooth;
using Windows.Devices.Bluetooth.GenericAttributeProfile;
using Windows.Devices.Enumeration;
using Windows.Security.Cryptography;

namespace AstroClient.Models;

public partial class CameraDevice(BluetoothDevice device, DispatcherQueue dispatcher) : ObservableObject
{
    private BluetoothLEDevice? _bleDevice;
    private readonly DispatcherQueue _dispatcher = dispatcher;

    private GattDeviceService? _astroService;
    private GattCharacteristic? _configReadCharacteristic;
    private GattCharacteristic? _configWriteCharacteristic;
    private GattCharacteristic? _captureImageCharacteristic;

    public static readonly Guid GATT_Astro = new("6C7028E2-DC4A-11EF-9134-75E88A34574D");
    public static readonly Guid CHAR_ConfigRead = new("87BDECC4-DC4A-11EF-AB7F-B7E88A34574D");
    public static readonly Guid CHAR_ConfigWrite = new("994E9452-DC4A-11EF-B90E-F0E88A34574D");
    public static readonly Guid CHAR_CaptureImage = new("114fc821-6a6e-4e81-bd05-1dd5ab7a679b");

    [ObservableProperty]
    public partial ulong Id { get; private set; } = device.Id;

    [ObservableProperty]
    public partial string Address { get; private set; } = device.Address;

    [ObservableProperty]
    public partial string Name { get; private set; } = device.Name;

    [ObservableProperty]
    [NotifyPropertyChangedFor(nameof(ConnectionStatusString))]
    [NotifyPropertyChangedFor(nameof(ConnectionStatusColor))]
    [NotifyPropertyChangedFor(nameof(IsConnected))]
    [NotifyPropertyChangedFor(nameof(IsDisconnected))]
    public partial BluetoothConnectionStatus ConnectionStatus { get; set; }

    public string ConnectionStatusString => ConnectionStatus switch
    {
        BluetoothConnectionStatus.Connected => "Connected",
        BluetoothConnectionStatus.Disconnected => "Disconnected",
        _ => "Unknown",
    };

    public SolidColorBrush ConnectionStatusColor => ConnectionStatus switch
    {
        BluetoothConnectionStatus.Connected => new SolidColorBrush(Colors.Green),
        BluetoothConnectionStatus.Disconnected => new SolidColorBrush(Colors.Red),
        _ => new SolidColorBrush(Colors.Orange),
    };

    public bool IsConnected => ConnectionStatus == BluetoothConnectionStatus.Connected;

    public bool IsDisconnected => ConnectionStatus == BluetoothConnectionStatus.Disconnected;

    public ObservableCollection<CameraConfig> Config { get; } =
    [
        new() { Id = "iso", DisplayName = "ISO" },
        new() { Id = "shutterspeed", DisplayName = "Shutter Speed" },
        new() { Id = "f-number", DisplayName = "F-Stop" }
    ];

    [RelayCommand]
    public async Task ConnectAsync()
    {
        // Connect to the device
        _bleDevice = await BluetoothLEDevice.FromBluetoothAddressAsync(Id);

        // Hook into connection status
        ConnectionStatus = _bleDevice.ConnectionStatus;
        _bleDevice.ConnectionStatusChanged += OnConnectionStatusChanged;

        // Pair if not already paired
        if (!_bleDevice.DeviceInformation.Pairing.IsPaired)
        {
            var pair = await _bleDevice.DeviceInformation.Pairing.PairAsync(DevicePairingProtectionLevel.None);
            if (pair.Status != DevicePairingResultStatus.Paired)
            {
                Disconnect();
                throw new Exception($"Pairing result of {DevicePairingResultStatus.Paired} expected but got {pair.Status}!");
            }
        }

        // Get access to the config service
        var astroServiceRequest = await _bleDevice.GetGattServicesForUuidAsync(GATT_Astro, BluetoothCacheMode.Uncached);
        if (astroServiceRequest.Status != GattCommunicationStatus.Success)
        {
            Disconnect();
            throw new Exception($"Unable to find astro service on this device: {astroServiceRequest.Status}");
        }

        _astroService = astroServiceRequest.Services.FirstOrDefault();
        if (_astroService == null)
        {
            Disconnect();
            throw new Exception($"Unable to find astro service on this device: No Results");
        }

        // Request access to this service
        var accessResult = await _astroService.RequestAccessAsync();
        if (accessResult != DeviceAccessStatus.Allowed)
        {
            Disconnect();
            throw new Exception($"Unable to access astro service on this device: {accessResult}");
        }

        var characteristicsResult = await _astroService.GetCharacteristicsAsync(BluetoothCacheMode.Uncached);
        if (characteristicsResult.Status != GattCommunicationStatus.Success)
        {
            Disconnect();
            throw new Exception($"Unable to access characteristics for astro service: {astroServiceRequest.Status}");
        }

        _configReadCharacteristic = characteristicsResult.Characteristics.FirstOrDefault(x => x.Uuid == CHAR_ConfigRead);
        if (_configReadCharacteristic == null)
        {
            Disconnect();
            throw new Exception($"Unable to access config read characteristic for the astro service: Not Found");
        }

        _configWriteCharacteristic = characteristicsResult.Characteristics.FirstOrDefault(x => x.Uuid == CHAR_ConfigWrite);
        if (_configWriteCharacteristic == null)
        {
            Disconnect();
            throw new Exception($"Unable to access config write characteristic for the astro service: Not Found");
        }

        _captureImageCharacteristic = characteristicsResult.Characteristics.FirstOrDefault(x => x.Uuid == CHAR_CaptureImage);
        if (_captureImageCharacteristic == null)
        {
            Disconnect();
            throw new Exception($"Unable to access capture image characteristic for the astro service: Not Found");
        }

        _configReadCharacteristic.ValueChanged += ConfigReadCharacteristic_ValueChanged;

        // We want to listen to notify events!
        var NotifyResult = await _configReadCharacteristic.WriteClientCharacteristicConfigurationDescriptorAsync(GattClientCharacteristicConfigurationDescriptorValue.Notify);
        if (NotifyResult != GattCommunicationStatus.Success)
        {
            Disconnect();
            throw new Exception($"Unable to listen to notify events on the config read characteristic: {NotifyResult}");
        }
    }

    [RelayCommand]
    public void Disconnect()
    {
        if (_configReadCharacteristic != null)
        {
            _configReadCharacteristic.ValueChanged -= ConfigReadCharacteristic_ValueChanged;
            _configReadCharacteristic = null;
        }

        if (_configWriteCharacteristic != null)
        {
            _configWriteCharacteristic = null;
        }

        if (_captureImageCharacteristic != null)
        {
            _captureImageCharacteristic = null;
        }

        if (_astroService != null)
        {
            _astroService.Dispose();
            _astroService = null;
        }

        if (_bleDevice != null)
        {
            _bleDevice.ConnectionStatusChanged -= OnConnectionStatusChanged;

            _bleDevice.Dispose();
            _bleDevice = null;

            ConnectionStatus = BluetoothConnectionStatus.Disconnected;
        }
    }

    [RelayCommand]
    public async Task RequestConfigValueAsync(string configId)
    {
        var writeBuffer = CryptographicBuffer.ConvertStringToBinary(configId, BinaryStringEncoding.Utf8);
        await _configReadCharacteristic.WriteValueAsync(writeBuffer, GattWriteOption.WriteWithoutResponse);
    }

    public async Task WriteConfigValueAsync(string configId, string value)
    {
        var writeBuffer = CryptographicBuffer.ConvertStringToBinary($"{configId}={value}", BinaryStringEncoding.Utf8);
        await _configWriteCharacteristic.WriteValueAsync(writeBuffer, GattWriteOption.WriteWithoutResponse);
    }

    [RelayCommand]
    public async Task CaptureImageAsync()
    {
        var writeBuffer = CryptographicBuffer.ConvertStringToBinary("v", BinaryStringEncoding.Utf8);
        await _captureImageCharacteristic.WriteValueAsync(writeBuffer, GattWriteOption.WriteWithoutResponse);
    }

    private void OnConnectionStatusChanged(BluetoothLEDevice sender, object args)
    {
        if (sender != _bleDevice)
        {
            return;
        }

        _dispatcher.TryEnqueue(
            DispatcherQueuePriority.Normal,
            () =>
            {
                ConnectionStatus = sender.ConnectionStatus;
            }
        );
    }

    private void ConfigReadCharacteristic_ValueChanged(GattCharacteristic sender, GattValueChangedEventArgs args)
    {
        using var stream = args.CharacteristicValue.AsStream();
        using var binaryReader = new BinaryReader(stream);

        var bytes = binaryReader.ReadBytes((int)stream.Length);

        var config_ptr = AstroLib.decode_camera_config(bytes, (nuint)bytes.Length);
        if (config_ptr == nint.Zero)
            return; // No valid data

        try
        {
            var config = Marshal.PtrToStructure<AstroLib.CameraConfigC>(config_ptr);

            var id = AstroLib.PtrToStringUTF8(config.id);
            var value = AstroLib.PtrToStringUTF8(config.value);
            var choices = AstroLib.PtrToStringArray(config.choices, config.choices_len);
            var isReadOnly = config.is_read_only;

            lock (this)
            {
                _dispatcher.TryEnqueue(
                    DispatcherQueuePriority.Normal,
                    () =>
                    {
                        var existing = Config.FirstOrDefault(x => x.Id == id);
                        if (existing != null)
                        {
                            existing.Choices = new(choices);
                            existing.Value = value;
                            existing.IsReadOnly = isReadOnly;
                        }
                        else
                        {
                            Config.Add(
                                new CameraConfig
                                {
                                    Id = id,
                                    Choices = new(choices),
                                    Value = value,
                                    IsReadOnly = isReadOnly,
                                }
                            );
                        }
                    }
                );
            }
        }
        finally
        {
            AstroLib.free_camera_config(config_ptr);
        }
    }
}
