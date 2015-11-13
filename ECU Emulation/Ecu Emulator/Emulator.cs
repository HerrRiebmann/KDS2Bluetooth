using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using Backend.Helper;
using EcuEmulator.Data;

namespace EcuEmulator
{
    public static class Emulator
    {
        //Return Values
        //10 GeneralReject 
        //11 ServiceNotSupported 
        //12 SubFunctionNotSupported-InvalidFormat 
        //21 Busy-RepeatRequest 
        //22 ConditionsNotCorrect or RequestSequenceError 
        //78 RequestCorrectlyReceived-ResponsePending 

        public enum ResponseType
        {
            Empty,
            NoCommonStartChar,
            Incomplete,
            WrongReceiver,
            ChecksumNotOk,
            RegisterUnknown,
            Error,
            Ok
        }

        private static EcuSetup _setup;
        public static EcuSetup Setup{
            get { return _setup ?? (_setup = new EcuSetup()); }
            set { _setup = value; }
        }

        private static RegisterList _register;
        public static RegisterList Register
        {
            get { return _register ?? InitRegister(); }
            set { _register = value; }
        }

        private static Status _status;
        public static Status Status
        {
            get { return _status ?? (_status = new Status()); }
            set { _status = value; }
        }
        
        /// <summary>
        /// Checks Request by RequestComplete()
        /// Answeres Request with AnswerRequest()
        /// </summary>
        /// <param name="data">Requst from Device</param>
        /// <returns>Response if Request was valid</returns>
        public static Result HandleRequest(string data)
        {
            //ToDo: Optimize Performance.
            //Starts With 80, Sender, Receiver, Size, Value(Size), Checksum
            //Starts With 81, Sender, Receiver, Size(129), Checksum
            //Otherwise uncompleted

            var result = new Result();

            if (data.Length < 2)
            {
                result.Type = ResponseType.Empty;
                return result;
            }
                

            if (data.StartsWith("80"))
            {
                if (data.Length > 10) //11chars
                {
                    var len = Int32.Parse(data.Split(' ')[3], NumberStyles.AllowHexSpecifier);
                    if (data.Length >= 11 + len*3 + 3) //11chars+len+checksum
                    {
                        var respType = CheckValuesComplete(ref data);
                        if (respType != ResponseType.Ok)
                        {
                            result.Type = respType;
                            return result;
                        }
                        result.Value = AnswerRequest(data);
                        if (String.IsNullOrEmpty(result.Value))
                        {
                            Status.LastErrorMessage = "Unknown Format Byte";
                            Messaging.WriteLine("Unknown Format Byte: " + data, Messaging.Types.Error, Messaging.Caller.Emulator);
                            result.Type = ResponseType.Error;
                            return result;
                        }
                        result.Type = ResponseType.Ok;
                        return result;
                    }
                    result.Type = ResponseType.Incomplete;
                    return result;
                }
                result.Type = ResponseType.Incomplete; 
                return result;
            }
            if (data.StartsWith("81"))
            {
                if (data.Length > 10) //11chars
                {
                    var len = Int32.Parse(data.Split(' ')[3], NumberStyles.AllowHexSpecifier);
                    if (len == 129) //11chars+len+checksum
                    {
                        var respType = CheckValuesComplete(ref data);
                        if (respType != ResponseType.Ok)
                        {
                            result.Type = respType;
                            return result;
                        }

                        result.Value = AnswerRequest(data);
                        if (String.IsNullOrEmpty(result.Value))
                        {
                            Status.LastErrorMessage = "Unknown Format Byte";
                            Messaging.WriteLine("Unknown Format Byte: " + data, Messaging.Types.Error, Messaging.Caller.Emulator);
                            result.Type = ResponseType.Error;
                            return result;
                        }
                        result.Type = ResponseType.Ok;
                        return result;
                    }
                    result.Type = ResponseType.Incomplete;
                    return result;
                }
                result.Type = ResponseType.Incomplete;
                return result;
            }
            
            //If Buffer overflows with several Messages
            //And do not begin with 80 or 81
            //Or do not contain 80 or 81 somewhere
            if (data.Contains("80") | data.Contains("81"))
            {
                if (data.Contains("80"))
                    return HandleRequest(data.Substring(data.IndexOf("80", StringComparison.CurrentCulture)));
                if (data.Contains("81"))
                    return HandleRequest(data.Substring(data.IndexOf("81", StringComparison.CurrentCulture)));
                result.Type = ResponseType.NoCommonStartChar;
                return result;
            }
            result.Type = ResponseType.Incomplete;
            return result;
        }

        public static string AnswerRequest(string data)
        {
            var hexValuesSplit = data.Split(' ');
            var msgSize = Int32.Parse(hexValuesSplit[3], NumberStyles.AllowHexSpecifier);
            //Init Request:
            if (msgSize.Equals(129))
            {
                Setup.EcuInitialized = true;
                Status.Init = true;
                var response = CreateInitResponse();
                Messaging.WriteLine(response, Messaging.Types.Outgoing, Messaging.Caller.Emulator, "ECU Initialized");
                return response;
            }

            if (!Setup.EcuInitialized)
            {
                Status.LastErrorMessage = "ECU not initialized!";
                Messaging.WriteLine("ECU not initialized!", Messaging.Types.Error);
                return CreateErrorResponse();
            }

            var mode = Converter.HexToByte(hexValuesSplit[4])[0];
            var value = Converter.HexToByte(hexValuesSplit[5])[0];

            foreach (var register in Register.Items)
            {
                if (msgSize == 2 && register.Mode.Equals(mode) && register.Parameter.Equals(value))
                {
                    return CreateAnswer(register);
                }
            }
            //Check and SET last request Date:
            if (!CheckLastRequestDate())
            {
                return CreateErrorResponse();
            }
            return CreateErrorResponse();
        }

        private static string CreateAnswer(Register register)
        {
            //80 F1 11 03 61 0B 01 F2
            //80 - Format/Adress
            //   F1 - Sender/Absender
            //      11 - Target/Empfänger
            //         03 - Lenght
            //            61 - ServiceID
            //              0B - Mode/PID/Befehl
            //                  01 - Value
            //                      F2 - Checksum
            var r = new Random();
            var i = 0;
            var calc = 0;
            //Result is valid + Requested Parameter
            var responseValues = Converter.ByteToHex(_setup.PositiveReply) + " " +
                                 Converter.ByteToHex(register.Parameter) + " ";

            //Return static Values (Except Start Diagnostic)
            if (register.PidType != PidTypes.StartDiagnostic && register.ResponseStaticValues != null && register.ResponseStaticValues.Length > 0)
            {
                foreach (var responseStaticValue in register.ResponseStaticValues)
                {
                    responseValues += Converter.ByteToHex(responseStaticValue) + " ";
                }
                //Remove last Space
                responseValues = responseValues.Remove(responseValues.Length - 1, 1);
            }
            else
            {
                switch (register.PidType)
                {
                    case PidTypes.StartDiagnostic:
                        Setup.DiagnosticsMode = true;
                        Status.Diagnostic = true;
                        //responseValues = "50 80";
                        //Don´t respond ServiceID:
                        responseValues = string.Empty;
                        foreach (var responseStaticValue in register.ResponseStaticValues)
                        {
                            responseValues += Converter.ByteToHex(responseStaticValue) + " ";
                        }
                        //Remove last Space
                        responseValues = responseValues.Remove(responseValues.Length - 1, 1);
                        break;
                    case PidTypes.EngineCoolantTemperature:
                    case PidTypes.IntakeAirTemperature:
                        //"61 C1 EA" == 116,2°
                        i = r.Next(register.ResponseMin, register.ResponseMax);
                        //calc = (int)((i + 48) / 1.6); // Convert to Fahrenheit
                        calc = (int) (i*1.6 + 48); // Convert to Fahrenheit
                        responseValues += calc.ToString("X").PadLeft(2, '0').ToUpper();
                        break;
                    case PidTypes.ThrottleOpening:
                        //   0 % "61 04 00 D8";
                        // 100 % "61 04 03 7f"
                        i = r.Next(register.ResponseMin, register.ResponseMax);
                        //responseValues += (i/100).ToString("X").PadLeft(2, '0').ToUpper();
                        //responseValues += " " + (i%100).ToString("X").PadLeft(2, '0').ToUpper();
                        //2 Byte:
                        responseValues += i.ToString("X").PadLeft(4, '0').ToUpper().Insert(2," ");
                        break;
                    case PidTypes.BarometricInletPressure:
                        i = r.Next(register.ResponseMin, register.ResponseMax);
                        responseValues += i.ToString("X").PadLeft(2, '0').ToUpper().Insert(2, " ");
                        responseValues += " ";
                        i = r.Next(register.ResponseMin, register.ResponseMax);
                        responseValues += i.ToString("X").PadLeft(2, '0').ToUpper().Insert(2, " ");
                        break;
                    case PidTypes.GearPosition:
                        // N "80 F1 11 03 61 0B 00 F1"
                        // 1 "80 F1 11 03 61 0B 01 F2"
                        i = r.Next(register.ResponseMin, register.ResponseMax);
                        responseValues += "0" + i;
                        break;
                    case PidTypes.Speed:
                        //Speedo 78 (156 / 2) KM
                        //responseValues += "00 9C";
                        i = r.Next(register.ResponseMin, register.ResponseMax);
                        calc = i;
                        calc *= 2;
                        responseValues += (calc/100).ToString("X").PadLeft(2, '0').ToUpper();
                        responseValues += " " + (calc%100).ToString("X").PadLeft(2, '0').ToUpper();
                        break;
                    case PidTypes.RotationsPerMinute:
                        //responseValues = "61 09 23 8F";
                        i = r.Next(register.ResponseMin, register.ResponseMax);
                        responseValues += (i/100).ToString("X").PadLeft(2, '0').ToUpper();
                        responseValues += " " + (i%100).ToString("X").PadLeft(2, '0').ToUpper();
                        break;
                    case PidTypes.TotalOperatingHours:
                        //Calc Total Seconds
                        i = r.Next(register.ResponseMin, register.ResponseMax);
                        responseValues += AddSpacesToString(i.ToString("X2"));
                        break;
                    case PidTypes.EcuId:
                        responseValues = "5A 32 31 31 37 35 2D 30 38 36 31";
                        break;
                    default:
                        //Do Something :)
                        //Calculate Min & Max Values
                        if (register.ResponseMin + register.ResponseMax > 0)
                        {
                            i = r.Next(register.ResponseMin, register.ResponseMax);
                            if (i > 256)
                            {
                                responseValues += (i/100).ToString("X").PadLeft(2, '0').ToUpper();
                                responseValues += " " + (i%100).ToString("X").PadLeft(2, '0').ToUpper();
                            }
                            else
                            {
                                responseValues += i.ToString("X").PadLeft(2, '0').ToUpper();
                            }
                        }
                        break;
                }
        }

        //Error if Diagnostic Mode is not started!
            if (!Setup.DiagnosticsMode)
            {
                Messaging.WriteLine("Diagnostic Mode not started!", Messaging.Types.Warning, Messaging.Caller.Emulator);
                Status.LastErrorMessage = "Diagnostic Mode not started!";
                return CreateErrorResponse();
            }
            
            //Check last request Date:
            if(!CheckLastRequestDate())
            {
                return CreateErrorResponse();
            }

            var responseValueSize = responseValues.Split(' ').Length;
            var response = String.Format("{0} {1:X} {2:X} {3} ", 80, Setup.SenderAdress, Setup.EcuAdress, responseValueSize.ToString(CultureInfo.InvariantCulture).PadLeft(2, '0').ToUpper());
            response += responseValues;
            response = AddChecksum(response);
            Status.LastResponse = response;
            Status.ConvertedResponse = i.ToString(CultureInfo.InvariantCulture);
            Status.RequestType = register.PidType.ToString();
            Messaging.WriteLine(response, Messaging.Types.Outgoing, Messaging.Caller.Emulator, String.Format("{0}: {1}", register.Description, i));
            CountMessagePerSecond();
            return response;
        }

        private static bool CheckLastRequestDate()
        {
            var milis = DateTime.Now.Subtract(Setup.LastRequest).TotalMilliseconds;
            if (milis > Setup.TimeOutMilis)
            {
                Messaging.WriteLine("Last Request older then " + (Setup.TimeOutMilis/1000) + " Seconds", Messaging.Types.Error, Messaging.Caller.Emulator);
                Status.LastErrorMessage = "Last Request older then " + (Setup.TimeOutMilis / 1000) + " Seconds";
                Setup.DiagnosticsMode = false;
                Status.Diagnostic = false;
                Setup.EcuInitialized = false;
                Status.Init = false;
                return false;
            }
            Setup.LastRequest = DateTime.Now;
            return true;
        }

        private static string CreateErrorResponse()
        {
            //"80 F1 11 03 7F 21 10 35"
            var errorResponse = String.Format("{0} {1:X} {2:X} {3} ", 80, Setup.SenderAdress, Setup.EcuAdress, Setup.ErrorSequence.Length.ToString(CultureInfo.InvariantCulture).PadLeft(2, '0').ToUpper());
            errorResponse += Converter.ByteToHex(Setup.ErrorSequence);
            errorResponse = AddChecksum(errorResponse);
            Messaging.WriteLine(errorResponse, Messaging.Types.Outgoing, Messaging.Caller.Emulator, String.Format("{0}: {1}", "Error", 0));
            return errorResponse;
        }

        private static string CreateInitResponse()
        {
            //"80 F1 11 03 C1 EA 8F BF"
            var initResponse = String.Format("{0} {1:X} {2:X} {3} ", 80, Setup.SenderAdress, Setup.EcuAdress, Setup.InitSequence.Length.ToString(CultureInfo.InvariantCulture).PadLeft(2, '0').ToUpper());
            initResponse += Converter.ByteToHex(Setup.InitSequence);
            initResponse = AddChecksum(initResponse);
            return initResponse;
        }
       
        private static ResponseType CheckValuesComplete(ref string data)
        {
            var hexValuesSplit = data.Split(' ');
            var counter = 0;
            var msgSize = 0;
            var values = new List<byte>();
            var startSequence = false;
            Status.Checksum = false;
            foreach (var hex in hexValuesSplit)
            {
                if (String.IsNullOrEmpty(hex))
                    continue;
                switch (counter++)
                {
                    case 0:
                        if(hex == "80")
                            Messaging.WriteLine("Adress Mode: " + hex, Messaging.Types.Info, Messaging.Caller.Emulator);
                        else if (hex == "81")
                        {
                            Messaging.WriteLine("Adress Mode - Start Sequence: " + hex, Messaging.Types.Info, Messaging.Caller.Emulator);
                            startSequence = true;
                        }
                        break;
                    case 1:
                        if (hex.Equals(Setup.SenderAdress.ToString(CultureInfo.InvariantCulture)))
                        {
                            Messaging.WriteLine("Recipient is Device", Messaging.Types.Warning, Messaging.Caller.Emulator);
                            //No message for us
                            return ResponseType.WrongReceiver;
                        }
                        Messaging.WriteLine("Recipient is ECU!", Messaging.Types.Info, Messaging.Caller.Emulator);
                        break;
                    case 2:
                        if (hex.Equals(Setup.EcuAdress.ToString(CultureInfo.InvariantCulture)))
                        {
                            Messaging.WriteLine("ECU is Sender!", Messaging.Types.Warning, Messaging.Caller.Emulator);
                            //Own message
                            return ResponseType.WrongReceiver;
                        }
                        Messaging.WriteLine("Device is Sender!", Messaging.Types.Info, Messaging.Caller.Emulator);
                        break;
                    case 3:
                        msgSize = Int32.Parse(hex, NumberStyles.AllowHexSpecifier);
                        if (startSequence && msgSize.Equals(129))
                            Messaging.WriteLine("ECU Initialization!", Messaging.Types.Info, Messaging.Caller.Emulator);
                        Messaging.WriteLine("Nachrichtengröße: " + msgSize, Messaging.Types.Debug, Messaging.Caller.Emulator);
                        break;
                    default:
                        //Store Data until Length reached
                        values.Add(Convert.ToByte(hex, 16));
                        if (counter - 4 >= msgSize)
                        {
                            //Kill all further info
                            if (data.Length > counter * 3 +2)//message + checksum
                                data = data.Substring(0, counter * 3 +2);
                            Status.CurrentRequest = data;
                            if(CheckRequest(values, data))
                                return ResponseType.Ok;
                            //Erst wenn auch Checksum übertragen ist
                            return ResponseType.Incomplete;
                        }
                        //If StartSequence ignore MessageSize (129) calc CheckSum (0x04)
                        if (startSequence)
                        {
                            data = data.Substring(0, counter * 3 - 1);
                            if (CheckChecksum(data))
                            {
                                Messaging.WriteLine(data, Messaging.Types.Incoming, Messaging.Caller.Emulator, "ECU Initialization");
                                return ResponseType.Ok;
                            }
                            return ResponseType.ChecksumNotOk;
                        }
                        break;
                } 
            }
            return ResponseType.Incomplete;
        }

        private static bool CheckRequest(List<byte> values, string data)
        {
            //80 11 F1 02 21 06 AB
            //80 - Format/Adress
            //   11 - Target/Empfänger
            //      F1 - Sender/Absender
            //         02 - Lenght
            //            21 - Mode/ServiceId/Befehl
            //               06 - Parameter/Register
            //                  AB - Checksum

            if (!CheckChecksum(data))
            {
                Messaging.WriteLine(data, Messaging.Types.Error, Messaging.Caller.Emulator, "Checksum false!");
                return false;
            }

            foreach (var register in Register.Items)
            {
                if (register.Mode.Equals(values[0]) && register.Parameter.Equals(values[1]))
                {
                    Messaging.WriteLine(data, Messaging.Types.Incoming, Messaging.Caller.Emulator, String.Format("Register {0} found!", register.Description));
                    return true;
                }
            }
            //Also answer unknown Register:
            //return false;
            Messaging.WriteLine(data, Messaging.Types.Incoming, Messaging.Caller.Emulator, "Unknown Register!");
            return true;
        }

        public static bool CheckChecksum(string data)
        {
            var hexValuesSplit = data.Split(' ');
            var len = hexValuesSplit.Length;
            var hexByte = new byte[len-1];
            var i = 0;
            foreach (var hex in hexValuesSplit)
            {
                //Skip Checksum
                if (i >= len -1)
                    continue;
                
                hexByte[i++] = Convert.ToByte(hex, 16);
            }
            var b = CalcCheckSum(hexByte);
            var bStr = Convert.ToString(b, 16).PadLeft(2, '0').ToUpper();
            var ok = bStr.Equals(hexValuesSplit[len - 1]);
            Status.Checksum = ok;
            return ok;
        }

        public static string AddChecksum(string data)
        {
            var hexValuesSplit = data.Split(' ');
            var len = hexValuesSplit.Length;
            var hexByte = new byte[len];
            var i = 0;
            foreach (var hex in hexValuesSplit)
            {
                //Skip Checksum
                if (String.IsNullOrEmpty(hex))
                    continue;

                hexByte[i++] = Convert.ToByte(hex, 16);
            }
            byte b = CalcCheckSum(hexByte);
            var bStr = Convert.ToString(b, 16).PadLeft(2, '0').ToUpper();
            data += " " + bStr;
            return data;
        }

        public static byte CalcCheckSum(IEnumerable<byte> hexValues)
        {
            return hexValues.Aggregate(new byte(), (current, hexValue) => (byte) (current + hexValue));
        }

        public static string AddSpacesToString(string valueField)
        {
            var ins = valueField.ToCharArray();
            var length = ins.Length + (ins.Length / 2);
            if (ins.Length % 2 == 0)
            {
                length--;
            }
            var outs = new char[length];

            var i = length - 1;
            var j = ins.Length - 1;
            var k = 0;
            do
            {
                if (k == 2)
                {
                    outs[i--] = ' ';
                    k = 0;
                }
                else
                {
                    outs[i--] = ins[j--];
                    k++;
                }
            }
            while (i >= 0);

            return new string(outs);
        }

        private static DateTime _lastRequest = DateTime.MinValue;
        private static int _counter;
        private static void CountMessagePerSecond()
        {
            if (_lastRequest == DateTime.MinValue)
                _lastRequest = DateTime.Now;
            _counter++;
            var duration = DateTime.Now.Subtract(_lastRequest);
            if (duration.TotalSeconds >= 1)
            {
                //Wenn die Sekunden aber älter als 1,1 Sekunden her ist, nicht mehr zählen:
                if (duration.TotalMilliseconds > 1100)
                    _counter--;
                _status.MessagesPerSecond = _counter;
                _counter = 0;
                _lastRequest = DateTime.Now;
            }
        }

        public static RegisterList InitRegister()
        {
            var list = new RegisterList();
            list.Items.Add(new Register(PidTypes.StartDiagnostic, 0x10, 0x80, "Start Diagnosis", new byte[]{0x50, 0x80}));

            list.Items.Add(new Register(PidTypes.PidList1to32, 0x21, 0x00, "Supported PIDs 1-32", new byte[] { 0xDF, 0xF7, 0x87, 0x87 }));
            list.Items.Add(new Register(PidTypes.PidList33to65, 0x21, 0x20, "Supported PIDs 33-65", new byte[] { 0xC3, 0xE5, 0xE0, 0x1F }));
            list.Items.Add(new Register(PidTypes.PidList66to96, 0x21, 0x40, "Supported PIDs 66-96", new byte[] { 0xF0, 0x00, 0x14, 0x3F }));
            list.Items.Add(new Register(PidTypes.PidList97to128, 0x21, 0x60, "Supported PIDs 97-128", new byte[] { 0xFF, 0x07, 0x00, 0x01 }));
            list.Items.Add(new Register(PidTypes.PidList129toX, 0x21, 0x80, "Supported PIDs 128-XXX", new byte[] { 0x20, 0x00, 0x00, 0x21 }));

            list.Items.Add(new Register(PidTypes.StarterSwitch, 0x21, 0x02, "Starter Switch", 0, 1, 1));
            list.Items.Add(new Register(PidTypes.ThrottleOpening, 0x21, 0x04, "Throttle Pos. Sensor", 201, 405, 1));
            list.Items.Add(new Register(PidTypes.BarometricInletPressure, 0x21, 0x05, "Intake Air Pressure", 161, 210, 1)); //2 Byte value Double precision
            list.Items.Add(new Register(PidTypes.EngineCoolantTemperature, 0x21, 0x06, "Water Temperature", 50, 120, 1));
            list.Items.Add(new Register(PidTypes.IntakeAirTemperature, 0x21, 0x07, "Intake Air Temperature", -20, 45, 1));
            //list.Items.Add(new Register(PidTypes.AtmosphericPressure, 0x21, 0x08, "Atmospheric Pressure", 0, 100, 1));
            //ToDo: dynamic
            list.Items.Add(new Register(PidTypes.AtmosphericPressure, 0x21, 0x08, "Atmospheric Pressure", 200,210,1));//206 / 2 = kPa = 38 Meter above NN
            list.Items.Add(new Register(PidTypes.RotationsPerMinute, 0x21, 0x09, "Rotations per minute", 1000, 13500, 1));
            list.Items.Add(new Register(PidTypes.BatteryVoltage, 0x21, 0x0A, "Battery Voltage", 155, 185, 1));//12 - 14,5 V
            list.Items.Add(new Register(PidTypes.GearPosition, 0x21, 0x0B, "Gear", 0, 6, 1));
            list.Items.Add(new Register(PidTypes.Speed, 0x21, 0x0C, "Speedometer", 0, 240, 1));

            //Without Service ID (not empty, without)
            //list.Items.Add(new Register(PidTypes.EcuId, 0x00, 0x1A, "ECU ID", new byte[] { 0x50, 0x80 }));
            
            //Total operating Hours between: 39h 54min 6sec - 71h 8min 54sec
            list.Items.Add(new Register(PidTypes.TotalOperatingHours, 0x21, 0x44, "Total Operating Hours", 143646, 256134, 1));

            list.Items.Add(new Register(PidTypes.SubThrottleValveOperatingAngle, 0x21, 0x5B, "Sub-throttle valve operating angle", 81, 189, 1)); //Max unknown
            list.Items.Add(new Register(PidTypes.VehicleDownSensorVoltage, 0x21, 0x5E, "Vehicle-down sensor voltage", 200, 220, 1)); //Active unknown
            list.Items.Add(new Register(PidTypes.InternalControlVoltage2, 0x21, 0x5F, "Internal control #2 Voltage", 155, 186, 1));
            list.Items.Add(new Register(PidTypes.InternalControlVoltage3, 0x21, 0x61, "Internal control #3 Voltage", new byte[] { 0x00}));
            list.Items.Add(new Register(PidTypes.EmergencyStop, 0x21, 0x62, "Emergency Stop", new byte[]{0x00, 0x00}));
            list.Items.Add(new Register(PidTypes.FuelCutMode, 0x21, 0x6A, "Fuel cut Mode (coasting/enginebreaking)", 0, 1, 1));
            list.Items.Add(new Register(PidTypes.EngineStartMode, 0x21, 0x6B, "Engine starting mode", 0, 1, 1));
            return list;
        }

        public static Transfer Interpret(string hexValues)
        {
            var transfer = new Transfer();

            var hexValuesSplit = hexValues.Split(' ');
            var counter = 0;
            foreach (var hex in hexValuesSplit)
            {
                if (String.IsNullOrEmpty(hex))
                    continue;
                switch (counter++)
                {
                    case 0:
                        transfer.Mode = Converter.HexToByte(hex)[0];
                        break;
                    case 1:
                        transfer.Receiver = Converter.HexToByte(hex)[0];
                        break;
                    case 2:
                        transfer.Sender = Converter.HexToByte(hex)[0];
                        break;
                    case 3:
                        transfer.Length = Converter.HexToByte(hex)[0];
                        break;
                    default:
                        //StartSequence:
                        if (transfer.Length.Equals(129))
                        {
                            if(counter==4)
                                transfer.Values.Add(Convert.ToByte(hex, 16));
                            if(counter==5)
                                transfer.Checksum = Convert.ToByte(hex, 16);
                            continue;
                        }
                        //Regular Message:
                        if (counter - 4 > transfer.Length)
                        {
                            transfer.Checksum = Convert.ToByte(hex, 16);
                            return transfer;
                        }
                        transfer.Values.Add(Convert.ToByte(hex, 16));
                        break;
                }
            }
            return transfer;
        }
        
    }
}
