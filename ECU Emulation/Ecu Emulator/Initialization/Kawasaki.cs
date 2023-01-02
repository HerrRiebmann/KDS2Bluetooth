using EcuEmulator.Data;

namespace EcuEmulator.Initialization
{
    public static class Kawasaki
    {
        public static void Init(ref Data.Calculations calculations)
        {
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Kawasaki,
                Sid = 0x13,
                Pid = 0x01,
                ByteStartPosition = 6,
                NumberOfBytes = 2,
                DataType = DataTypes.Hex,
                Unit = "DTC",
                Description = "Current Error 1"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Kawasaki,
                Sid = 0x13,
                Pid = 0x02,
                ByteStartPosition = 6,
                NumberOfBytes = 2,
                DataType = DataTypes.Hex,
                Unit = "DTC",
                Description = "Current Error 2"
            });            
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Kawasaki,
                Sid = 0x1A,
                Pid = 0xFF,
                ByteStartPosition = 5,
                NumberOfBytes = 10,
                DataType = DataTypes.Text,
                Unit = "Id",
                Description = "ECU"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Kawasaki,
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
                Manufacturer = ManufacturerEnum.Kawasaki,
                Sid = 0x82,
                Pid = 0x00,
                ByteStartPosition = 4,
                NumberOfBytes = 1,
                DataType = DataTypes.Hex,
                Unit = "",
                Description = "Stop Communication Request",
                Formula = "Inverted 0x82"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Kawasaki,
                Sid = 0x10,
                Pid = 0x80,
                ByteStartPosition = 5,
                NumberOfBytes = 1,
                DataType = DataTypes.Hex,
                Description = "Start Diagnostic Session"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Kawasaki,
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
                Manufacturer = ManufacturerEnum.Kawasaki,
                Sid = 0x14,
                Pid = 0xFF,
                ByteStartPosition = 4,
                NumberOfBytes = 1,
                DataType = DataTypes.Hex,
                Unit = "",
                Description = "Clear Diagnostic"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Kawasaki,
                Sid = 0x21,
                Pid = 0x00,
                ByteStartPosition = 6,
                NumberOfBytes = 4,
                DataType = DataTypes.Binary,
                Unit = "X",
                Description = "PID´s supported 1-32"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Kawasaki,
                Sid = 0x21,
                Pid = 0x01,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                DataType = DataTypes.Int,
                Unit = "Enum",
                Description = "OBD Conform",
                Formula = "5 = Not OBD compliant"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Kawasaki,
                Sid = 0x21,
                Pid = 0x02,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                DataType = DataTypes.Bool,
                Unit = "Bool",
                Description = "Starter Switch"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Kawasaki,
                Sid = 0x21,
                Pid = 0x03,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                DataType = DataTypes.Bool,
                Unit = "Bool",
                Description = "Engine Emergency Stop"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Kawasaki,
                Sid = 0x21,
                Pid = 0x04,
                ByteStartPosition = 6,
                NumberOfBytes = 2,
                Resolution = 0.004882813,
                MaximumValue = 5,
                DataType = DataTypes.Decimal,
                DecimalPlaces = 3,
                Unit = "V",
                Description = "Throttle Position Sensor",
                Formula = "(A*256+B) * 5 / 4 / 256"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Kawasaki,
                Sid = 0x21,
                Pid = 0x05,
                ByteStartPosition = 6,
                NumberOfBytes = 2,
                Resolution = 0.01449585,
                MaximumValue = 800,
                Scale = 200,
                DataType = DataTypes.Decimal,
                Unit = "mmHg",
                Description = "Air Pressure (Inlet / Manifold)",
                Formula = "(A*256+B) * 190 * 5 / 256"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Kawasaki,
                Sid = 0x21,
                Pid = 0x06,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                Resolution = 0.625,
                Offset = -30,
                MinimumValue = -50,
                MaximumValue = 150,
                Scale = 50,
                DataType = DataTypes.Decimal,
                Unit = "°C",
                Description = "Water Temperature",
                Formula = "A * 160 / 256"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Kawasaki,
                Sid = 0x21,
                Pid = 0x07,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                Resolution = 0.625,
                Offset = -30,
                MinimumValue = -40,
                MaximumValue = 60,
                Scale = 20,
                DataType = DataTypes.Decimal,
                Unit = "°C",
                Description = "Inlet Air Temperature",
                Formula = "A * 160 / 256"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Kawasaki,
                Sid = 0x21,
                Pid = 0x08,
                ByteStartPosition = 6,
                NumberOfBytes = 2,
                Resolution = 0.01449585,
                MaximumValue = 800,
                Scale = 200,
                DataType = DataTypes.Decimal,
                Unit = "mmHg",
                //Unit = "kPa", //ToDo: calc to kPa
                Description = "Atmospheric Pressure",
                Formula = "(A*256+B) * 190 * 5 / 256"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Kawasaki,
                Sid = 0x21,
                Pid = 0x09,
                ByteStartPosition = 6,
                NumberOfBytes = 2,
                Resolution = 0.390625,
                MaximumValue = 10000,
                Scale = 2000,
                DataType = DataTypes.Int,
                Unit = "RPM",
                Description = "Engine Speed",
                Formula = "(A*256+B) * 100 / 256"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Kawasaki,
                Sid = 0x21,
                Pid = 0x0A,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                Resolution = 0.078125,
                MaximumValue = 15,
                Scale = 5,
                DataType = DataTypes.Decimal,
                Unit = "V",
                Description = "Battery Voltage",
                Formula = "A * 20 / 256"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Kawasaki,
                Sid = 0x21,
                Pid = 0x0B,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                MaximumValue = 6,
                DataType = DataTypes.Int,
                Description = "Gear"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Kawasaki,
                Sid = 0x21,
                Pid = 0x0C,
                ByteStartPosition = 6,
                NumberOfBytes = 2,
                Resolution = 0.5,
                MaximumValue = 300,
                Scale = 50,
                DataType = DataTypes.Int,
                Unit = "km/h",
                Description = "Vehicle Speed",
                Formula = "(A*256+B) * 1 / 2"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Kawasaki,
                Sid = 0x21,
                Pid = 0x20,
                ByteStartPosition = 6,
                NumberOfBytes = 4,
                DataType = DataTypes.Binary,
                Unit = "X",
                Description = "PID´s supported 33-65"
            });

            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Kawasaki,
                Sid = 0x21,
                Pid = 0x40,
                ByteStartPosition = 6,
                NumberOfBytes = 4,
                DataType = DataTypes.Binary,
                Unit = "X",
                Description = "PID´s supported 66-96"
            });            
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Kawasaki,
                Sid = 0x21,
                Pid = 0x5B,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                Resolution = 0.01953125,
                MaximumValue = 5,
                Scale = 1,
                DataType = DataTypes.Decimal,
                DecimalPlaces = 3,
                Unit = "V",
                Description = "Sub-Throttle Opening Angle",
                Formula = "A * 5 / 256"
            });            
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Kawasaki,
                Sid = 0x21,
                Pid = 0x60,
                ByteStartPosition = 6,
                NumberOfBytes = 4,
                DataType = DataTypes.Binary,
                Unit = "X",
                Description = "PID´s supported 97-128"
            });            
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Kawasaki,
                Sid = 0x21,
                Pid = 0x80,
                ByteStartPosition = 6,
                NumberOfBytes = 4,
                DataType = DataTypes.Binary,
                Unit = "X",
                Description = "PID´s supported 129-160"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Kawasaki,
                Sid = 0x21,
                Pid = 0xA0,
                ByteStartPosition = 6,
                NumberOfBytes = 4,
                DataType = DataTypes.Binary,
                Unit = "X",
                Description = "PID´s supported 161-192"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Kawasaki,
                Sid = 0x21,
                Pid = 0xC0,
                ByteStartPosition = 6,
                NumberOfBytes = 4,
                DataType = DataTypes.Binary,
                Unit = "X",
                Description = "PID´s supported 193-224"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Kawasaki,
                Sid = 0x21,
                Pid = 0xE0,
                ByteStartPosition = 6,
                NumberOfBytes = 4,
                DataType = DataTypes.Binary,
                Unit = "X",
                Description = "PID´s supported 225-256"
            });
            
            InitResponseFailureCodes(ref calculations);
            InitAbs(ref calculations);
        }

        private static void InitResponseFailureCodes(ref Data.Calculations calculations)
        {
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.Kawasaki,
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
                Manufacturer = ManufacturerEnum.Kawasaki,
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
                Manufacturer = ManufacturerEnum.Kawasaki,
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
                Manufacturer = ManufacturerEnum.Kawasaki,
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

        private static void InitAbs(ref Data.Calculations calculations)
        {
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.KawasakiABS,
                Sid = 0x1A,
                Pid = 0xFF,
                ByteStartPosition = 5,
                NumberOfBytes = 10,
                DataType = DataTypes.Text,
                Unit = "Id",
                Description = "ECU"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.KawasakiABS,
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
                Manufacturer = ManufacturerEnum.KawasakiABS,
                Sid = 0x82,
                Pid = 0x00,
                ByteStartPosition = 4,
                NumberOfBytes = 1,
                DataType = DataTypes.Hex,
                Unit = "",
                Description = "Stop Communication Request",
                Formula = "Inverted 0x82"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.KawasakiABS,
                Sid = 0x10,
                Pid = 0x80,
                ByteStartPosition = 5,
                NumberOfBytes = 1,
                DataType = DataTypes.Hex,
                Description = "Start Diagnostic Session"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.KawasakiABS,
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
                Manufacturer = ManufacturerEnum.KawasakiABS,
                Sid = 0x14,
                Pid = 0xFF,
                ByteStartPosition = 4,
                NumberOfBytes = 1,
                DataType = DataTypes.Hex,                
                Description = "Clear Diagnostic"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.KawasakiABS,
                Sid = 0x21,
                Pid = 0x00,
                ByteStartPosition = 6,
                NumberOfBytes = 4,
                DataType = DataTypes.Binary,
                Unit = "X",
                Description = "PID´s supported 1-32"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.KawasakiABS,
                Sid = 0x21,
                Pid = 0x01,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                DataType = DataTypes.Int,
                Unit = "Enum",
                Description = "OBD Conform",
                Formula = "5 = Not OBD compliant"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.KawasakiABS,
                Sid = 0x21,
                Pid = 0x02,                
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                Resolution = 0.078125,
                MaximumValue = 15,
                Scale = 5,
                DataType = DataTypes.Decimal,
                Unit = "V",
                Description = "ABS Voltage",
                Formula = "A * 20 / 256"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.KawasakiABS,
                Sid = 0x21,
                Pid = 0x03,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                DataType = DataTypes.Int,
                Unit = "km/h?",
                Description = "ABS Frontwheel",
                Formula = "A"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.KawasakiABS,
                Sid = 0x21,
                Pid = 0x04,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                DataType = DataTypes.Int,
                Unit = "km/h?",
                Description = "ABS Rearwheel",
                Formula = "A"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.KawasakiABS,
                Sid = 0x21,
                Pid = 0x05,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                DataType = DataTypes.Hex,
                Unit = "No?",
                Description = "Number of DTC",
                Formula = "A"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.KawasakiABS,
                Sid = 0x21,
                Pid = 0x20,
                ByteStartPosition = 6,
                NumberOfBytes = 4,
                DataType = DataTypes.Binary,
                Unit = "X",
                Description = "PID´s supported 33-65"
            });
            
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.KawasakiABS,
                Sid = 0x21,
                Pid = 0x40,
                ByteStartPosition = 6,
                NumberOfBytes = 4,
                DataType = DataTypes.Binary,
                Unit = "X",
                Description = "PID´s supported 66-96"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.KawasakiABS,
                Sid = 0x21,
                Pid = 0x60,
                ByteStartPosition = 6,
                NumberOfBytes = 4,
                DataType = DataTypes.Binary,
                Unit = "X",
                Description = "PID´s supported 97-128"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.KawasakiABS,
                Sid = 0x21,
                Pid = 0x80,
                ByteStartPosition = 6,
                NumberOfBytes = 4,
                DataType = DataTypes.Binary,
                Unit = "X",
                Description = "PID´s supported 129-160"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.KawasakiABS,
                Sid = 0x21,
                Pid = 0xA0,
                ByteStartPosition = 6,
                NumberOfBytes = 4,
                DataType = DataTypes.Binary,
                Unit = "X",
                Description = "PID´s supported 161-192"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.KawasakiABS,
                Sid = 0x21,
                Pid = 0xC0,
                ByteStartPosition = 6,
                NumberOfBytes = 4,
                DataType = DataTypes.Binary,
                Unit = "X",
                Description = "PID´s supported 193-224"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.KawasakiABS,
                Sid = 0x21,
                Pid = 0xE0,
                ByteStartPosition = 6,
                NumberOfBytes = 4,
                DataType = DataTypes.Binary,
                Unit = "X",
                Description = "PID´s supported 225-256"
            });

            InitResponseFailureCodesABS(ref calculations);
        }

        private static void InitResponseFailureCodesABS(ref Data.Calculations calculations)
        {
            calculations.Add(new Calculation
            {
                Manufacturer = ManufacturerEnum.KawasakiABS,
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
                Manufacturer = ManufacturerEnum.KawasakiABS,
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
                Manufacturer = ManufacturerEnum.KawasakiABS,
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
                Manufacturer = ManufacturerEnum.KawasakiABS,
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
