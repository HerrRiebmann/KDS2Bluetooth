using System;

namespace SerialComPort.Events
{
    public class InfoEventArgs : EventArgs
    {
        public readonly string Info;
        public readonly Serial.MessageTypes MessageType;

        public InfoEventArgs(Serial.MessageTypes type, string info)
        {
            Info = info;
            MessageType = type;
        }
    }
}