using System.Collections.ObjectModel;
using System.ComponentModel;

namespace Backend.Data
{
    public class SerializableList<T> : Serializer<SerializableList<T>>
    {
        private ObservableCollection<T> _items = new ObservableCollection<T>();

        public ObservableCollection<T> Items
        {
            get => _items;
            set
            {
                if (_items != value)
                {
                    _items.CollectionChanged -= Items_CollectionChanged;
                    _items = value;
                    if (_items != null)
                        _items.CollectionChanged += Items_CollectionChanged;
                    Notify();
                }
            }
        }

        // ReSharper disable once UnusedMember.Global
        public SerializableList()
        {
            _items.CollectionChanged += Items_CollectionChanged;
        }
        private void Items_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            if (e.OldItems != null)
            {
                foreach (INotifyPropertyChanged item in e.OldItems)
                    item.PropertyChanged -= Item_PropertyChanged;
            }
            if (e.NewItems != null)
            {
                foreach (INotifyPropertyChanged item in e.NewItems)
                    item.PropertyChanged += Item_PropertyChanged;
            }
            //Notify("Item");
        }

        private void Item_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            Notify($"Items");
        }
    }

}
