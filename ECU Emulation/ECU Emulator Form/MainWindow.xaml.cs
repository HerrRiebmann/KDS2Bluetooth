using System;
using System.Threading;
using System.Windows;
using System.Windows.Threading;
using Backend.Data;
using EcuEmulator;
using EcuEmulator.Data;
using SerialComPort;
using SerialComPort.Data;

namespace EcuEmulatorForm
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow
    {
        private EcuSetup _ecuSetup;
        private ComSetup _comSetup;
        private Serial _serialPort;
        private const Serializer.ConversionTypes FileType = Serializer.ConversionTypes.Json;
        private volatile string _dataReceived;

        public MainWindow()
        {
            InitializeComponent();
            InitializeData();
            InitializeComPort();
            
            gridStatus.DataContext = Emulator.Status;
        }

        private void InitializeComPort()
        {
            _dataReceived = String.Empty;
            _serialPort = new Serial {Setup = _comSetup};
            _serialPort.DataReceived += _serialPort_DataReceived;
            //_serialPort.InfoReceived += _serialPort_InfoReceived;
            cbComPort.ItemsSource = SerialComPort.Helper.SetupValues.GetPortNameValues();
            cbComPort.SelectedItem = _serialPort.Setup.PortName;
        }

        void _serialPort_InfoReceived(object sender, SerialComPort.Events.InfoEventArgs e)
        {
            switch (e.MessageType)
            {
                case Serial.MessageTypes.Incoming:
                    break;
                case Serial.MessageTypes.Normal:
                    break;
                case Serial.MessageTypes.Outgoing:
                    break;
                case Serial.MessageTypes.Warning:
                    break;
                case Serial.MessageTypes.Error:
                    break;
            }
        }

        void _serialPort_DataReceived(object sender, SerialComPort.Events.DataEventArgs e)
        {
            //Only Receive incoming Messages
            //All other messages are received in MessageReceived()-Event
            _dataReceived += String.IsNullOrEmpty(_dataReceived) ? e.Data : " " + e.Data;
            var result = Emulator.HandleRequest(_dataReceived);
            switch (result.Type)
            {
                case Emulator.ResponseType.Empty:
                case Emulator.ResponseType.Incomplete:
                    //Nothing
                    break;
                case Emulator.ResponseType.ChecksumNotOk:
                case Emulator.ResponseType.Error:
                case Emulator.ResponseType.RegisterUnknown:
                    //Respond Error
                case Emulator.ResponseType.Ok:
                    _serialPort.Send(result.Value);
                    _dataReceived = string.Empty;
                    break;
                case Emulator.ResponseType.NoCommonStartChar:
                //Delete Overhead Data from Buffer, when containing no Start-char
                case Emulator.ResponseType.WrongReceiver:
                    //Ignore
                    _dataReceived = string.Empty;
                    break;
            }
        }

        private void InitializeData()
        {
            _ecuSetup = EcuSetup.Load(FileType);
            _comSetup = ComSetup.Load(FileType);
            var lastPortName = string.Empty;
            var portFound = false;
            foreach (var portName in SerialComPort.Helper.SetupValues.GetPortNameValues())
            {
                if (_comSetup.PortName.Equals(portName))
                {
                    portFound = true;
                    break;
                }
                lastPortName = portName;
            }
            if (!portFound)
                _comSetup.PortName = lastPortName;
            Emulator.Setup = _ecuSetup;
            Emulator.Register = RegisterList.Load(FileType);
            if (Emulator.Register.Items.Count <= 0)
                Emulator.Register = Emulator.InitRegister();
        }

        private void StoreData()
        {
            _ecuSetup.Save(FileType);
            _comSetup.Save(FileType);
            Emulator.Register.Save(FileType);
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            if (_serialPort.IsOpen)
                _serialPort.Close();
            StoreData();
        }

        private void BtnComPort_Click(object sender, RoutedEventArgs e)
        {
            if (_serialPort.IsOpen)
            {
                BtnComPort.Content = "Connect";
                _serialPort.Close();
                BtnEdit.IsEnabled = true;
            }
            else
            {
                _serialPort.Setup.PortName = (string)cbComPort.SelectedItem;
                BtnComPort.Content = "Disconnect";
                _serialPort.Open();
                BtnEdit.IsEnabled = false;
            }
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            
        }

        private void BtnMessages_Click(object sender, RoutedEventArgs e)
        {
            //MessageWindow msgWin = new MessageWindow();
            //msgWin.Show();
            BtnMessages.IsEnabled = false;
            // Create a thread
            var newWindowThread = new Thread(() =>
            {
                // Create and show the Window
                var tempWindow = new MessageWindow();
                tempWindow.Closed += (s, evt) =>
                    Dispatcher.CurrentDispatcher.BeginInvokeShutdown(DispatcherPriority.Background);
                tempWindow.Closing += (o, args) => Application.Current.Dispatcher.BeginInvoke((ThreadStart)delegate
                {
                    BtnMessages.IsEnabled = true;
                });
                tempWindow.Show();
                // Start the Dispatcher Processing
                Dispatcher.Run();
            });
            // Set the apartment state
            newWindowThread.SetApartmentState(ApartmentState.STA);
            // Make the thread a background thread
            newWindowThread.IsBackground = true;
            // Start the thread
            newWindowThread.Start();
        }

        private void BtnEdit_Click(object sender, RoutedEventArgs e)
        {
            var setupWin = new SetupWindow(_comSetup, _ecuSetup);
            setupWin.ShowDialog();
            _comSetup = setupWin.ComSetup;
            _ecuSetup = setupWin.EcuSetup;
            cbComPort.SelectedItem = _serialPort.Setup.PortName;
        }

        private void BtnRegister_Click(object sender, RoutedEventArgs e)
        {
            var registerWin = new RegisterWindow();
            registerWin.ShowDialog();
        }
    }
}
