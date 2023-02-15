using EcuEmulator.Data;
using Hex_Calculator.Graph.Data;
using ScottPlot;
using ScottPlot.Plottable;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Windows;
using System.Windows.Controls;

namespace Hex_Calculator.Graph
{
    /// <summary>
    /// Interaktionslogik für PlotViewer.xaml
    /// </summary>
    public partial class PlotViewer : Window
    {
        private readonly Plot _plot;        
        public Plot Plot { get => _plot;}
        
        private DataEntries _dataEntries;        
        public DataEntries DataEntries { get => _dataEntries; set { _dataEntries = value;
                InitializeData(); } }

        private PlotData _plotData;
        public PlotData PlotData { get => _plotData; set => _plotData = value; }
        private string _fileName;
        public string FileName { get => _fileName; set => _fileName = value; }
                
        private MarkerPlot HighlightedPoint;
        private int LastHighlightedIndex = -1;
        private readonly List<ScatterPlot> _scatterPlotList;
        private int _selectedPlot = -1;

        public PlotViewer()
        {
            InitializeComponent();
            _plot = new Plot();            
            _scatterPlotList = new List<ScatterPlot>();            
        }

        private void InitializeData()
        {
            ConvertDataEntries();
            DataContext = PlotData;
            wpfMultiPlot.Reset(_plot);
            UpdateDataEntries();
            wpfMultiPlot.MouseMove += WpfMultiPlot_MouseMove;
        }

        private void ConvertDataEntries()
        {
            _plotData = DataUnitizer.ConvertDataEntries(_dataEntries);
            _plotData.Title = FileName;
            _plot.Title(_plotData.Title);
            _plot.YLabel(_plotData.YAxisLabel);
            _plot.XLabel(_plotData.XAxisLabel);            
        }

        private void UpdateDataEntries()
        {
            _scatterPlotList.Clear();
            foreach (var item in _plotData.Items)
            {
                if (!item.Active)
                    continue;
                _scatterPlotList.Add(_plot.AddScatter(item.Entries.Select(e => e.Key).ToArray(), item.Entries.Select(e => e.Value).ToArray(), item.Color, 1, 3, MarkerShape.filledCircle, LineStyle.Solid, item.Description));                
            }
            InitMouseHighlight();            
            wpfMultiPlot.Refresh();            
        }

        private void UpdateButton_Click(object sender, RoutedEventArgs rea)
        {
            _plot.Clear();
            UpdateDataEntries();
        }

        private void ListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var lb = e.Source as ListBox;
            var selectedIndex = -1;
            if (lb != null)
            {
                selectedIndex = lb.SelectedIndex;
            }

            _selectedPlot = -1;
            var maxActiveItems = _plotData.Items.Count(i => i.Active);
            for ( var i = 0; i <= selectedIndex; i++)
            {
                if (maxActiveItems - 1 == _selectedPlot)
                {
                    _selectedPlot = -1;
                    break;
                }
                if (_plotData.Items[i].Active)
                    _selectedPlot++;
            }
            if(_selectedPlot < 0 || !_plotData.Items[selectedIndex].Active)
                ItemsListBox.SelectedIndex = -1;
        }

        private void InitMouseHighlight()
        {
            // Add a red circle we can move around later as a highlighted point indicator
            HighlightedPoint = wpfMultiPlot.Plot.AddPoint(0, 0);
            HighlightedPoint.Color = System.Drawing.Color.Red;
            HighlightedPoint.MarkerSize = 10;
            HighlightedPoint.MarkerShape = MarkerShape.openCircle;
            HighlightedPoint.IsVisible = false;
            
            ItemsListBox.SelectedIndex = -1;
        }

        private void WpfMultiPlot_MouseMove(object sender, System.Windows.Input.MouseEventArgs e)
        {
            if (_selectedPlot < 0 || _selectedPlot >= _scatterPlotList.Count())
                return;
            // determine point nearest the cursor
            (double mouseCoordX, double mouseCoordY) = wpfMultiPlot.GetMouseCoordinates();
            double xyRatio = wpfMultiPlot.Plot.XAxis.Dims.PxPerUnit / wpfMultiPlot.Plot.YAxis.Dims.PxPerUnit;
            (double pointX, double pointY, int pointIndex) = _scatterPlotList[_selectedPlot].GetPointNearest(mouseCoordX, mouseCoordY, xyRatio);

            // place the highlight over the point of interest
            HighlightedPoint.X = pointX;
            HighlightedPoint.Y = pointY;
            HighlightedPoint.IsVisible = true;

            // render if the highlighted point chanaged
            if (LastHighlightedIndex != pointIndex)
            {
                LastHighlightedIndex = pointIndex;
                var items = _plotData.Items.FindAll(i => i.Active);
                if (items.Count < _selectedPlot)
                    return;
                var item = items[_selectedPlot];
                Debug.WriteLine($"Point index {pointIndex} at ({pointX:N2}, {pointY:N2}) {item.Entries[pointIndex].ToolTip}");
                HighlightedPoint.Text = item.Entries[pointIndex].ToolTip;
                wpfMultiPlot.Render();
            }
        }
    }
}
