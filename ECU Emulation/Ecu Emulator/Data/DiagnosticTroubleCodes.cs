using System.Collections.Generic;
using System.Collections.ObjectModel;

namespace EcuEmulator.Data
{
    public class DiagnosticTroubleCodes : ObservableCollection<DiagnosticTroubleCode>
    {
        public void Add(char family, IEnumerable<byte> numbers, string description, ManufacturerEnum manufacturer, string note = "")
        {
            this.Add(new DiagnosticTroubleCode(family, numbers, description, null, manufacturer, note));
        }
        public void Add(char family, IEnumerable<byte> numbers, string description, FTB ftb, ManufacturerEnum manufacturer, string note = "")
        {
            this.Add(new DiagnosticTroubleCode(family, numbers, description, ftb, manufacturer, note));
        }
    }
    public class DiagnosticTroubleCode : Backend.Data.Serializer<DiagnosticTroubleCode>
    {
        private char _family;
        private IEnumerable<byte> _numbers;
        private string _description = string.Empty;
        private ManufacturerEnum _manufacturer = ManufacturerEnum.Unknown;
        private FTB _ftb;
        private string _note = string.Empty;
        private string _manufacturerCode = string.Empty;

        public DiagnosticTroubleCode()
        {

        }
        public DiagnosticTroubleCode(char family, IEnumerable<byte> numbers, string description, FTB ftb, string note = "")
        {
            _family = family;
            _numbers = numbers;
            _description = description;
            _ftb = ftb;
            _note = note;
        }     
        public DiagnosticTroubleCode(char family, IEnumerable<byte> numbers, string description, FTB ftb, ManufacturerEnum manufacturer, string note = "")
        {
            _family = family;
            _numbers = numbers;
            _description = description;
            _ftb = ftb;
            _manufacturer = manufacturer;
            _note = note;
        }
    }
    public class FTBs: ObservableCollection<FTB>
    {
        public FTB GetFTBByHex(byte code)
        {
            return null;
        }
    }
    public class FTB : Backend.Data.Serializer<FTB>
    {
        private byte _code;
        private string _description = string.Empty;

        public byte Code
        {
            get { return _code; }
            set
            {
                if (_code != value)
                {
                    _code = value;
                    Notify();
                }
            }
        }
        public string Description
        {
            get { return _description; }
            set
            {
                if (_description != value)
                {
                    _description = value;
                    Notify();
                }
            }
        }

        public FTB(byte code, string description)
        {
            _code = code;
            _description = description;
        }
    }
}
