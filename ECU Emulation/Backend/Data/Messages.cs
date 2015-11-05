using System;
using Backend.Helper;

namespace Backend.Data
{
    public class Message : Serializer<Message>
    {
        private string _data = string.Empty;
        public string Data
        {
            get { return _data; }
            set {
                if (_data != value)
                {
                    _data = value;
                    Notify("Data");
                } }
        }

        private Messaging.Types _type = Messaging.Types.Normal;
        public Messaging.Types Type
        {
            get { return _type; }
            set {
                if (_type != value)
                {
                    _type = value;
                    Notify("Type");
                } }
        }

        private DateTime _received = DateTime.MinValue;
        public DateTime Received
        {
            get { return _received; }
            set {
                if (_received != value)
                {
                    _received = value;
                    Notify("Received");
                } }
        }

        private string _convertedValue = string.Empty;
        public string ConvertedValue
        {
            get { return _convertedValue; }
            set {
                if (_convertedValue != value)
                {
                    _convertedValue = value;
                    Notify("ConvertedValue");
                } }
        }

        private Messaging.Caller _caller = Messaging.Caller.Unknown;
        public Messaging.Caller Caller
        {
            get { return _caller; }
            set {
                if (_caller != value)
                {
                    _caller = value;
                    Notify("Caller");
                } }
        }

        public Message()
        {
            
        }

        public Message(string data, Messaging.Types type)
        {
            _type = type;
            _received = DateTime.Now;
            Data = data;
        }

        public Message(string data, Messaging.Types type, Messaging.Caller caller)
        {
            _type = type;
            _received = DateTime.Now;
            _caller = caller;
            Data = data;
        }

        public Message(string data, Messaging.Types type, Messaging.Caller caller, string convertedValue)
        {
            _type = type;
            _received = DateTime.Now;
            _caller = caller;
            _convertedValue = convertedValue;
            Data = data;
        }
    }

    public class Messages : Serializer<Messages>
    {
        private MultiThreadObservableCollection<Message> _item = new MultiThreadObservableCollection<Message>();

        public MultiThreadObservableCollection<Message> Item
        {
            get { return _item; }
            set { _item = value; }
        }
    }
}
