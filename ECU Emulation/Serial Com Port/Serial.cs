using System;
using System.IO.Ports;
using Backend.Helper;
using SerialComPort.Data;
using SerialComPort.Events;
using Converter = SerialComPort.Helper.Converter;

namespace SerialComPort
{
    public class Serial : IDisposable
    {
        #region Manager Enums
        /// <summary>
        /// enumeration to hold our transmission types
        /// </summary>
        public enum TransmissionTypes { Text, Hex }

        /// <summary>
        /// enumeration to hold our message types
        /// </summary>
        public enum MessageTypes { Incoming, Outgoing, Normal, Warning, Error };
        #endregion

        private readonly SerialPort _comPort;
        private ComSetup _setup;
        public ComSetup Setup
        {
            get { return _setup; }
            set { _setup = value; }
        }

        private TransmissionTypes _transmissionType = TransmissionTypes.Hex;
        public TransmissionTypes TransmissionType
        {
            get { return _transmissionType; }
            set { _transmissionType = value; }
        }

        private Exception _lastException = new Exception();
        public Exception LastException
        {
            get { return _lastException; }
            set { _lastException = value; }
        }

        public bool IsOpen
        {
            get
            {
                return _comPort != null && _comPort.IsOpen;
            }
        }
        
        /// <summary>
        /// Only incoming Data to receive
        /// </summary>
        public event EventHandler<DataEventArgs> DataReceived = delegate { };

        /// <summary>
        /// All Info around Serial Port.
        /// For Debug purpose, only!
        /// </summary>
        public event EventHandler<InfoEventArgs> InfoReceived = delegate { };
        //public delegate void InfoHandler(InfoEventArgs e);
        //public event InfoHandler = delegate { };
        
        public Serial()
        {
            _comPort = new SerialPort();
            _setup = new ComSetup();
            _comPort.DataReceived += _comPort_DataReceived;
            _comPort.Disposed += _comPort_Disposed;
            _comPort.ErrorReceived += _comPort_ErrorReceived;
            _comPort.PinChanged += _comPort_PinChanged;
        }

        public bool Open()
        {
            _comPort.BaudRate = Converter.BaudRate(_setup.BaudRate);
            _comPort.Parity = Converter.Parity(_setup.Parity);
            _comPort.StopBits = Converter.StopBits(_setup.StopBits);
            _comPort.DataBits = Converter.DataBits(_setup.DataBits);
            _comPort.PortName = _setup.PortName;
            _comPort.Handshake = Converter.Handshake(_setup.Handshake);
            _comPort.Encoding = Converter.Encode(_setup.Encoding);
            _comPort.RtsEnable = _setup.RtsEnable;
            if (_setup.TimeOutMs > 0)
            {
                _comPort.ReadTimeout = _setup.TimeOutMs;
                _comPort.WriteTimeout = _setup.TimeOutMs;
            }
            _comPort.DtrEnable = _setup.DtrEnable;

            try
            {
                _comPort.Open();
                _comPort.DiscardOutBuffer();
                _comPort.DiscardInBuffer();
                DisplayData(MessageTypes.Normal, "ComPort Opened");
                return true;
            }
            catch (Exception ex)
            {
                _lastException = ex;
                DisplayData(MessageTypes.Error, ex.Message);
            }
            return false;
        }

        public bool Open(ComSetup setup)
        {
            _setup = setup;
            return Open();
        }

        public bool Close()
        {
            if(_comPort.IsOpen)
                try
                {
                    _comPort.DiscardOutBuffer();
                    _comPort.DiscardInBuffer();
                    _comPort.Close();
                    DisplayData(MessageTypes.Normal, "Com Port Closed");
                    return true;
                }
                catch (Exception ex)
                {
                    _lastException = ex;
                    DisplayData(MessageTypes.Error, ex.Message);
                }
            else
                DisplayData(MessageTypes.Error, "ComPort already closed");
            return false;
        }

        public bool Send(string command)
        {
            if (_comPort.IsOpen)
                try
                {
                    //Convert to HEX:
                    var msg = Backend.Helper.Converter.HexToByte(command);
                    _comPort.Write(msg, 0, msg.Length);
                    DisplayData(MessageTypes.Outgoing, command);
                    return true;
                }
                catch (Exception ex)
                {
                    _lastException = ex;
                    DisplayData(MessageTypes.Error, ex.Message);
                }
            else
                DisplayData(MessageTypes.Error, "ComPort not open!");
            
            return false;
        }

        public bool SendLine(string command)
        {
            if (_comPort.IsOpen)
                try
                {
                    //Convert to HEX:
                    var msg = Backend.Helper.Converter.HexToByte(command + " 0D 0A"); //Environment.NewLine
                    _comPort.Write(msg, 0, msg.Length);
                    DisplayData(MessageTypes.Outgoing, command);
                    return true;
                }
                catch (Exception ex)
                {
                    _lastException = ex;
                    DisplayData(MessageTypes.Error, ex.Message);
                }
            else
                DisplayData(MessageTypes.Error, "ComPort not open!");
            return false;
        }

        public bool Write(string command)
        {
            if (_comPort.IsOpen)
                try
                {                    
                    _comPort.Write(command);
                    DisplayData(MessageTypes.Outgoing, command);
                    return true;
                }
                catch (Exception ex)
                {
                    _lastException = ex;
                    DisplayData(MessageTypes.Error, ex.Message);
                }
            else
                DisplayData(MessageTypes.Error, "ComPort not open!");

            return false;
        }

        #region ComPort Events
        void _comPort_PinChanged(object sender, SerialPinChangedEventArgs e)
        {
            DisplayData(MessageTypes.Warning, "Com Port Pin Change: " + e.EventType);
        }

        void _comPort_ErrorReceived(object sender, SerialErrorReceivedEventArgs e)
        {
            DisplayData(MessageTypes.Error, "Com Port Error: " + e.EventType);
        }

        void _comPort_Disposed(object sender, EventArgs e)
        {
            DisplayData(MessageTypes.Normal, "Com Port Disposed");
        }

        void _comPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            var bytes = _comPort.BytesToRead;
            //create a byte array to hold the awaiting data
            var comBuffer = new byte[bytes];
            //read the data and store it
            //_comPort.Read(comBuffer, 0, bytes);
            _comPort.Read(comBuffer, 0, bytes);

            if (bytes == 0)
                return;

            //0x44 == D (Debuger)
            //if (comBuffer[0].Equals(0x44))
            //{
            //    char[] delimiters = new char[] { '\r', '\n' };
            //    string result = System.Text.Encoding.UTF8.GetString(comBuffer);
            //    foreach (var part in result.Split(delimiters, StringSplitOptions.RemoveEmptyEntries))
            //    {
            //        if (part.StartsWith("D"))
            //        {
            //            var str = part.Remove(0, 1);
            //            Messaging.WriteLine(str, Messaging.Types.Debug, Messaging.Caller.ComPort);
            //            if (result.Contains(Environment.NewLine))
            //                result = result.Remove(0, result.IndexOf('\n')+1);
            //            else
            //                result = string.Empty;
            //        }

            //        if (String.IsNullOrEmpty(result))
            //            return;
            //        else
            //            comBuffer = System.Text.Encoding.UTF8.GetBytes(result);
            //    }
            //}
            
            //display the data to the user
            var msg = Backend.Helper.Converter.ByteToHex(comBuffer);

            DisplayData(MessageTypes.Incoming, msg);
            //DisplayData(MessageTypes.Incoming, _comPort.ReadExisting());
        }
        #endregion

        /// <summary>
        /// method to display the data to & from the port
        /// on the screen
        /// </summary>
        /// <param name="type">MessageType of the message</param>
        /// <param name="data">Data to display</param>
        [STAThread]
        private void DisplayData(MessageTypes type, string data)
        {
            if (String.IsNullOrEmpty(data))
                return;
            Messaging.WriteLine(data, (Messaging.Types)type,Messaging.Caller.ComPort);
            
            if (InfoReceived != null)
                InfoReceived(this, new InfoEventArgs(type, data));
            if (type != MessageTypes.Incoming)
                return;
            if (DataReceived != null)
                DataReceived(this, new DataEventArgs(data));
        }

        public void Dispose()
        {
            _comPort.DataReceived -= _comPort_DataReceived;
            _comPort.Disposed -= _comPort_Disposed;
            _comPort.ErrorReceived -= _comPort_ErrorReceived;
            _comPort.PinChanged -= _comPort_PinChanged;
            if(_comPort.IsOpen)
                _comPort.Close();
            _comPort.Dispose();
        }
    }
}
