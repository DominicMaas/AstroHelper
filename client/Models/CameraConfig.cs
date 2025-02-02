using CommunityToolkit.Mvvm.ComponentModel;
using System.Collections.ObjectModel;

namespace AstroClient.Models;

public partial class CameraConfig : ObservableObject
{
    [ObservableProperty]
    public partial string Id { get; set; }

    [ObservableProperty]
    public partial string DisplayName { get; set; }

    [ObservableProperty]
    public partial string Value { get; set; }

    [ObservableProperty]
    public partial ObservableCollection<string> Choices { get; set; } = [];

    [ObservableProperty]
    [NotifyPropertyChangedFor(nameof(CanWrite))]
    public partial bool IsReadOnly { get; set; }

    public bool CanWrite => !IsReadOnly;
}
