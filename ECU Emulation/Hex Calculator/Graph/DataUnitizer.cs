using EcuEmulator.Data;
using Hex_Calculator.Graph.Data;
using System;

namespace Hex_Calculator.Graph
{
    public static class DataUnitizer
    {
        public static PlotData ConvertDataEntries(DataEntries dataEntries)
        {
            var data = new PlotData();
            bool timestamp = false;
            int counter = 0;
            foreach (var entry in dataEntries)
            {
                var results = entry.GetResults;
                foreach (var result in results)
                {
                    if (entry.MsgReceiver != "F1") // Tester
                        continue;
                    if(entry.MsgSid == "7F") // Error
                        continue;
                    //Skip Text only values:
                    if (!string.IsNullOrEmpty(result.ResultString))
                        continue;
                    if(!data.Items.Exists(i => i.Description == result.Description))
                        data.Items.Add(new PlotItem { Description = result.Description, Formula = result.Formula, Unit = result.Unit, Active = Activate(result.Description), Color = GetNextColor(counter++) });
                    var item = data.Items.Find(i => i.Description == result.Description);
                    double key = entry.Timestamp != 0 ? TimeSpan.FromMilliseconds(entry.Timestamp).TotalSeconds : entry.MsgNo;
                    string tooltip = $"{result.ResultText} {result.Unit} ({entry.MsgNo})";
                    item.Entries.Add(new Entry { Key = key, Value = ValueConverter(result), ToolTip = tooltip });
                    if (!timestamp && entry.Timestamp > 0)
                        timestamp = true;
                }
            }
            if (timestamp)
                data.XAxisLabel = "Seconds";
            else
                data.XAxisLabel = "Entry No.";
            data.YAxisLabel = "Engine Data";
            return data;
        }

        private static double ValueConverter(CalculationResult result)
        {
            if (result.ResultBool != null)
                return result.ResultBool.Value ? 1 : 0;
            if (result.ResultInt != 0)
                return result.ResultInt;
            if (result.ResultDecimal != 0)
                return (double)result.ResultDecimal;
            return 0;
        }

        private static bool Activate(string description)
        {
            switch(description)
            {
                case "Engine Speed":
                    return true;
                case "Throttle Position Sensor":
                    return true;
                case "Intake/Manifold Air Pressure (IAP)": 
                case "Air Pressure (Inlet / Manifold)":
                    return true;
                default:
                    return false;
            }
        }

        private static System.Drawing.Color GetNextColor(int counter)
        {
            var myPalette = new ScottPlot.Palettes.Category20();
            if(myPalette.Count() > counter)
            {
                return myPalette.Colors[counter];
            }
            return GetRandomColor();
        }

        private static Random r = new Random();
        private static System.Drawing.Color GetRandomColor()
        {
            return System.Drawing.Color.FromArgb(255, (byte)r.Next(50, 255), (byte)r.Next(50, 255), (byte)r.Next(50, 255));
        }
        public static string LimitTo(this string s, int maxLen)
        {
            string toEnd = "...";

            if (s.Length > maxLen)
            {
                maxLen -= toEnd.Length;
                while (!char.IsWhiteSpace(s[maxLen])) maxLen--;
                s = s.Substring(0, maxLen) + toEnd;
            }
            return s;
        }
    }
}
