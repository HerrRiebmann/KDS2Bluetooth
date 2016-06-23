using System.Collections.ObjectModel;
using Backend.Data;

namespace EcuEmulator.Data
{
    public enum PidTypes
    {
        //FuelSystemStatus = 3,
        //CalculatedEngineLoadValue = 4,
        //EngineCoolantTemperature = 5,
        //FuelPressure = 10,
        //RotationsPerMinute = 12,
        //Speed = 13,
        //IntakeAirTemperature = 15,
        //ThrottleOpening = 17,
        //BarometricPressure = 33,
        //StartDiagnostic = 200,  //Undocumented!
        //Gear = 201,             //Undocumented!
        //AbsPressure = 202       //Undocumented!
        StartDiagnostic = 200,  //80 but Mode 10 (Not 21)
        PidList1to32 = 0,
        StarterSwitch = 2,
        ThrottleOpening = 4,
        BarometricInletPressure = 5,
        EngineCoolantTemperature = 6,
        IntakeAirTemperature = 7,
        AtmosphericPressure = 8,
        RotationsPerMinute = 9,
        BatteryVoltage = 10,
        GearPosition = 11,
        Speed = 12,
        Injector1OperatingTime = 14,
        Injector2OperatingTime = 15,
        Injector3OperatingTime = 16,
        Injector4OperatingTime = 17,
        Injector5OperatingTime = 18,
        Injector6OperatingTime = 19,
        Injector7OperatingTime = 20,
        Injector8OperatingTime = 21,
        IgnitionTimingCylinder1 = 22,
        IgnitionTimingCylinder2 = 23,
        IgnitionTimingCylinder3 = 24,
        IgnitionTimingCylinder4 = 25,
        EcuId = 26,
        CoAdjustValve1 = 30,
        CoAdjustValve2 = 31,
        PidList33to65 = 32,
        CoAdjustValve3 = 33,
        CoAdjustValve4 = 34,
        PidList66to96 = 64,
        TotalOperatingHours = 68,
        SubThrottleValveOperatingAngle = 91,
        VehicleDownSensorVoltage = 94,
        InternalControlVoltage2 = 95,
        PidList97to128 = 96,
        InternalControlVoltage3 = 97,
        EmergencyStop = 98,
        FuelCutMode = 106,
        EngineStartMode = 107,
        PidList129to160 = 128,
        PidList160to191 = 160,
        PidList192to223 = 192
    }

    public class Register : Serializer<Register>
    {
        private PidTypes _pidType;
        public PidTypes PidType
        {
            get { return _pidType; }
            set {
                if (_pidType != value)
                {
                    _pidType = value;
                    Notify("PidType");
                } }
        }

        private byte _mode;
        public byte Mode
        {
            get { return _mode; }
            set {
                if (_mode != value)
                {
                    _mode = value;
                    Notify("Mode");
                } }
        }

        private byte _parameter;
        public byte Parameter
        {
            get { return _parameter; }
            set
            {
                if (_parameter != value)
                {
                    _parameter = value;
                    Notify("Parameter");
                }
            }
        }
        
        private string _description = string.Empty;
        public string Description
        {
            get { return _description; }
            set {
                if (_description != value)
                {
                    _description = value;
                    Notify("Description");
                } }
        }

        #region Response
        private int _responseMin;
        public int ResponseMin
        {
            get { return _responseMin; }
            set
            {
                if (_responseMin != value)
                {
                    _responseMin = value;
                    Notify("ResponseMin");
                }
            }
        }

        private int _responseMax;
        public int ResponseMax
        {
            get { return _responseMax; }
            set
            {
                if (_responseMax != value)
                {
                    _responseMax = value;
                    Notify("ResponseMax");
                }
            }
        }

        private int _responseSteps;
        public int ResponseSteps
        {
            get { return _responseSteps; }
            set {
                if (_responseSteps != value)
                {
                    _responseSteps = value;
                    Notify("ResponseSteps");
                } }
        }

        private byte[] _responseStaticValues;
        public byte[] ResponseStaticValues
        {
            get { return _responseStaticValues; }
            set { _responseStaticValues = value; }
        }
        #endregion

        public Register()
        {
            
        }

        public Register(PidTypes pidType, byte mode, byte parameter)
        {
            _pidType = pidType;
            _mode = mode;
            _parameter = parameter;
        }

        public Register(PidTypes pidType, byte mode, byte parameter, string description)
        {
            _pidType = pidType;
            _mode = mode;
            _parameter = parameter;
            _description = description;
        }

        public Register(PidTypes pidType, byte mode, byte parameter, string description, int responseMin, int responseMax, int responseStep)
        {
            _pidType = pidType;
            _mode = mode;
            _parameter = parameter;
            _description = description;
            _responseMin = responseMin;
            _responseMax = responseMax;
            _responseSteps = responseStep;
        }

        public Register(PidTypes pidType, byte mode, byte parameter, string description, byte[] staticResponse)
        {
            _pidType = pidType;
            _mode = mode;
            _parameter = parameter;
            _description = description;
            _responseStaticValues = staticResponse;
        }
    }

    public class RegisterList : Serializer<RegisterList>
    {
        private ObservableCollection<Register> _items = new ObservableCollection<Register>();

        public ObservableCollection<Register> Items
        {
            get { return _items; }
            set { _items = value; }
        }
    }
}
