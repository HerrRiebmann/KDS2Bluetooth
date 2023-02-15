using System.Windows;
using EcuEmulator.Data;

namespace Hex_Calculator
{
    /// <summary>
    /// Interaktionslogik für DetailView.xaml
    /// </summary>
    public partial class DetailView : Window
    {
        public Backend.Data.RangeObservableCollection<CalculationResult> CalculationResultList;
        
        public bool ParentClosing;
        public DetailView()
        {
            InitializeComponent();
            CalculationResultList = new Backend.Data.RangeObservableCollection<CalculationResult>();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            var calculationResultViewSource = (System.Windows.Data.CollectionViewSource)FindResource("calculationResultViewSource");
            calculationResultViewSource.Source = CalculationResultList;
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            e.Cancel = !ParentClosing;
            if(e.Cancel)
                Hide();
        }
    }
}
