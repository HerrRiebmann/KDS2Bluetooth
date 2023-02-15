using Backend.Data;

namespace EcuEmulator.Data
{
    public class Status : Serializer<Status>
    {
        private bool _init;
        public bool Init
        {
            get { return _init; }
            set {
                if (_init != value)
                {
                    _init = value;
                    Notify("Init");
                } }
        }

        private bool _diagnostic;
        public bool Diagnostic
        {
            get { return _diagnostic; }
            set {
                if (_diagnostic != value)
                {
                    _diagnostic = value;
                    Notify("Diagnostic");
                } }
        }

        private string _currentRequest = string.Empty;
        public string CurrentRequest
        {
            get { return _currentRequest; }
            set {
                if (_currentRequest != value)
                {
                    _currentRequest = value;
                    Notify("CurrentRequest");
                } }
        }

        private string _requestType = string.Empty;
        public string RequestType
        {
            get { return _requestType; }
            set {
                if (_requestType != value)
                {
                    _requestType = value;
                    Notify("RequestType");
                } }
        }

        private string _lastResponse = string.Empty;
        public string LastResponse
        {
            get { return _lastResponse; }
            set { if(_lastResponse != value)
                {
                    _lastResponse = value;
                    Notify("LastResponse");
                } }
        }

        private string _convertedResponse = string.Empty;
        public string ConvertedResponse
        {
            get { return _convertedResponse; }
            set {
                if (_convertedResponse != value)
                {
                    _convertedResponse = value;
                    Notify("ConvertedResponse");
                } }
        }

        private string _lastErrorMessage = string.Empty;
        public string LastErrorMessage
        {
            get { return _lastErrorMessage; }
            set {
                if (_lastErrorMessage != value)
                {
                    _lastErrorMessage = value;
                    Notify("LastErrorMessage");
                } }
        }

        private int _messagesPerSecond;
        public int MessagesPerSecond
        {
            get { return _messagesPerSecond; }
            set {
                if(_messagesPerSecond != value)
                {
                    _messagesPerSecond = value;
                    Notify("MessagesPerSecond");
                }
            }
        }

        private bool _checksum;
        public bool Checksum
        {
            get { return _checksum; }
            set {
                if (_checksum != value)
                {
                    _checksum = value;
                    Notify("Checksum");
                } }
        }
    }
}
