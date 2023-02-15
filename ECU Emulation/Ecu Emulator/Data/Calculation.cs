using System;
using Backend.Data;

namespace EcuEmulator.Data
{
    public enum ManufacturerEnum
    {
        Unknown = 0,
        Kawasaki = 0x11,
        Suzuki = 0x12,
        KawasakiABS = 0x28,
        Honda = 0x72,
        Tester = 0xF1,
        OBD2 = 0x33 //?!? - OBD2 on KLine
    }

    public enum DataTypes
    {
        Decimal,
        Int,
        Text,
        Hex,
        Bool,
        YesNo,
        Binary
    }

    public class Calculation : Backend.Data.Serializer<Calculation>
    {
        public Calculation()
        {
            
        }

        private ManufacturerEnum _manufacturer;

        private int _no;

        private byte _sid;

        private byte _pid = 0xFF; //Wildcard

        private byte _filter = 0xFF; //Wildcard

        private int _filterPosition = -1; //Wildcard

        private int _numberOfBytes = 1;

        private int _byteStartPosition;

        private double _resolution = 1;

        private int _resolutionBase = 255;

        private int _bitPosition = -1; //Not initialized

        private int _offset;

        private int _minimumValue;

        private int _maximumValue;

        private int _scale;

        private int _decimalPlaces = 0;

        private string _unit;

        private DataTypes _dataType = DataTypes.Decimal;

        private string _description = string.Empty;

        private string _formula = string.Empty;

        private bool _negotiate = false;

        public int No
        {
            get { return _no; }
            set {
                if (_no != value)
                {
                    _no = value;
                    Notify();
                } }
        }

        public byte Sid
        {
            get { return _sid; }
            set {
                if (_sid != value)
                {
                    _sid = value;
                    Notify();
                } }
        }

        public byte Pid
        {
            get { return _pid; }
            set {
                if (_pid != value)
                {
                    _pid = value;
                    Notify();
                } }
        }

        public byte Filter
        {
            get { return _filter; }
            set {
                if (_filter != value)
                {
                    _filter = value;
                    Notify();
                } }
        }

        public int FilterPosition
        {
            get { return _filterPosition; }
            set {
                if (_filterPosition != value)
                {
                    _filterPosition = value;
                    Notify();
                } }
        }

        public int NumberOfBytes
        {
            get { return _numberOfBytes; }
            set {
                if (_numberOfBytes != value)
                {
                    _numberOfBytes = value;
                    Notify();
                } }
        }

        public int ByteStartPosition
        {
            get { return _byteStartPosition; }
            set {
                if (_byteStartPosition != value)
                {
                    _byteStartPosition = value;
                    Notify();
                } }
        }

        public double Resolution
        {
            get { return _resolution; }
            set {
                if (_resolution != value)
                {
                    _resolution = value;
                    Notify();
                } }
        }

        public int ResolutionBase
        {
            get { return _resolutionBase; }
            set {
                if (_resolutionBase != value)
                {
                    _resolutionBase = value;
                    Notify();
                } }
        }

        public int Offset
        {
            get { return _offset; }
            set {
                if (_offset != value)
                {
                    _offset = value;
                    Notify();
                } }
        }

        public int BitPosition
        {
            get { return _bitPosition; }
            set {
                if (_bitPosition != value)
                {
                    _bitPosition = value;
                    Notify();
                } }
        }

        public int MinimumValue
        {
            get { return _minimumValue; }
            set {
                if (_minimumValue != value)
                {
                    _minimumValue = value;
                    Notify();
                } }
        }

        public int MaximumValue
        {
            get { return _maximumValue; }
            set {
                if (_maximumValue != value)
                {
                    _maximumValue = value;
                    Notify();
                } }
        }

        public int Scale
        {
            get { return _scale; }
            set {
                if (_scale != value)
                {
                    _scale = value;
                    Notify();
                } }
        }

        public int DecimalPlaces
        {
            get { return _decimalPlaces; }
            set {
                if (_decimalPlaces != value)
                {
                    _decimalPlaces = value;
                    Notify();
                } }
        }

        public string Unit
        {
            get { return _unit; }
            set {
                if (_unit != value)
                {
                    _unit = value;
                    Notify();
                } }
        }

        public DataTypes DataType
        {
            get { return _dataType; }
            set {
                if (_dataType != value)
                {
                    _dataType = value;
                    Notify();
                } }
        }

        public string Description
        {
            get { return _description; }
            set {
                if (_description != value)
                {
                    _description = value;
                    Notify();
                } }
        }

        public string Formula
        {
            get { return _formula; }
            set {
                if (_formula != value)
                {
                    _formula = value;
                    Notify();
                } }
        }

        public bool Negotiate
        {
            get { return _negotiate; }
            set
            {
                if (_negotiate != value)
                {
                    _negotiate = value;
                    Notify();
                }
            }
        }

        public ManufacturerEnum Manufacturer
        {
            get { return _manufacturer; }
            set {
                if (_manufacturer != value)
                {
                    _manufacturer = value;
                    Notify();
                    switch (_manufacturer)
                    {
                        case ManufacturerEnum.Unknown:
                            break;
                        case ManufacturerEnum.Kawasaki:
                        case ManufacturerEnum.KawasakiABS:
                            ResolutionBase = 256;
                            break;
                        case ManufacturerEnum.Suzuki:
                            ResolutionBase = 255;
                            break;
                        case ManufacturerEnum.Honda:
                            break;
                        case ManufacturerEnum.Tester:
                            break;
                        case ManufacturerEnum.OBD2:
                            ResolutionBase = 255;
                            break;
                        default:
                            throw new ArgumentOutOfRangeException();
                    }
                } }
        }
    }

    public class Calculations : SerializableList<Calculation>
    {
        public void Add(Calculation calculation)
        {
            if (calculation.No == 0)
                calculation.No = Items.Count;
            Items.Add(calculation);
        }

        public void AddRange(SerializableList<Calculation> calculationList)
        {
            foreach (var calculation in calculationList.Items)
            {
                Items.Add(calculation);
            }
        }

        public void Clear()
        {
            Items.Clear();
        }
    }
}
