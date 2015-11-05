using System;
using System.Collections.Generic;

namespace EcuEmulator.Data
{
    public struct Result
    {
        public string Value;

        public Emulator.ResponseType Type;
    }

    public struct Transfer
    {
        public byte Format;

        public byte Receiver;

        public byte Sender;

        public byte Length;

        public int LengthInt
        {
            get { return Convert.ToInt32(Length); }
        }

        public byte Mode;

        private List<byte> _values;
        public List<byte> Values
        {
            get { return _values ?? (_values = new List<byte>()); }
            set { _values = value; }
        }

        public byte Checksum;

        public bool TestChecksum
        {
            get
            {
                var byteList = new List<byte>
                {
                    Format,
                    Receiver,
                    Sender,
                    Length,
                    Mode
                };
                byteList.AddRange(Values);
                var checksum = Emulator.CalcCheckSum(byteList);
                return checksum.Equals(Checksum);
            }
        }
    }
}
