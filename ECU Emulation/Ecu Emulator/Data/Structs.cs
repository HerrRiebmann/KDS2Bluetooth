using System;
using System.Collections.Generic;

namespace EcuEmulator.Data
{
    public struct Result
    {
        public string Value;

        public Emulator.ResponseType Type;
    }

    public struct Transfer
    {
        public byte Format;

        public byte Receiver;

        public byte Sender;

        public byte Length;

        public int LengthInt
        {
            get { return Convert.ToInt32(Length); }
        }

        public byte Mode;

        private List<byte> _values;
        public List<byte> Values
        {
            get { return _values ?? (_values = new List<byte>()); }
            set { _values = value; }
        }

        public byte Checksum;

        public bool TestChecksum
        {
            get
            {
                var byteList = new List<byte>
                {
                    Format,
                    Receiver,
                    Sender,
                    Length,
                    Mode
                };
                byteList.AddRange(Values);
                var checksum = Emulator.CalcCheckSum(byteList);
                return checksum.Equals(Checksum);
            }
        }
    }

    public class CalculationResult
    {
        public CalculationResult(Calculation calculation, string resultPlaceholder)
        {
            Description = calculation.Description;
            Unit = string.Empty;
            HexValue = string.Empty;
            Formula = string.Empty;
            ResultText = resultPlaceholder;
        }
        public CalculationResult(Calculation calculation, object result, string hexValue)
        {
            Description = calculation.Description;
            Unit = calculation.Unit;
            HexValue = hexValue;
            Formula = calculation.Formula;

            if (result is int)
            {
                ResultInt = Convert.ToInt32(result);
                ResultText = ResultInt == 0xFF ? "--" : ResultInt.ToString();
            }

            if (result is decimal)
            {
                ResultDecimal = Convert.ToDecimal(result);
                ResultText = ResultDecimal == 0xFF ? "--" : ResultDecimal.ToString("0.##");
            }

            if (result is bool)
            {
                ResultBool = Convert.ToBoolean(result);
                if (calculation.DataType == DataTypes.Bool)
                    ResultText = ResultBool.Value ? "On" : "Off";
                else
                    ResultText = ResultBool.Value ? "Yes" : "No";
            }

            if (result is string)
            {
                ResultString = Convert.ToString(result);
                ResultText = ResultString;
            }
        }

        public int ResultInt;
        public decimal ResultDecimal;
        public bool? ResultBool;
        public string ResultString;

        public string ResultText { get; }
        
        public string Description { get; }

        public string Unit { get; }

        public string HexValue { get; }

        public string Formula { get; }
    }
}
