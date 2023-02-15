using System;
using System.Collections.Generic;
using System.Linq;
using EcuEmulator.Translation;

namespace EcuEmulator.Data
{
    public class DataEntry : Backend.Data.Serializer<DataEntry>
    {
        public DataEntry()
        {
            
        }

        public DataEntry(string hexString)
        {
            HexString = hexString;
        }

        public DataEntry(int msgNo, string hexString, string timestamp)
        {
            MsgNo = msgNo;
            HexString = hexString;
            if (!Int32.TryParse(timestamp, out _timestamp))
            {
                _timestamp = 0;
                _timestampString = timestamp;
            }
        }

        public DataEntry(int msgNo, string hexString)
        {
            MsgNo = msgNo;
            HexString = hexString;
        }

        public int _msgNo = 0;

        private string _hexString = string.Empty;

        private string _hexMessage = string.Empty;

        private IEnumerable<byte> _hexValues;

        private string _hexData = string.Empty;

        private string _preText = string.Empty;

        private string _postText = string.Empty;

        private string _preHex = string.Empty;

        private string _postHex = string.Empty;

        private string _msgFormat = string.Empty;

        private string _msgSender = string.Empty;

        private string _msgReceiver = string.Empty;

        private string _msgSid = string.Empty;

        private string _msgPid = string.Empty;

        private string _msgSidInverted = string.Empty;

        private int _msgLength = 0;

        private string _msgLengthHex = string.Empty;

        private string _msgData = string.Empty;
        
        private string _msgChecksum = string.Empty;

        private string _msgText = string.Empty;

        private IEnumerable<byte> _msgValues;

        private UInt64 _msgDataInt = 0;

        private string _msgDataBinary = string.Empty;

        private Int32 _timestamp = 0;

        private List<CalculationResult> _calculationResultList;

        public int MsgNo
        {
            get { return _msgNo; }
            set {
                if (_msgNo != value)
                {
                    _msgNo = value;
                    Notify("MsgNo");
                } }
        }

        public string HexString
        {
            get { return _hexString; }
            set
            {
                if (_hexString != value)
                {                    
                    _hexString = value;
                    Notify("HexString");
                    //HexValues = HexCalculator.ConvertToBytes(HexMessage);
                    var cleanHexStr = CleanupHexString(_hexString);
                    HexValues = HexCalculator.ConvertToBytes(cleanHexStr);
                    PreHex = HexCalculator.GetPreText(_hexString);
                    PostHex = HexCalculator.GetPostText(_hexString);
                    //HexMessage = HexCalculator.GetMessageText(_hexString);
                    HexMessage = HexCalculator.GetMessageText(cleanHexStr);
                }
            }
        }

        public string HexMessage
        {
            get { return _hexMessage; }
            set {
                if (_hexMessage != value)
                {
                    _hexMessage = value;
                    Notify("HexMessage");
                    if (HexCalculator.ValidateHexText(_hexMessage))
                    {
                        MsgValues = HexCalculator.ConvertToBytes(_hexMessage);
                    }
                }
            }
        }

        public IEnumerable<byte> HexValues
        {
            get { return _hexValues; }
            set
            {
                if (_hexValues != value)
                {
                    _hexValues = value;
                    Notify("HexValues");
                }
            }
        }

        public string HexData
        {
            get { return _hexData; }
            set {
                if (_hexData != value)
                {
                    _hexData = value;
                    Notify("HexData");
                    if (!string.IsNullOrEmpty(_hexData))
                    {
                        MsgDataInt = HexCalculator.ConvertToInteger(_hexData);
                        if (_hexData != null)
                            MsgDataBinary = HexCalculator.ConvertToBinaryString(HexCalculator.ConvertToBytes(_hexData));
                    }
                } }
        }

        public string PreText
        {
            get { return _preText; }
            set {
                if (_preText != value)
                {
                    _preText = value;
                    Notify("PreText");
                } }
        }

        public string PostHex
        {
            get { return _postHex; }
            set {
                if (_postHex != value)
                {
                    _postHex = value;
                    Notify("PostHex");
                    PostText = HexCalculator.ConvertToText(PostHex);
                } }
        }

        public string PreHex
        {
            get { return _preHex; }
            set
            {
                if (_preHex != value)
                {
                    _preHex = value;
                    Notify("PreHex");
                    PreText = HexCalculator.ConvertToText(PreHex);
                }
            }
        }

        public string PostText
        {
            get { return _postText; }
            set
            {
                if (_postText != value)
                {
                    _postText = value;
                    Notify("PostText");
                }
            }
        }

        public string MsgFormat
        {
            get { return _msgFormat; }
            set {
                if (_msgFormat != value)
                {
                    _msgFormat = value;
                    Notify("MsgFormat");
                } }
        }

        public string MsgSender
        {
            get { return _msgSender; }
            set {
                if (_msgSender != value)
                {
                    _msgSender = value;
                    Notify("MsgSender");
                } }
        }

        public string MsgReceiver
        {
            get { return _msgReceiver; }
            set {
                if (_msgReceiver != value)
                {
                    _msgReceiver = value;
                    Notify("MsgReceiver");
                } }
        }

        public string MsgSid
        {
            get { return _msgSid; }
            set {
                if (_msgSid != value)
                {
                    _msgSid = value;
                    Notify("MsgSid");
                    MsgSidInverted = MsgSender.Equals("F1")
                        ? HexCalculator.ConvertToHexString((byte) (HexCalculator.ConvertToByte(MsgSid) + 0x40))
                        : HexCalculator.ConvertToHexString((byte) (HexCalculator.ConvertToByte(MsgSid) - 0x40));
                    
                } }
        }

        public string MsgPid
        {
            get { return _msgPid; }
            set {
                if (_msgPid != value)
                {
                    _msgPid = value;
                    Notify("MsgPid");
                } }
        }

        public string MsgSidInverted
        {
            get { return _msgSidInverted; }
            set {
                if (_msgSidInverted != value)
                {
                    _msgSidInverted = value;
                    Notify();
                } }
        }

        public int MsgLength
        {
            get { return _msgLength; }
            set
            {
                if (_msgLength != value)
                {
                    _msgLength = value;
                    Notify("MsgLength");
                    MsgLengthHex = HexCalculator.ConvertToHexString(new[] {(byte) MsgLength});
                }
            }
        }

        public string MsgLengthHex
        {
            get { return _msgLengthHex; }
            set {
                if (_msgLengthHex != value)
                {
                    _msgLengthHex = value;
                    Notify();
                } }
        }

        public string MsgData
        {
            get { return _msgData; }
            set {
                if (_msgData != value)
                {
                    _msgData = value;
                    Notify("MsgData");
                } }
        }

        public string MsgChecksum
        {
            get { return _msgChecksum; }
            set {
                if (_msgChecksum != value)
                {
                    _msgChecksum = value;
                    Notify("MsgChecksum");
                } }
        }

        public string MsgText
        {
            get { return _msgText; }
            set {
                if (_msgText != value)
                {
                    _msgText = value;
                    Notify("MsgText");
                } }
        }

        public IEnumerable<byte> MsgValues
        {
            get { return _msgValues; }
            set
            {
                if (_msgValues != value)
                {
                    _msgValues = value;
                    Notify("MsgValues");
                    if (MsgValues.Any())
                    {
                        MsgFormat = HexCalculator.ConvertToHexString(MsgValues.ElementAt(0));
                        if (!MsgValues.ElementAt(0).Equals(HexCalculator.FormatBytes[3]) && MsgValues.Count() > 2)
                        {
                            MsgReceiver = HexCalculator.ConvertToHexString(MsgValues.ElementAt(1));
                            MsgSender = HexCalculator.ConvertToHexString(MsgValues.ElementAt(2));
                        }

                        MsgData = HexCalculator.ConvertToHexString(HexCalculator.GetMessageData(MsgValues));
                        //Message without length information
                        if(!MsgValues.ElementAt(0).Equals(HexCalculator.FormatBytes[1]))
                            MsgLength = HexCalculator.GetMessageLength(MsgValues);
                        if (MsgLength >= 2 && MsgValues.Count() > 5)
                        {
                            MsgSid = HexCalculator.ConvertToHexString(MsgValues.ElementAt(4));
                            MsgPid = HexCalculator.ConvertToHexString(MsgValues.ElementAt(5));
                        }
                        else
                            if(MsgLength == 1 && MsgValues.Count() > 4)
                                MsgSid = HexCalculator.ConvertToHexString(MsgValues.ElementAt(4));
                        MsgChecksum = HexCalculator.ConvertToHexString(HexCalculator.GetMessageChecksum(MsgValues));
                        HexData = HexCalculator.ConvertToHexString(HexCalculator.GetMessageData(MsgValues));
                        MsgText = HexCalculator.ConvertToText(HexCalculator.GetMessageData(MsgValues));
                    }
                }
            }
        }

        public ulong MsgDataInt
        {
            get { return _msgDataInt; }
            set {
                if (_msgDataInt != value)
                {
                    _msgDataInt = value;
                    Notify("MsgDataInt");
                } }
        }

        public string MsgDataBinary
        {
            get { return _msgDataBinary; }
            set {
                if (_msgDataBinary != value)
                {
                    _msgDataBinary = value;
                    Notify("MsgDataBinary");
                } }
        }

        public string Text
        {
            get { return $"{PreText} {MsgText} {PostText}"; }
        }

        public List<CalculationResult> GetResults => _calculationResultList ?? (_calculationResultList = Translate.GetResults(this));

        public int Timestamp { get => _timestamp;
            set { if (_timestamp != value)
                {
                    _timestamp = value;
                    if(PostText == string.Empty)
                        PostText = TimestampString;
                    _timestampString = string.Empty;
                }
            } }

        private string _timestampString = string.Empty;
        public string TimestampString
        {
            get {
                if (_timestamp == 0)
                    return _timestampString;
                TimeSpan t = TimeSpan.FromMilliseconds(_timestamp);
                return string.Format("{0:D2}:{1:D2}:{2:D2}.{3:D3}",
                                        t.Hours,
                                        t.Minutes,
                                        t.Seconds,
                                        t.Milliseconds);
            }
            set
            {
                if (_timestampString != value)
                {
                    _timestampString = value;
                    if (PostText == string.Empty)
                        PostText = TimestampString;
                    _timestamp = 0;
                }
            }
        }

        private static string CleanupHexString(string hexString)
        {
            const char seperator = ' ';
            string newHexString = hexString;

            if (newHexString.StartsWith(seperator.ToString()))
                newHexString = newHexString.Substring(1, newHexString.Length - 1);

            //Corrupt logger data: BT<, >
            if(newHexString.Contains("BT"))
                newHexString = newHexString.Substring(0, newHexString.IndexOf("BT"));
            if (newHexString.Contains('<'))
                newHexString = newHexString.Substring(0, newHexString.IndexOf('<'));
            if (newHexString.Contains('>'))
                newHexString = newHexString.Substring(0, newHexString.IndexOf('>'));

            if (!hexString.Contains(seperator))
                newHexString = System.Text.RegularExpressions.Regex.Replace(hexString, ".{2}", "$0 ");
            
            if (newHexString.EndsWith(seperator.ToString()))
                newHexString = newHexString.Substring(0, newHexString.Length -1);
                
            //Number of values must be dividable by 3 (plus faking the last space)
            if ((newHexString.Length +1) % 3 > 0)
                newHexString = newHexString.Substring(0, newHexString.Length - 1);

            return newHexString;
        }
    }
}
