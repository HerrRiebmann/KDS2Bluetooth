using System;
using System.Xml.Serialization;
using Backend.Data;

namespace EcuEmulator.Data
{
    [Serializable]
    public class EcuSetup : Serializer<EcuSetup>
    {
        private int _isoRequestDelay = 10;
        public int IsoRequestDelay
        {
            get { return _isoRequestDelay; }
            set { _isoRequestDelay = value; }
        }

        private byte _ecuAdress = 0x11;
        public byte EcuAdress
        {
            get { return _ecuAdress; }
            set { _ecuAdress = value; }
        }

        private byte _senderAdress = 0xF1;
        public byte SenderAdress
        {
            get { return _senderAdress; }
            set { _senderAdress = value; }
        }

        private byte _positiveReply = 0x61;
        public byte PositiveReply
        {
            get { return _positiveReply; }
            set { _positiveReply = value; }
        }

        private byte[] _errorSequence = {0x7F, 0x21, 0x10};
        public byte[] ErrorSequence
        {
            get { return _errorSequence; }
            set { _errorSequence = value; }
        }

        private byte[] _initSequence = {0xC1, 0xEA, 0x8F};
        public byte[] InitSequence
        {
            get { return _initSequence; }
            set { _initSequence = value; }
        }

        private int _offsetMax = 216;
        public int OffsetMax
        {
            get { return _offsetMax; }
            set { _offsetMax = value; }
        }

        private int _offsetMin = 117;
        public int OffsetMin
        {
            get { return _offsetMin; }
            set { _offsetMin = value; }
        }

        private int _timeOutMilis = 2000;
        public int TimeOutMilis
        {
            get { return _timeOutMilis; }
            set {
                if (_timeOutMilis != value)
                {
                    _timeOutMilis = value;
                    Notify("TimeOutMilis");
                } }
        }

        #region Internal Fields
        [NonSerialized]
        private bool _ecuInitialized;
        [XmlIgnore]
        public bool EcuInitialized
        {
            get { return _ecuInitialized; }
            internal set { _ecuInitialized = value; }
        }

        [NonSerialized]
        private bool _diagnosticMode;
        [XmlIgnore]
        public bool DiagnosticsMode
        {
            get { return _diagnosticMode; }
            internal set
            {
                _diagnosticMode = value;
                if (_diagnosticMode)
                    _lastRequest = DateTime.Now;
            }
        }

        [NonSerialized]
        private DateTime _lastRequest;
        [XmlIgnore]
        public DateTime LastRequest
        {
            get { return _lastRequest; }
            internal set { _lastRequest = value; }
        }
        #endregion
    }
}
