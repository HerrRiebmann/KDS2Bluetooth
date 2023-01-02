using System;
using System.Collections.Generic;
using System.Linq;
using EcuEmulator.Data;

namespace EcuEmulator.Translation
{
    public static class Translate
    {        
        private static IEnumerable<Calculation> GetCalculations(DataEntry entry)
        {
            if(entry == null)
                return new Calculation[0];
            var sid = (byte)(HexCalculator.ConvertToByte(entry.MsgSid) - 0x40);
            var pid = HexCalculator.ConvertToByte(entry.MsgPid);
            var manufacturer = HexCalculator.ConvertToByte(entry.MsgSender);
            System.Diagnostics.Debug.WriteLine($"Filter: Sid {sid} Pid {pid} Manufacturer {(ManufacturerEnum)manufacturer}");
            return Emulator.Calculations.Items.Where(itm => itm.Sid == sid && (itm.Pid == 0xFF || itm.Pid == pid) && (byte)itm.Manufacturer == manufacturer && ((itm.Filter == 0xFF || itm.FilterPosition < 0) || entry.MsgValues.ElementAt(itm.FilterPosition) == itm.Filter));
        }

        private static IEnumerable<Calculation> GetDescriptions(DataEntry entry)
        {
            if (entry == null)
                return new Calculation[0];
            var sid = HexCalculator.ConvertToByte(entry.MsgSid);
            var pid = HexCalculator.ConvertToByte(entry.MsgPid);
            if(pid == 0)
                return new Calculation[0];
            var manufacturer = HexCalculator.ConvertToByte(entry.MsgReceiver);
            System.Diagnostics.Debug.WriteLine($"Filter: Sid {sid} Pid {pid} Manufacturer {(ManufacturerEnum)manufacturer}");
            return Emulator.Calculations.Items.Where(itm => itm.Sid == sid && itm.Pid == pid && (byte)itm.Manufacturer == manufacturer);
        }

        public static List<CalculationResult> GetResults(DataEntry entry)
        {
            var resultList = new List<CalculationResult>();
            var calcs = GetCalculations(entry);
            if(calcs.Count() == 0)
                calcs = GetDescriptions(entry);
            foreach (var calculation in calcs)
            {
                resultList.Add(CalculateResult(entry, calculation));
            }
            return resultList;
        }

        private static CalculationResult CalculateResult(DataEntry entry, Calculation calculation)
        {
            if(HexCalculator.ConvertToByte(entry.MsgSender) == (byte)ManufacturerEnum.Tester)
                return new CalculationResult(calculation, "Request");
            if (entry.MsgValues.Count() < calculation.ByteStartPosition + calculation.NumberOfBytes)            
                return new CalculationResult(calculation, 0, "NO DATA");
            
            object result;
            switch (calculation.DataType)
            {
                case DataTypes.Decimal:
                    result = ValueToDecimal(entry, calculation);
                    break;
                case DataTypes.Int:
                    result = (int)ValueToDecimal(entry, calculation);
                    break;
                case DataTypes.Text:
                    result = ValueToString(entry, calculation);
                    break;
                case DataTypes.Hex:
                    result = ValueToHexString(entry, calculation);
                    break;
                case DataTypes.Bool:
                case DataTypes.YesNo:
                    result = ValueToBool(entry, calculation);
                    break;
                case DataTypes.Binary:
                    result = ValueToBinaryString(entry, calculation);
                    break;
                default:
                    result = 0;
                    break;
            }

            return new CalculationResult(calculation, result, ValueToHexString(entry, calculation));
        }

        public static decimal ValueToDecimal(DataEntry entry, Calculation calc)
        {            
            decimal value = 0;
            for (int i = calc.ByteStartPosition; i < calc.ByteStartPosition + calc.NumberOfBytes; i++)
            {
                value *= 256;
                value += entry.MsgValues.ElementAt(i);
            }

            //Not supported/not available mostly 0xFF:
            if ((calc.NumberOfBytes == 1 && value == 0xFF) || (calc.NumberOfBytes == 2 && value == 0xFFFF) || (calc.NumberOfBytes == 3 && value == 0xFFFFFF))
                return 0xFF;

            var factor = (decimal)calc.Resolution;
            value = Convert.ToDecimal(value * factor * 256 / calc.ResolutionBase);
            
            //if (calc.Scale == 0)
            //    value = Math.Round(value, 0);
            value += calc.Offset;
            if (calc.Negotiate)
                value *= -1;
            return value;
        }

        public static bool ValueToBool(DataEntry entry, Calculation calc)
        {
            var b = entry.MsgValues.ElementAt(calc.ByteStartPosition);
            //Everything > 0 is true
            if (calc.BitPosition < 0)
                return b != 0;
            //Otherwise check Bit
            //00000010
            var c = b & calc.BitPosition;
            if (calc.Negotiate)
                return c == 0;
            return c != 0;
        }

        public static string ValueToString(DataEntry entry, Calculation calc)
        {
            var msgBytes = new byte[calc.NumberOfBytes];
            Array.Copy(entry.MsgValues.ToArray(), calc.ByteStartPosition, msgBytes, 0, calc.NumberOfBytes);
            return HexCalculator.ConvertToText(msgBytes);
        }

        public static string ValueToHexString(DataEntry entry, Calculation calc)
        {
            if (calc.NumberOfBytes + calc.ByteStartPosition > entry.MsgValues.Count())
                return "OutOfRange";
            var msgBytes = new byte[calc.NumberOfBytes];
            Array.Copy(entry.MsgValues.ToArray(), calc.ByteStartPosition, msgBytes, 0, calc.NumberOfBytes);
            return HexCalculator.ConvertToHexString(msgBytes);
        }

        public static string ValueToBinaryString(DataEntry entry, Calculation calc)
        {
            var msgBytes = new byte[calc.NumberOfBytes];
            Array.Copy(entry.MsgValues.ToArray(), calc.ByteStartPosition, msgBytes, 0, calc.NumberOfBytes);
            return HexCalculator.ConvertToBinaryString(msgBytes);
        }

        public static string PrettyPrint(this List<CalculationResult> results)
        {
            var output = string.Empty;
            foreach (var calculationResult in results)
            {
                if (!string.IsNullOrEmpty(output))
                    output += System.Environment.NewLine;
                output += $"{calculationResult.Description}\t{calculationResult.ResultText} {calculationResult.Unit}";
            }

            return output;
        }
    }
}
