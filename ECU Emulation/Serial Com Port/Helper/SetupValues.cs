using System;
using System.IO.Ports;

namespace SerialComPort.Helper
{
    public static class SetupValues
    {
        public static string[] GetParityValues()
        {
            return Enum.GetNames(typeof (Parity));
        }

        public static string[] GetStopBitValues()
        {
            return Enum.GetNames(typeof (StopBits));
        }
       
        public static string[] GetPortNameValues()
        {
            return SerialPort.GetPortNames();
        }

        public static System.Collections.IEnumerable GetBaudrateValues()
        {
            return new []
            {
                300,
                600,
                1200,
                2400,
                4800,
                9600,
                38400,
                14400,
                19200,
                28800,
                36000,
                10400,
                115000
            };
        }

        public static System.Collections.IEnumerable GetDataBits()
        {
            return new[]
            {
                7,
                8,
                9
            };
        }

        public static System.Collections.IEnumerable GetEncoding()
        {
            return new[]
            {
                "ASCII",
                "unicode",
                "utf-8",
                "utf-16",
                "utf-32",
            };
        }

        public static System.Collections.IEnumerable GetHandshakes()
        {
            return Enum.GetNames(typeof (Handshake));
        }
    }
}
