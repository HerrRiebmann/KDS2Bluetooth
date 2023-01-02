using EcuEmulator.Data;

namespace EcuEmulator.Initialization
{
    public static class Suzuki
    {
        public static void Init(ref Data.Calculations calculations)
        {
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Suzuki,
                Sid = 0x1A,
                Pid = 0x91,
                ByteStartPosition = 6,
                NumberOfBytes = 16,
                DataType = DataTypes.Text,
                Unit = "Id",
                Description = "ECU 1"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Suzuki,
                Sid = 0x1A,
                Pid = 0x9A,
                ByteStartPosition = 6,
                NumberOfBytes = 16,
                DataType = DataTypes.Text,
                Unit = "Id",
                Description = "ECU 2"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Suzuki,
                Sid = 0x81,
                Pid = 0xEA,
                ByteStartPosition = 5,
                NumberOfBytes = 2,
                DataType = DataTypes.Hex,
                Unit = "Headertype",
                Description = "Start Communication Request"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Suzuki,
                Sid = 0x82,
                Pid = 0xFF,
                ByteStartPosition = 4,
                NumberOfBytes = 1,
                DataType = DataTypes.Hex,
                Unit = "",
                Description = "Stop Communication Request",
                Formula = "Inverted 82"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Suzuki,
                Sid = 0x3E,
                Pid = 0xFF,
                ByteStartPosition = 4,
                NumberOfBytes = 1,
                DataType = DataTypes.YesNo,
                Unit = "Bool",
                Description = "Keepalive"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Suzuki,
                Sid = 0x18,
                Pid = 0xFF, //Wildcard
                ByteStartPosition = 5,
                DataType = DataTypes.Int,
                Description = "No of DTC"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Suzuki,
                Sid = 0x21,
                Pid = 0x08,
                ByteStartPosition = 6,
                DataType = DataTypes.Int,
                Description = "No of DTC"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Suzuki,
                Sid = 0x21,
                Pid = 0x08,
                ByteStartPosition = 7,
                NumberOfBytes = 2,
                DataType = DataTypes.Hex,
                Unit = "DTC",
                Description = "Freezed Frame 1"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Suzuki,
                Sid = 0x21,
                Pid = 0x08,
                ByteStartPosition = 10,
                NumberOfBytes = 2,
                DataType = DataTypes.Hex,
                Unit = "DTC",
                Description = "Freezed Frame 2"
            });                   
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Suzuki,
                Sid = 0x21,
                Pid = 0x08,
                ByteStartPosition = 16,
                NumberOfBytes = 1,
                MinimumValue = 0,                
                Resolution = 2.0,                
                DataType = DataTypes.Int,
                Unit = "km/h",
                Description = "Vehicle Speed",
                Formula = "A * 2"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Suzuki,
                Sid = 0x21,
                Pid = 0x08,
                ByteStartPosition = 17,
                NumberOfBytes = 2,
                MinimumValue = 0,
                MaximumValue = 10000,
                Resolution = 0.390625,
                Scale = 2000,
                DataType = DataTypes.Int,
                Unit = "rpm",
                Description = "Engine Speed",
                Formula = "(A * 256 + B) * 100 / 256"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Suzuki,
                Sid = 0x21,
                Pid = 0x08,
                ByteStartPosition = 19,
                NumberOfBytes = 1,
                MinimumValue = 0,
                MaximumValue = 180, // ???
                Resolution = 0.5,
                DataType = DataTypes.Decimal,
                Unit = "°",
                Description = "Throttle Position Sensor",
                Formula = "A / 2"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Suzuki,
                Sid = 0x21,
                Pid = 0x08,
                ByteStartPosition = 20,
                NumberOfBytes = 1,
                MinimumValue = 0,
                MaximumValue = 200,
                Resolution = 0.65,
                Offset = -20,
                DataType = DataTypes.Decimal,
                Unit = "kPa",
                Description = "Intake/Manifold Air Pressure (IAP)",
                Formula = "A * 166.66 / 256 - 20"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Suzuki,
                Sid = 0x21,
                Pid = 0x08,
                ByteStartPosition = 21,
                MinimumValue = -40,
                MaximumValue = 150,
                Resolution = 0.625,
                Offset = -30,
                DataType = DataTypes.Decimal,
                Unit = "°C",
                Description = "Engine Coolant Temperature (CLT)",
                Formula = "A * 160 / 256 - 30"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Suzuki,
                Sid = 0x21,
                Pid = 0x08,
                ByteStartPosition = 22,
                MinimumValue = -40,
                MaximumValue = 60,
                Resolution = 0.625,
                Offset = -30,
                DataType = DataTypes.Decimal,
                Unit = "°C",
                Description = "Intake Air Temperature (IAT)",
                Formula = "A * 160 / 256 - 30"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Suzuki,
                Sid = 0x21,
                Pid = 0x08,
                ByteStartPosition = 23,
                MinimumValue = 0,
                MaximumValue = 200,
                Resolution = 0.65,
                Offset = -20,
                DataType = DataTypes.Decimal,
                Unit = "kPa",
                Description = "Atmospheric Air Pressure (AP)",
                Formula = "A * 166.66 / 256 - 20"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Suzuki,
                Sid = 0x21,
                Pid = 0x08,
                ByteStartPosition = 24,
                MinimumValue = 0,
                MaximumValue = 15,
                Resolution = 0.078125,
                Scale = 5,
                DataType = DataTypes.Decimal,
                Unit = "V",
                Description = "Battery Voltage",
                Formula = "A * 20 / 256"
            });            
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Suzuki,
                Sid = 0x21,
                Pid = 0x08,
                ByteStartPosition = 26,
                MaximumValue = 6,
                DataType = DataTypes.Int,
                Description = "Gear"
            });            
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Suzuki,
                Sid = 0x21,
                Pid = 0x08,
                ByteStartPosition = 52,
                BitPosition = 16,
                DataType = DataTypes.Bool,
                Description = "Clutch"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Suzuki,
                Sid = 0x21,
                Pid = 0x08,
                ByteStartPosition = 52,
                BitPosition = 32,
                DataType = DataTypes.Bool,
                Description = "Starter Relais"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Suzuki,
                Sid = 0x21,
                Pid = 0x08,
                ByteStartPosition = 53,
                BitPosition = 2,
                DataType = DataTypes.Bool,
                Description = "Neutral",
                Negotiate = true,
                Formula = "!= 0"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Suzuki,
                Sid = 0x18,                
                ByteStartPosition = 6,
                NumberOfBytes = 9,
                DataType = DataTypes.Hex,
                Description = "Diagnostic Trouble Codes",
                Unit = "DTC"
            });

            InitResponseFailureCodes(ref calculations);
                        
        }

        private static void InitResponseFailureCodes(ref Data.Calculations calculations)
        {
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Suzuki,
                Sid = 0x7F - 0x40,
                Pid = 0xFF,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                FilterPosition = 6,
                Filter = 0x10,
                DataType = DataTypes.Hex,
                Unit = "Response Failure Code",
                Description = "General Reject"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Suzuki,
                Sid = 0x7F - 0x40,
                Pid = 0xFF,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                FilterPosition = 6,
                Filter = 0x11,
                DataType = DataTypes.Hex,
                Unit = "Response Failure Code",
                Description = "Service Not Supported"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Suzuki,
                Sid = 0x7F - 0x40,
                Pid = 0xFF,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                FilterPosition = 6,
                Filter = 0x12,
                DataType = DataTypes.Hex,
                Unit = "Response Failure Code",
                Description = "Sub Function Not Supported - Invalid Format"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Suzuki,
                Sid = 0x7F - 0x40,
                Pid = 0xFF,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                FilterPosition = 6,
                Filter = 0x13,
                DataType = DataTypes.Hex,
                Unit = "Response Failure Code",
                Description = "Message length or format incorrect"
            });
        }
    }
}
