using System;
using System.Text;

namespace SerialComPort.Helper
{
    public static class Converter
    {
        public static int BaudRate(string baudRate)
        {
            return Convert.ToInt32(baudRate);
        }

        public static System.IO.Ports.Parity Parity(string parity)
        {
            return (System.IO.Ports.Parity)Enum.Parse(typeof(System.IO.Ports.Parity), parity);
        }

        public static System.IO.Ports.StopBits StopBits(string stopBits)
        {
            return (System.IO.Ports.StopBits)Enum.Parse(typeof(System.IO.Ports.StopBits), stopBits);
        }

        public static int DataBits(string dataBits)
        {
            //7, 8 & 9 only!
            return Convert.ToInt32(dataBits);
        }

        public static System.IO.Ports.Handshake Handshake(string handshake)
        {
            return (System.IO.Ports.Handshake)Enum.Parse(typeof(System.IO.Ports.Handshake), handshake);
        }

        public static Encoding Encode(string encoding)
        {
            return Encoding.GetEncoding(encoding);
        }
    }
}
