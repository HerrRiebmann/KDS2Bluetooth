namespace Hex_Calculator.Graph.Data
{
    public class Entry
    {
        private double _key;        
        private double _value;
        private string _toolTip;

        public double Key { get => _key; set => _key = value; }
        public double Value { get => _value; set => _value = value; }
        public string ToolTip { get => _toolTip; set => _toolTip = value; }
    }
}
