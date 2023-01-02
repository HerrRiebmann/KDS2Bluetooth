using System.Collections.Generic;
using System.Linq;
using System.Windows;
using EcuEmulator.Data;

namespace Hex_Calculator.Data
{
    public static class Filter
    {
        public static void Distinct(ref DataEntries calculations)
        {
            var calcsDistinct = calculations.Distinct(new EqualityParity()).ToArray();
            if (MessageBox.Show($"Do you want to reduce {calculations.Count} to {calcsDistinct.Count()} entries?", "Distinct", MessageBoxButton.YesNo, MessageBoxImage.Question) == MessageBoxResult.No)
                return;
            calculations.Clear();
            foreach (var calc in calcsDistinct)
            {
                calculations.Add(calc);
            }
        }

        public static void IncomingOnly(ref DataEntries calculations)
        {
            foreach (var entry in calculations.ToList())
                if(entry.MsgSender == "F1")
                    calculations.Remove(entry);
        }
    }

    public class EqualityParity : IEqualityComparer<DataEntry>
    {
        public bool Equals(DataEntry x, DataEntry y)
        {
            return x?.HexString == y?.HexString;
        }

        public int GetHashCode(DataEntry obj)
        {
            return obj.HexString.GetHashCode();
        }
    }
}
