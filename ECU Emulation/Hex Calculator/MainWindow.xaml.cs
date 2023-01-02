using System;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using EcuEmulator.Data;
using EcuEmulator.Translation;
using Hex_Calculator.Data;
using Microsoft.Win32;
using Path = System.IO.Path;

namespace Hex_Calculator
{
    /// <summary>
    /// Interaktionslogik für MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private readonly string[] Extensions = { ".sds", ".sdt", ".kds", ".hds", ".xml", ".csv", ".txt", ".raw" };
        DataEntries _dataEntries;
        private DetailView _detail;
        private string _currentFilename;
        public bool FilterActive { get; set; }

        public MainWindow()
        {
            InitializeComponent();
            _dataEntries = new DataEntries();
            InitTestData();
            
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            var calcViewSource = ((CollectionViewSource)(FindResource("calcViewSource")));
            calcViewSource.Source = _dataEntries;
        }

        private void InitTestData()
        {
            //Calculations.Add("43 6F 6D 6D 30 33 07 80 12 F1 02 21 08 AE 00 00");
            //Calculations.Add("80 12 F1 02 21 08 AE");
            //Calculations.Add("47 11");
            //Calculations.Add("08 5");
            //Calculations.Add("80 F1 12 12 61 50 03 53 A0 FF 4A BD 62 AD B8 03 8F FF FF FF FF FF 96");
            //Calculations.Add("80 F1 12 34 61 08 04 16 55 A0 03 53 A0 FF FF FF FF 00 00 38 B9 46 45 BA 00 00 00 FF FF FF FF 00 00 00 00 00 00 00 00 FF FF 40 40 40 40 FF EE FF FF FF 00 00 04 28 FF FF 65");
            //Calculations.Add("43 6F 6D 6D 30 33 07 80 F1 12 12 61 50 03 53 A0 FF 4A BD 62 AD B8 03 8F FF FF FF FF FF 96 00 00");
            ////Checksum pos
            //Calculations.Add("00 80 F1 12 34 61 08 04 16 55 A0 03 53 A0 FF FF FF FF 00 00 E0 B9 46 45 BA 00 00 00 FF FF FF FF 00 00 00 00 00 00 00 00 FF FF 40 40 40 40 FF EE FF FF FF 00 00 04 28 FF FF 0D");
            //Error to long:
            //Calculations.Add("00 80 F1 12 34 61 08 04 16 55 A0 03 53 A0 FF FF FF FF 0D 4B 38 9A 4E 46 B9 AE 00 00 FF FF FF FF 06 B1 06 9A 06 98 06 75 FF FF 44 44 44 44 FF 1A FF FF FF 00 00 04 28 FF FF");
            //_dataEntries.Add("80 F1 12 34 61 08 01 17 69 A2 00 96 00 FF FF FF 00 00 00 39 BA 36 38 BB 00 FF 00 FF 7C 35 FF 00 00 00 00 00 00 00 00 FF FF 40 40 40 40 FF FF 00 74 15 30 04 05 28 FF FF 5E");
        }
        
        private void Window_DragOver(object sender, DragEventArgs e)
        {
            var files = (string[])e.Data.GetData(DataFormats.FileDrop);
            foreach (var file in files)
            {
                if (!Extensions.Contains(Path.GetExtension(file).ToLower()))
                {
                    e.Effects = DragDropEffects.None;
                    e.Handled = false;
                    return;
                }
            }
            e.Handled = true;
            e.Effects = DragDropEffects.Copy;
        }

        private void Window_Drop(object sender, DragEventArgs e)
        {
            var files = (string[])e.Data.GetData(DataFormats.FileDrop);
            foreach (var file in files)
            {
                OpenFile(file);
            }
            
        }

        private void OpenFile(string file)
        {
            if (Extensions.Contains(Path.GetExtension(file).ToLower()))
            {
                IsEnabled = false;
                Mouse.OverrideCursor = Cursors.Wait;
                if (Path.GetExtension(file).Equals(".csv", StringComparison.CurrentCultureIgnoreCase))
                    Import.LoadCsvData(file, ref _dataEntries);
                else if (Path.GetExtension(file).Equals(".txt", StringComparison.CurrentCultureIgnoreCase))
                    Import.LoadTxtData(file, ref _dataEntries);
                else if (Path.GetExtension(file).Equals(".raw", StringComparison.CurrentCultureIgnoreCase))
                    Import.LoadRawData(file, ref _dataEntries);
                else
                    Import.LoadHexData(file, ref _dataEntries);
                IsEnabled = true;
                Mouse.OverrideCursor = null;
                Title = $"Diagnostic Hexadecimal Calculator / Reader ({Path.GetFileName(file)})";
                _currentFilename = file;
                return;
            }
        }

        private void BtnDistinct_Click(object sender, RoutedEventArgs e)
        {
            Filter.Distinct(ref _dataEntries);
        }

        private void BtnIncomingOnly_Click(object sender, RoutedEventArgs e)
        {
            Filter.IncomingOnly(ref _dataEntries);            
        }

        private void Row_DoubleClick(object sender, MouseButtonEventArgs e)
        {
            var row = sender as DataGridRow;
            var entry = (DataEntry) row.Item;
            if(_detail == null)
                _detail = new DetailView();
            _detail.CalculationResultList.AddRange(entry.GetResults);
            _detail.Show();
        }

        private void BtnDetails_Click(object sender, RoutedEventArgs e)
        {
            if (!calcDataGrid.HasItems)
                return;
            DataEntry entry;
            var selectedItem = calcDataGrid.SelectedItem;
            if (selectedItem != null)
                entry = (DataEntry) selectedItem;
            else
                entry = (DataEntry)calcDataGrid.Items[0];
            if (_detail == null)
                _detail = new DetailView();
            _detail.CalculationResultList.AddRange(entry.GetResults);
            _detail.Show();
        }

        private void BtnCleanup_Click(object sender, RoutedEventArgs e)
        {
            Import.CleanupFile(_currentFilename);
        }

        private void calcDataGrid_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (_detail == null || !_detail.IsVisible)
                return;
            
            if (sender is DataGrid item)
            {
                var entry = (DataEntry)item.CurrentItem;
                if (entry == null)
                    return;
                _detail.CalculationResultList.AddRange(entry.GetResults);
            }
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            if (_detail != null)
            {
                _detail.ParentClosing = true;
                _detail.Close();
            }
        }

        private void BtnFilter_Click(object sender, RoutedEventArgs e)
        {
            InputBox.Visibility = Visibility.Visible;
        }
        private void YesButton_Click(object sender, RoutedEventArgs e)
        {
            InputBox.Visibility = Visibility.Collapsed;
            FilterActive = !string.IsNullOrEmpty(InputTextBox.Text);
            if (!FilterActive)
                return;
            var calcViewSource = (CollectionViewSource)FindResource("calcViewSource");
            calcViewSource.Filter += CalcViewSource_Filter;
        }

        private void NoButton_Click(object sender, RoutedEventArgs e)
        {
            if (!FilterActive)
                return;
            InputBox.Visibility = Visibility.Collapsed;
            
            // Clear InputBox.
            InputTextBox.Text = string.Empty;
            var calcViewSource = (CollectionViewSource)FindResource("calcViewSource");
            calcViewSource.Filter -= CalcViewSource_Filter;
            FilterActive = false;
        }

        private void CalcViewSource_Filter(object sender, FilterEventArgs e)
        {
            if (string.IsNullOrEmpty(InputTextBox.Text))
            {
                e.Accepted = true;
                return;
            }

            if (e.Item is DataEntry dataEntry)
            {
                var results = Translate.GetResults(dataEntry);
                e.Accepted = results.Any(r => r.ResultText.Equals(InputTextBox.Text));
            }
        }
        
        private void HexStringTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if(e.Key == Key.Return && _dataEntries.Count == 0)
                _dataEntries.Add(((TextBox)sender).Text);
        }

        private void OpenButton_Click(object sender, RoutedEventArgs e)
        {
            var openFileDialog = new OpenFileDialog();            
            //openFileDialog.InitialDirectory = "Files";
            openFileDialog.Filter = "Log files (*.csv)|*.csv|All files (*.*)|*.*";
            openFileDialog.FilterIndex = 2;
            openFileDialog.RestoreDirectory = true;            
            if (openFileDialog.ShowDialog().GetValueOrDefault())
            {
                var filePath = openFileDialog.FileName;
                OpenFile(filePath);
            }
            
        }
    }
}
