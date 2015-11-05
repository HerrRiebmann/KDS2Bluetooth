using System;

namespace SerialComPort.Events
{
    public class DataEventArgs : EventArgs
    {
        public readonly string Data;

        public DataEventArgs(string data)
        {
            Data = data;
        }
    }
}
