using System.Collections.ObjectModel;

namespace EcuEmulator.Data
{
    public class DataEntries : ObservableCollection<DataEntry>
    {
        public void Add(string hexValue)
        {
            Add(Count, hexValue);
        }
        public void Add(int msgNo, string hexValue, string timestamp)
        {
            this.Add(new DataEntry(msgNo, hexValue, timestamp));
        }
        public void Add(int msgNo, string hexValue)
        {
            this.Add(new DataEntry(msgNo, hexValue));
        }
    }
}
