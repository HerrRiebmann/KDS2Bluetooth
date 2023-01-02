using EcuEmulator.Data;

namespace EcuEmulator.Initialization
{
    public static class OBD2
    {
        private static ManufacturerEnum manufacturer = ManufacturerEnum.OBD2;        

        public static void Init(ref Data.Calculations calculations)
        {
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0x00,
                ByteStartPosition = 6,
                NumberOfBytes = 4,
                DataType = DataTypes.Binary,
                Unit = "X",
                Description = "PID´s supported 1-32"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0x20,
                ByteStartPosition = 6,
                NumberOfBytes = 4,
                DataType = DataTypes.Binary,
                Unit = "X",
                Description = "PID´s supported 33-65"
            });
            
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0x40,
                ByteStartPosition = 6,
                NumberOfBytes = 4,
                DataType = DataTypes.Binary,
                Unit = "X",
                Description = "PID´s supported 66-96"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0x60,
                ByteStartPosition = 6,
                NumberOfBytes = 4,
                DataType = DataTypes.Binary,
                Unit = "X",
                Description = "PID´s supported 97-128"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0x80,
                ByteStartPosition = 6,
                NumberOfBytes = 4,
                DataType = DataTypes.Binary,
                Unit = "X",
                Description = "PID´s supported 129-160"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0xA0,
                ByteStartPosition = 6,
                NumberOfBytes = 4,
                DataType = DataTypes.Binary,
                Unit = "X",
                Description = "PID´s supported 161-192"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0xC0,
                ByteStartPosition = 6,
                NumberOfBytes = 4,
                DataType = DataTypes.Binary,
                Unit = "X",
                Description = "PID´s supported 193-224"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0xE0,
                ByteStartPosition = 6,
                NumberOfBytes = 4,
                DataType = DataTypes.Binary,
                Unit = "X",
                Description = "PID´s supported 225-256"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0x01,
                ByteStartPosition = 6,
                NumberOfBytes = 4,                
                DataType = DataTypes.Hex,
                Unit = "byte encoded",
                Description = "Monitor status since DTCs cleared",
                Formula = ""
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0x02,
                ByteStartPosition = 6,
                NumberOfBytes = 2,
                DataType = DataTypes.Hex,
                Unit = "DTC",
                Description = "Freezed DTC"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0x03,
                ByteStartPosition = 6,
                NumberOfBytes = 2,
                DataType = DataTypes.Hex,
                Unit = "Int",
                Description = "Fuel system status",
                Formula = "0 Motor off, 1 Open loop, 2 closed loop, 4, 8, 16"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0x04,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                Resolution = 2.55,
                MinimumValue = 0,
                MaximumValue = 100,
                DataType = DataTypes.Decimal,
                DecimalPlaces = 3,
                Unit = "%",
                Description = "Calculated engine load ",
                Formula = "(100 /255) * A"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0x05,
                ByteStartPosition = 6,
                NumberOfBytes = 1,                
                Offset = -40,
                MinimumValue = -40,
                MaximumValue = 215,                
                DataType = DataTypes.Decimal,
                Unit = "°C",
                Description = "Engine coolant temperature",
                Formula = "A -40"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0x06,
                ByteStartPosition = 6,
                NumberOfBytes = 1,                
                MinimumValue = -100,
                MaximumValue = 100,
                Offset = -100,
                Resolution = 0.78125,
                DataType = DataTypes.Decimal,
                Unit = "%",
                Description = "Short term fuel trim—Bank 1",
                Formula = "(100 / 128) * A - 100"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0x07,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                MinimumValue = -100,
                MaximumValue = 100,
                Offset = -100,
                Resolution = 0.78125,
                DataType = DataTypes.Decimal,
                Unit = "%",
                Description = "Long term fuel trim—Bank 1",
                Formula = "(100 / 128) * A - 100"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0x08,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                MinimumValue = -100,
                MaximumValue = 100,
                Offset = -100,
                Resolution = 0.78125,
                DataType = DataTypes.Decimal,
                Unit = "%",
                Description = "Short term fuel trim—Bank 2",
                Formula = "(100 / 128) * A - 100"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0x09,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                MinimumValue = -100,
                MaximumValue = 100,
                Offset = -100,
                Resolution = 0.78125,
                DataType = DataTypes.Decimal,
                Unit = "%",
                Description = "Long term fuel trim—Bank 2",
                Formula = "(100 / 128) * A - 100"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0x0A,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                MinimumValue = 0,
                MaximumValue = 765,
                Resolution = 3,
                DataType = DataTypes.Int,
                Unit = "kPa",
                Description = "Fuel Pressure",
                Formula = "3 A"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0x0B,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                MaximumValue = 255,   
                DataType = DataTypes.Int,
                Unit = "kPa",
                Description = "Intake manifold absolute pressure",
                Formula = "A"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0x0C,
                ByteStartPosition = 6,
                NumberOfBytes = 2,
                Resolution = 0.25,
                MaximumValue = 16384,
                Scale = 2000,
                DataType = DataTypes.Int,
                Unit = "RPM",
                Description = "Engine Speed",
                Formula = "(A*256+B) / 4"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0x0D,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                MinimumValue = 0,
                MaximumValue = 255,
                DataType = DataTypes.Int,
                Unit = "km/h",
                Description = "Vehicle Speed",
                Formula = "A"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0x0E,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                MinimumValue = -64,
                MaximumValue = 63,
                Offset = -64,
                Resolution = 0.5,
                DataType = DataTypes.Decimal,
                DecimalPlaces = 1,
                Unit = "°",
                Description = "Timing advance (before TDC)",
                Formula = "A / 2 -64"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0x0F,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                Offset = -40,
                MinimumValue = -40,
                MaximumValue = 215,
                DataType = DataTypes.Decimal,
                DecimalPlaces = 1,
                Unit = "°C",
                Description = "Intake air temperature",
                Formula = "A -40"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0x11,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                MinimumValue = 0,
                MaximumValue = 100,
                Resolution = 0.255,
                DataType = DataTypes.Decimal,
                DecimalPlaces = 1,
                Unit = "%",
                Description = "Throttle position",
                Formula = "100/255 * A"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0x12,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                DataType = DataTypes.Binary,                
                Unit = "Bit",
                Description = "Commanded secondary air status",
                Formula = "Bit encoded"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0x13,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                DataType = DataTypes.Int,
                Unit = "",
                Description = "Oxygen sensors present (in 2 banks)",
                Formula = "[A0..A3] == Bank 1, Sensors 1-4. [A4..A7] == Bank 2... "
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0x14,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                MinimumValue = 0,
                MaximumValue = 1275,
                Resolution = 0.2,
                DataType = DataTypes.Decimal,
                DecimalPlaces = 1,
                Unit = "V",
                Description = "Oxygen Sensor 1",
                Formula = "A/200"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0x14,
                ByteStartPosition = 7,
                NumberOfBytes = 1,
                MinimumValue = -100,
                MaximumValue = 99,
                Resolution = 0.78125,
                Offset = -100,
                DataType = DataTypes.Decimal,
                DecimalPlaces = 2,
                Unit = "%",
                Description = "Oxygen Sensor 1",
                Formula = "100/128 * B - 100"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0x21,
                ByteStartPosition = 6,
                NumberOfBytes = 2,
                MinimumValue = 0,
                MaximumValue = 65535,
                DataType = DataTypes.Int,
                Unit = "km",
                Description = "Distance traveled with MIL on",
                Formula = "256A+B"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x01,
                Pid = 0x87,
                ByteStartPosition = 6,
                NumberOfBytes = 5,
                MaximumValue = 255,
                DataType = DataTypes.Hex,
                Unit = "kPa",
                Description = "Intake manifold absolute pressure",
                Formula = "?"
            });


            //Read DTC
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x04,
                ByteStartPosition = 6,
                NumberOfBytes = 0,
                DataType = DataTypes.YesNo,
                Description = "Clear Diagnostic Trouble Codes",
                Unit = "DTC"
            });
            //Clear DTC
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x03,
                ByteStartPosition = 6,
                NumberOfBytes = 9,
                DataType = DataTypes.Hex,
                Description = "Diagnostic Trouble Codes",
                Unit = "DTC"
            });

            //ECU ID
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x09,
                Pid = 0x00,
                ByteStartPosition = 6,
                NumberOfBytes = 4,
                DataType = DataTypes.Binary,
                Unit = "X",
                Description = "PID´s supported 1-11"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x09,
                Pid = 0x03,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                DataType = DataTypes.Int,
                Unit = "Id",
                Description = "Calibration ID message count for PID 04",
                Formula = "A * 4"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x09,
                Pid = 0x04,
                ByteStartPosition = 7,
                NumberOfBytes = 4,
                DataType = DataTypes.Text,
                Unit = "Id",
                Description = "ECU 1"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x09,
                Pid = 0x05,
                ByteStartPosition = 6,
                NumberOfBytes = 1,
                DataType = DataTypes.Int,
                Unit = "Id",
                Description = "Calibration verification numbers (CVN) message count for PID 06.",
                Formula = "A * 4"
            });
            calculations.Add(new Calculation
            {
                Manufacturer = manufacturer,
                Sid = 0x09,
                Pid = 0x06,
                ByteStartPosition = 6,
                NumberOfBytes = 4,
                DataType = DataTypes.Text,
                Unit = "Id",
                Description = "Calibration Verification Numbers (CVN)"                
            });
        }
    }
}
