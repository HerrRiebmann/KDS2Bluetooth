using System.Windows;
using EcuEmulator;
using EcuEmulator.Data;

namespace EcuEmulatorForm
{
    /// <summary>
    /// Interaction logic for RegisterWindow.xaml
    /// </summary>
    public partial class RegisterWindow
    {
        public RegisterWindow()
        {
            InitializeComponent();
        }
        
        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            var registerListViewSource = ((System.Windows.Data.CollectionViewSource)(this.FindResource("registerListViewSource")));
            // Load data by setting the CollectionViewSource.Source property:
            registerListViewSource.Source = Emulator.Register.Items;
        }

        private void BtnNew_Click(object sender, RoutedEventArgs e)
        {
            Emulator.Register.Items.Add(new Register());
        }

        private void BtnDelete_Click(object sender, RoutedEventArgs e)
        {
            var row = itemsListView.SelectedItem;
            if (row == null)
                return;
            var register = (Register) row;
            if(MessageBox.Show("Do you really want to delete \"" + register.Description + "\"?", "Delete Row", MessageBoxButton.YesNo, MessageBoxImage.Question, MessageBoxResult.No) == MessageBoxResult.Yes)
                Emulator.Register.Items.Remove(register);
        }
    }
}
