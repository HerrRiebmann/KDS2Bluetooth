using System;
using Backend.Data;

namespace Backend.Events
{
    public class MessagingEventArgs : EventArgs
    {
        public readonly Message Message;

        public MessagingEventArgs(Message message)
        {
            Message = message;
        }
    }
}
