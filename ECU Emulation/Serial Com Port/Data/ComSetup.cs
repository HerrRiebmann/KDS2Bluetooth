using Backend.Data;

namespace SerialComPort.Data
{
    public class ComSetup : Serializer<ComSetup>
    {
        private string _baudRate = "10400";
        public string BaudRate
        {
            get { return _baudRate; }
            set { _baudRate = value; }
        }

        private string _parity = "None";
        public string Parity
        {
            get { return _parity; }
            set { _parity = value; }
        }

        private string _stopBits = "One";
        public string StopBits
        {
            get { return _stopBits; }
            set { _stopBits = value; }
        }

        private string _dataBits = "8";
        public string DataBits
        {
            get { return _dataBits; }
            set { _dataBits = value; }
        }

        private string _portName = string.Empty;
        public string PortName
        {
            get { return _portName; }
            set { _portName = value; }
        }

        private string _handshake = "None";
        public string Handshake
        {
            get { return _handshake; }
            set { _handshake = value; }
        }

        private string _encoding = "ASCII";
        public string Encoding
        {
            get { return _encoding; }
            set { _encoding = value; }
        }

        private bool _rtsEnable = true;
        public bool RtsEnable
        {
            get { return _rtsEnable; }
            set { _rtsEnable = value; }
        }

        private int _timeOutMs;
        public int TimeOutMs
        {
            get { return _timeOutMs; }
            set { _timeOutMs = value; }
        }

        private bool _dtrEnable;
        public bool DtrEnable
        {
            get { return _dtrEnable; }
            set { _dtrEnable = value; }
        }
        
    }
}
