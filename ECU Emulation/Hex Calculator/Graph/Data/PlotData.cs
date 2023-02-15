using System.Collections.Generic;

namespace Hex_Calculator.Graph.Data
{
    public class PlotData
    {
        private string _title = string.Empty;
        private string _xAxisLabel = string.Empty;
        private string _yAxisLabel = string.Empty;
        private List<PlotItem> _items;

        public string Title { get => _title; set => _title = value; }
        public string XAxisLabel { get => _xAxisLabel; set => _xAxisLabel = value; }
        public string YAxisLabel { get => _yAxisLabel; set => _yAxisLabel = value; }

        public List<PlotItem> Items { get => _items; set => _items = value; }

        public PlotData()
        {
            _items = new List<PlotItem>();
            _title = "Fancy title from file";
        }        
    }
}
