using AstroClient.ViewModels;
using Microsoft.UI.Xaml.Controls;

namespace AstroClient;

public sealed partial class RootView : UserControl
{
    public RootViewModel ViewModel => (RootViewModel)DataContext;

    public RootView()
    {
        InitializeComponent();
        DataContext = new RootViewModel();
    }

    private void ItemsView_ItemInvoked(ItemsView sender, ItemsViewItemInvokedEventArgs args)
    {
        ViewModel.ConnectToDeviceCommand.Execute(args.InvokedItem);
    }
}
