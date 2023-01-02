using System;
using System.Windows;
using EcuEmulator.Data;
using SerialComPort.Data;

namespace EcuEmulatorForm
{
    /// <summary>
    /// Interaction logic for SetupWindow.xaml
    /// </summary>
    public partial class SetupWindow
    {
        private ComSetup _comSetup;
        public ComSetup ComSetup
        {
            get { return _comSetup; }
            set { _comSetup = value; }
        }

        private EcuSetup _ecuSetup;
        public EcuSetup EcuSetup
        {
            get { return _ecuSetup; }
            set { _ecuSetup = value; }
        }

        public SetupWindow(ComSetup comSetup, EcuSetup ecuSetup)
        {
            ComSetup = comSetup;
            EcuSetup = ecuSetup;
            InitializeComponent();

            InitializeComPort();
            InititalizeEcuSetup();
        }

        private void InitializeComPort()
        {
            cbComPort.ItemsSource = SerialComPort.Helper.SetupValues.GetPortNameValues();
            baudRateComboBox.ItemsSource = SerialComPort.Helper.SetupValues.GetBaudrateValues();
            dataBitsComboBox.ItemsSource = SerialComPort.Helper.SetupValues.GetDataBits();
            encodingComboBox.ItemsSource = SerialComPort.Helper.SetupValues.GetEncoding();
            handshakeComboBox.ItemsSource = SerialComPort.Helper.SetupValues.GetHandshakes();
            parityComboBox.ItemsSource = SerialComPort.Helper.SetupValues.GetParityValues();
            stopBitsComboBox.ItemsSource = SerialComPort.Helper.SetupValues.GetStopBitValues();

            cbComPort.SelectedItem = _comSetup.PortName;
            baudRateComboBox.SelectedItem = _comSetup.BaudRate;
            dataBitsComboBox.SelectedItem = _comSetup.DataBits;
            encodingComboBox.SelectedItem = _comSetup.Encoding;
            handshakeComboBox.SelectedItem = _comSetup.Handshake;
            parityComboBox.SelectedItem = _comSetup.Parity;
            stopBitsComboBox.SelectedItem = _comSetup.StopBits;
        }

        private void InititalizeEcuSetup()
        {
            bikeComboBox.ItemsSource = Enum.GetValues(typeof(Bikes));
            bikeComboBox.SelectedItem = _ecuSetup.Bike;
        }
        
        private void Window_Loaded(object sender, RoutedEventArgs e)
        {

            //System.Windows.Data.CollectionViewSource comSetupViewSource = ((System.Windows.Data.CollectionViewSource)(this.FindResource("comSetupViewSource")));
            // Load data by setting the CollectionViewSource.Source property:
            //comSetupViewSource.Source = ComSetup;
            gridComPort.DataContext = ComSetup;
            //System.Windows.Data.CollectionViewSource ecuSetupViewSource = ((System.Windows.Data.CollectionViewSource)(this.FindResource("ecuSetupViewSource")));
            // Load data by setting the CollectionViewSource.Source property:
            //ecuSetupViewSource.Source = EcuSetup;
            gridEcuSetup.DataContext = EcuSetup;
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
        }
    }
}
