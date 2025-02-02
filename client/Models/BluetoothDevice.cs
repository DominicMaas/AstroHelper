// Ignore Spelling: Connectable Bluetooth Uuids

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using CommunityToolkit.Mvvm.ComponentModel;
using Windows.Devices.Bluetooth.Advertisement;

namespace AstroClient.Models;

public partial class BluetoothDevice : ObservableObject
{
    [ObservableProperty]
    public partial ulong Id { get; private set; }

    [ObservableProperty]
    public partial string Address { get; private set; }

    [ObservableProperty]
    public partial string Name { get; private set; } = "Unknown Device";

    [ObservableProperty]
    [NotifyPropertyChangedFor(nameof(IsPairedOrIsConnectable))]
    public partial bool IsPaired { get; private set; }

    [ObservableProperty]
    [NotifyPropertyChangedFor(nameof(IsPairedOrIsConnectable))]
    public partial bool IsConnectable { get; private set; }

    [ObservableProperty]
    [NotifyPropertyChangedFor(nameof(LastSeenFormatted))]
    public partial DateTimeOffset LastSeen { get; private set; }

    public string LastSeenFormatted => LastSeen.ToString("h:mm:ss tt");

    public bool IsPairedOrIsConnectable => IsPaired || IsConnectable;

    public ObservableCollection<Guid> ServiceUuids { get; } = [];

    public int ServiceUuidsCount => ServiceUuids.Count;

    public BluetoothDevice(BluetoothLEAdvertisementReceivedEventArgs args)
    {
        Id = args.BluetoothAddress;

        Address = args.BluetoothAddress.ToString("X12");
        Address = string.Join(":", Enumerable.Range(0, Address.Length / 2).Select(i => Address.Substring(i * 2, 2)));

        Update(args);
    }

    public void Update(BluetoothLEAdvertisementReceivedEventArgs args)
    {
        LastSeen = args.Timestamp;
        IsConnectable = args.IsConnectable;

        if (!string.IsNullOrEmpty(args.Advertisement.LocalName))
        {
            Name = args.Advertisement.LocalName;
        }

        var newIds = new HashSet<Guid>(ServiceUuids);
        newIds.UnionWith(args.Advertisement.ServiceUuids);

        ServiceUuids.Clear();
        foreach (var id in newIds)
        {
            ServiceUuids.Add(id);
        }

        OnPropertyChanged(nameof(ServiceUuidsCount));
    }
}
