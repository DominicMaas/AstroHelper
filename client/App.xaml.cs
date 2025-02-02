// Ignore Spelling: App

using Microsoft.UI.Xaml;
using WinUIEx;

namespace AstroClient;

public partial class App : Application
{
    public WindowEx? Window { get; private set; }

    public static new App Current => (App)Application.Current;

    public App()
    {
        InitializeComponent();
    }

    protected override void OnLaunched(LaunchActivatedEventArgs args)
    {
        Window = new MainWindow();
        Window.Activate();
    }
}
