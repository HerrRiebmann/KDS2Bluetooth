using System.Collections.Specialized;
using System.Windows;
using System.Windows.Data;
using Backend.Helper;

namespace EcuEmulatorForm
{
    /// <summary>
    /// Interaction logic for MessageWindow.xaml
    /// </summary>
    public partial class MessageWindow : Window
    {
        private bool _autoscroll = true;
        public bool Autoscroll
        {
            get { return _autoscroll; }
            set { _autoscroll = value; }
        }

        public MessageWindow()
        {
            InitializeComponent();
            ((INotifyCollectionChanged)itemListView.Items).CollectionChanged += ListView_CollectionChanged;
        }
        
        private void ListView_CollectionChanged(object sender, NotifyCollectionChangedEventArgs e)
        {
            ScrollToEnd();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            CollectionViewSource messagesViewSource = ((System.Windows.Data.CollectionViewSource)(this.FindResource("messagesViewSource")));
            // Load data by setting the CollectionViewSource.Source property:
            //messagesViewSource.Source = Messaging.MessageList;
            messagesViewSource.Source = Messaging.MessageList.Item;
            
        }

        private void ScrollToEnd()
        {
            if (Autoscroll && itemListView.Items.Count > 0)
                itemListView.ScrollIntoView(itemListView.Items[itemListView.Items.Count - 1]);
        }

        private void BtnClear_Click(object sender, RoutedEventArgs e)
        {
            Messaging.MessageList.Item.Clear();
        }
    }
}
