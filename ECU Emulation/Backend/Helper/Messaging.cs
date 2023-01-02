using System;
using Backend.Data;
using Backend.Events;

namespace Backend.Helper
{
    public static class Messaging
    {
        public delegate void MessageReceivedHandler(MessagingEventArgs e);
        public static event MessageReceivedHandler MessageReceived = delegate { };

        public enum Types
        {
            Incoming, Outgoing, Normal, Warning, Error, Info, Debug, Echo
        }

        public enum Caller
        {
            Unknown,
            Emulator,
            ComPort,
            Form
        }

        private static volatile Messages _messageList;
        public static Messages MessageList
        {
            get { return _messageList ?? (_messageList = new Messages()); }
            set { _messageList = value; }
        }

        private static string _typeFilter = "Incoming,Outgoing,Warning,Error,Echo";
        public static string TypeFilter
        {
            get { return _typeFilter; }
            set { _typeFilter = value; }
        }

        private static string _callerFilter = "Emulator";//"Emulator,ComPort";
        public static string CallerFilter
        {
            get { return _callerFilter; }
            set { _callerFilter = value; }
        }
        
        public static void WriteLine(string message)
        {
            WriteLine(message, Types.Normal);
        }

        public static void WriteLine(string message, Types type)
        {
            WriteLine(message, type, Caller.Unknown);
        }

        public static void WriteLine(string message, Types type, Caller caller)
        {
            WriteLine(message, type, caller, string.Empty);
        }

        public static void WriteLine(string message, Types type, Caller caller, string convertedValue)
        {
            WriteLine(new Message(message, type, caller, convertedValue));
        }

        public static void WriteLine(Message message)
        {
            if (!message.Type.ToString().Equals("Debug"))
            {
                if (!String.IsNullOrEmpty(CallerFilter) && !CallerFilter.Contains(message.Caller.ToString()))
                    return;

                if (!String.IsNullOrEmpty(TypeFilter) && !TypeFilter.Contains(message.Type.ToString()))
                    return;
            }
            //Dispatcher.CurrentDispatcher.Invoke((Action)(() =>
            //{
                lock (MessageList)
                {
                    MessageList.Item.Add(message);
                }
            //}));
            if (MessageReceived != null)
                MessageReceived(new MessagingEventArgs(message));
        }
    }
}
