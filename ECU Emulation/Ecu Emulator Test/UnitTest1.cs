using System;
using System.Globalization;
using System.Threading;
using Backend.Data;
using EcuEmulator.Data;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using SerialComPort;
using SerialComPort.Data;

namespace EcuEmulatorTest
{
    [TestClass]
    public class UnitTest1
    {
        [TestMethod]
        public void TestStockRequests()
        {
            InitializeEcu();

            CheckRequests();
        }

        [TestMethod]
        public void TestRequestTimeout()
        {
            InitializeEcu();

            GenerateRequestTimeout();
        }

        [TestMethod]
        public void TestSpecificRegister()
        {
            InitializeEcu();

            RequestSpecificRegister();
        }

        [TestMethod]
        public void CheckChecksum()
        {

            var dataWithChechsum3 = EcuEmulator.Emulator.AddChecksum("80 12 F1 06 A5 06 00 00 00 00");
            Console.WriteLine(dataWithChechsum3);

            var dataWithChechsum2 = EcuEmulator.Emulator.AddChecksum("80 11 F1 02 01 0C");
            Console.WriteLine(dataWithChechsum2);

            var data = "80 F1 11 03 41 1E 04";
            var expectedChecksum = " E8";
            
            var dataWithChechsum = EcuEmulator.Emulator.AddChecksum(data);
            var result = EcuEmulator.Emulator.CheckChecksum(dataWithChechsum);
            Assert.IsTrue(result, "Checksum ist fehlerhaft!");
            Assert.AreEqual(dataWithChechsum, data + expectedChecksum, "Werte stimmen nicht überein");
        }

        private void RequestSpecificRegister()
        {
            var result = CheckPid("44");
            //Remove: 21 44
            result.Values.RemoveRange(0,2);
            var bytes = result.Values.ToArray();
            var bytesHex = Backend.Helper.Converter.ByteToHex(bytes);
            bytesHex = bytesHex.Replace(" ", "");
            Int64 operatingHours = 0;
            try
            {
                operatingHours = Int64.Parse(bytesHex, NumberStyles.HexNumber);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
            
            var dt = TimeSpan.FromSeconds(operatingHours);
            Console.WriteLine("OperatingSeconds: {0}\tD: {1}H: {2} M:{3} S: {4}", operatingHours, dt.Days, dt.TotalHours, dt.Minutes, dt.Seconds);

        }

        private EcuEmulator.Data.Transfer CheckPid(string pid)
        {
            var request = "80 11 F1 ";
            request += "02 21 ";
            request += pid;
            request = EcuEmulator.Emulator.AddChecksum(request);
            var result = EcuEmulator.Emulator.HandleRequest(request);
            var msg = EcuEmulator.Emulator.Interpret(result.Value);
            Assert.IsTrue(msg.TestChecksum);
            return msg;
        }

        [TestMethod]
        public void InitSetup()
        {
            //Create XML:
            var setup = EcuSetup.Load();

            setup.Save();

            //Create Json
            var setupJ = EcuSetup.Load(Serializer.ConversionTypes.Json);

            setupJ.Save(Serializer.ConversionTypes.Json);

            //Create Bin:
            var setupB = EcuSetup.Load(Serializer.ConversionTypes.Yaml);

            setupB.Save(Serializer.ConversionTypes.Yaml);
        }

        public void InitializeEcu()
        {
            //Init Ecu:
            var result = EcuEmulator.Emulator.HandleRequest("81 11 F1 81 04");
            Assert.AreEqual(result.Value, "80 F1 11 03 C1 EA 8F BF");
            
            //Start Diagnosis:
            result = EcuEmulator.Emulator.HandleRequest("80 11 F1 02 10 80 14");
            Assert.AreEqual(result.Value, "80 F1 11 02 50 80 54");
        }
        
        public void CheckRequests()
        {
            //ToDo: Test response with dynamic values
            ////Gear Request
            //var result = EcuEmulator.Emulator.HandleRequest("80 11 F1 02 21 0B B0");
            //var msg =  EcuEmulator.Emulator.Interpret(result.Value);
            //Assert.IsTrue(msg.TestChecksum);
            //Assert.AreEqual(result.Value, "80 F1 11 03 61 0B 00 F1");

            ////Speed Request
            //result = EcuEmulator.Emulator.HandleRequest("80 11 F1 02 21 0C B1");
            //Assert.AreEqual(result.Value, "80 F1 11 04 61 0C 00 9C 8F");

            ////RPM Request
            //result = EcuEmulator.Emulator.HandleRequest("80 11 F1 02 21 09 AE");
            //Assert.AreEqual(result.Value, "80 F1 11 04 61 09 23 8F A2");

            ////Temp Request
            //result = EcuEmulator.Emulator.HandleRequest("80 11 F1 02 21 06 AB");
            //Assert.AreEqual(result.Value, "80 F1 11 03 61 C1 EA 91");

            ////Throttle
            //result = EcuEmulator.Emulator.HandleRequest("80 11 F1 02 21 04 A9");
            //Assert.AreEqual(result.Value, "80 F1 11 04 61 04 00 D8 C3");

            ////Unknown Register - Error as result
            //result = EcuEmulator.Emulator.HandleRequest("80 11 F1 02 21 50 A9");
            //Assert.AreEqual(result.Value, "80 F1 11 03 7F 21 10 35");

            //Suzuki
            var result = EcuEmulator.Emulator.HandleRequest("80 12 F1 02 21 08 AE");
            //ECU ID 12 Suzi, 11 Kawa
            Assert.AreEqual(result.Value, "80 F1 12 34 61 08 01 16 55 A0 00 00 00 FF FF FF FF 00 00 39 B9 4D 4C B9 00 00 00 FF FF FF FF 00 00 00 00 00 00 00 00 FF FF 40 40 40 40 FF EF FF FF FF 00 00 04 28 FF FF 7A");
        }

        public void GenerateRequestTimeout()
        {
            //Gear Request
            var result = EcuEmulator.Emulator.HandleRequest("80 11 F1 02 21 0B B0");
            //Compare without values, only:
            //Assert.AreEqual(response, "80 F1 11 03 61 0B 00 F1");
            Assert.AreEqual(result.Value.Substring(0, result.Value.Length - 6), "80 F1 11 03 61 0B");
            Assert.IsTrue(EcuEmulator.Emulator.CheckChecksum(result.Value));

            //Timeout 2,1 Seconds
            Thread.Sleep(2100);

            //Speed Request - Error expected
            result = EcuEmulator.Emulator.HandleRequest("80 11 F1 02 21 0C B1");
            Assert.AreEqual(result.Value, "80 F1 11 03 7F 21 10 35");
        }

        [TestMethod]
        public void TestComPortInit()
        {
            ComPortInitialization();
        }

        [TestMethod]
        public void TestComPort()
        {
            var setupJ = ComSetup.Load(Serializer.ConversionTypes.Json);
            CheckSetup(ref setupJ);
            setupJ.Save(Serializer.ConversionTypes.Json);

            var serial = new Serial();
            serial.DataReceived += serial_DataReceived;
            Assert.IsTrue(serial.Open(setupJ), "Open Com Port " + setupJ.PortName);
            
            var i = 0;
            while (i++ <= 200)
            {
                Thread.Sleep(10);
                if (!String.IsNullOrEmpty(_response))
                    if(_response.Length >= 14)
                    {
                        Assert.AreEqual("81 11 F1 81 04", _response, "Init Request");
                        _response = string.Empty;
                        break;
                    }
            }

            //Send Init Response
            Assert.IsTrue(serial.Send("80 F1 11 03 C1 EA 8F BF"), "Send Init");
            
            i = 0;
            while (i++ <= 200)
            {
                Thread.Sleep(10);
                if (!String.IsNullOrEmpty(_response))
                    if (_response.Length >= 20)
                    {
                        Assert.AreEqual("80 11 F1 02 10 80 14", _response, "Enable Diagnostic Request");
                        _response = string.Empty;
                        break;
                    }
            }
            
            //Send Enable Diagnostic Response
            Assert.IsTrue(serial.Send("80 F1 11 02 50 80 54"), "Enable Diagnostic Response");

            //Wait for Gear request...
            i = 0;
            while (i++ <= 200)
            {
                Thread.Sleep(10);
                if (!String.IsNullOrEmpty(_response))
                    if (_response.Length >= 20)
                    {
                        Assert.AreEqual("80 11 F1 02 21 0B B0", _response, "Gear Request");
                        _response = string.Empty;
                        break;
                    }
            }
            //Send Gear Response
            Assert.IsTrue(serial.SendLine("80 F1 11 03 61 0B 00 F1"), "Send Neutral Gear Response");

            //Wait a bit..
            while (i++ <= 200)
                Thread.Sleep(10);

            Assert.IsTrue(serial.Close(), "Closing Com Port");
        }

        private volatile string _response;

        void serial_DataReceived(object sender, SerialComPort.Events.DataEventArgs e)
        {
            _response += String.IsNullOrEmpty(_response) ? e.Data : " " + e.Data;
            //Assert.AreEqual(e.Data, "81 11 F1 81 04");
            Console.WriteLine("{0}:\t{1}", "Incoming", e.Data);
        }

        public void ComPortInitialization()
        {
            //XML
            var setup = ComSetup.Load();
            CheckSetup(ref setup);
            setup.Save();

            //JSon
            var setupJ = ComSetup.Load(Serializer.ConversionTypes.Json);
            CheckSetup(ref setupJ);
            setupJ.Save(Serializer.ConversionTypes.Json);
        }

        public void CheckSetup(ref ComSetup setup)
        {
            if (!String.IsNullOrEmpty(setup.BaudRate) && !String.IsNullOrEmpty(setup.PortName))
                return;
            setup.BaudRate = "10400";
            setup.Parity = "None";
            setup.TimeOutMs = 0;
            setup.StopBits = "One";
            setup.DataBits = "8";
            setup.Handshake = "None";
            setup.Encoding = "ASCII";
            setup.RtsEnable = true;
            setup.DtrEnable = false;
            setup.PortName = SerialComPort.Helper.SetupValues.GetPortNameValues()[0];
        }
    }
}
