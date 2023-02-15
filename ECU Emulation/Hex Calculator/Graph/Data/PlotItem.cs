using System.Collections.Generic;
using System.Drawing;

namespace Hex_Calculator.Graph.Data
{
    public class PlotItem
    {       
        private bool _active;        
        private string _description;
        private string _unit;
        private string _hexValue;
        private string _formula;
        private List<Entry> _entries;
        private Color _color;
        private System.Windows.Media.Brush _brush;

        public bool Active { get => _active; set => _active = value; }        
        public string Description { get => _description; set => _description = value; }
        public string ShortDescription { get { return Description.LimitTo(31); } }
        public string Unit { get => _unit; set => _unit = value; }
        public string HexValue { get => _hexValue; set => _hexValue = value; }
        public string Formula { get => _formula; set => _formula = value; }
        public List<Entry> Entries { get => _entries; set => _entries = value; }
        public Color Color { get => _color; set { _color = value;
                _brush = new System.Windows.Media.SolidColorBrush(System.Windows.Media.Color.FromArgb(_color.A, _color.R, _color.G, _color.B));
            } }
        public System.Windows.Media.Brush Brush { get => _brush;}

        public PlotItem()
        {
            _entries= new List<Entry>();
        }
    }
}
