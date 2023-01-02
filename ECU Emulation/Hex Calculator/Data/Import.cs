using System;
using System.IO;
using System.Linq;
using EcuEmulator.Data;

namespace Hex_Calculator.Data
{
    public static class Import
    {
        public static void LoadHexData(string filePath, ref DataEntries calc)
        {
            calc.Clear();
            using (var fs = new FileStream(filePath, FileMode.Open))
            {
                int hexIn;
                int msgCounter = 0;
                var hex = string.Empty;
                for (var i = 1; (hexIn = fs.ReadByte()) != -1; i++)
                {
                    hex += string.Format("{0:X2}", hexIn);
                    hex += ' ';
                    if (i % 65 == 0)
                    {
                        hex = RemoveTrailingZeros(hex);
                        calc.Add(msgCounter++, hex);
                        hex = string.Empty;
                    }
                }
            }
        }

        public static void LoadCsvData(string filePath, ref DataEntries calc)
        {
            var hexValues = new[] {"<80", ">80", "<81", ">81"};
            var seperator = char.MinValue;
            var lineCounter = 0;
            calc.Clear();
            try
            {
                using (var fs = new StreamReader(filePath))
                {
                    while (fs.Peek() >= 0)
                    {
                        var line = fs.ReadLine();
                        var hexData = string.Empty;
                        var timestamp = string.Empty;
                        lineCounter++;
                        if (!hexValues.Contains(line.Substring(0,3)))
                            continue;

                        if (seperator == char.MinValue)
                            if (line.Contains(','))
                                seperator = ',';
                            else
                            if (line.Contains(';'))
                                seperator = ';';

                        if (line.Contains(seperator))
                        {
                            hexData = line.Substring(1, line.IndexOf(seperator) - 1);
                            timestamp = line.Substring(line.IndexOf(seperator) +1);
                        }
                        else
                            hexData = line.Substring(1);

                        hexData = RemoveSpaces(hexData);

                        if (!string.IsNullOrEmpty(hexData))
                            try
                            {
                                calc.Add(lineCounter, hexData, timestamp);
                            }
                            catch (Exception lineEx)
                            {
                                System.Diagnostics.Debug.WriteLine($"{lineCounter}:\t{lineEx.Message}");
                            }
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }
        }

        public static void LoadTxtData(string filePath, ref DataEntries calc)
        {
            const char seperator = ' ';
            var lineCounter = 0;
            calc.Clear();
            try
            {
                using (var fs = new StreamReader(filePath))
                {
                    while (fs.Peek() >= 0)
                    {
                        var line = fs.ReadLine();
                        var hexData = string.Empty;
                        var timestamp = string.Empty;
                        lineCounter++;

                        if(line.Contains(seperator))
                        {
                            hexData = line.Substring(line.IndexOf(seperator) + 1);
                            timestamp = line.Substring(0, line.IndexOf(seperator));
                            
                            if (hexData.StartsWith("80") || hexData.StartsWith("81"))
                                if (!string.IsNullOrEmpty(hexData))
                                    calc.Add(lineCounter, hexData, timestamp);
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }
        }
        public static void LoadRawData(string filePath, ref DataEntries calc)
        {            
            var lineCounter = 0;
            calc.Clear();
            try
            {
                using (var fs = new StreamReader(filePath))
                {
                    while (fs.Peek() >= 0)
                    {
                        var line = fs.ReadLine();
                        var hexData = string.Empty;
                        var timestamp = string.Empty;
                        lineCounter++;

                        hexData = line;
                        if (hexData.StartsWith("80") || hexData.StartsWith("81"))
                            if (!string.IsNullOrEmpty(hexData))
                                calc.Add(lineCounter, hexData, timestamp);
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }
        }
        private static string RemoveTrailingZeros(string hex)
        {
            hex = RemoveSpaces(hex);
            var ending = " 00";
            while (hex.EndsWith(ending))
            {
                hex = hex.Substring(0, hex.Length - ending.Length);
            }
            return hex;
        }

        private static string RemoveLeadingZeros(string hex)
        {
            if (hex.StartsWith("00"))
                return hex.Substring(2);
            return hex;
        }

        private static string RemoveSpaces(string hex)
        {
            hex = hex.Trim(' ');
            return hex;
        }

        public static bool CleanupFile(string filePath)
        {
            //21:22:39.420 xtf -> Timestamp, No Hex
            //21:23:38.566 8011F10210801480F11102508054 -> Timestamp & Hex (but doubled)
            //>008111F18104 -> Incoming, but trailing Zeros...
            //

            var output = new StreamWriter(filePath.Replace(Path.GetExtension(filePath), "_2.csv"));
            try
            {
                using (var fs = new StreamReader(filePath))
                {
                    while (fs.Peek() >= 0)
                    {
                        var line = fs.ReadLine();
                        var hexData = string.Empty;
                        var timestamp = string.Empty;

                        //Starts with timestamp:
                        //21:22:39.420 xtf -> Timestamp, No Hex
                        //21:23:38.566 8011F10210801480F11102508054 -> Timestamp & Hex (but doubled)
                        if (line.Contains(' ') && line.Contains(':'))
                        {
                            timestamp = line.Substring(0, line.IndexOf(' '));
                            hexData = line.Substring(line.IndexOf(' ') + 1);
                            CreateOutput(ref output, timestamp, hexData);
                        }
                        if (line.Contains(';'))
                        {
                            hexData = line.Substring(1, line.IndexOf(';') - 1);
                            timestamp = line.Substring(line.IndexOf(';') + 1);
                            CreateOutput(ref output, timestamp, hexData);
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
                output.Close();
                return false;
            }
            output.Close();
            return true;
        }

        private static void CreateOutput(ref StreamWriter output, string timestamp, string hexData)
        {
            var messages = new System.Collections.Generic.List<string>();
            //Remove leading zeros (only if not further message-part stored)
            hexData = RemoveLeadingZeros(hexData);
            //Check trailing Zeros and remove spaces
            hexData = RemoveTrailingZeros(hexData);
            //Remove spaces in between
            hexData = hexData.Replace(" ", "");

            if(hexData.Contains('<'))
                hexData.Remove(hexData.IndexOf('<'),1);
            if (hexData.Contains('>'))
                hexData.Remove(hexData.IndexOf('>'), 1);

            //Check hexData contains single message
            //otherwise add the message to a temp storage

            //>81 12 F1 81 05 80 F1 12 03 C1 EA 8F C0;45509 -> 2 messages
            //> 80 12 F1 01; 45553 -> Incomplete
            //> 3E C2 80 F1 12 01 7E 02 80 12 F1 02 21 08 AE 80 F1 12 34 61 08 00 00 00 00 00 00 00 FF FF FF FF 00 00 39 B9 5B 5A BA 00 00 00 FF FF FF FF 00 00 00 00 00 00 00 00 FF FF 40 40 40 40 FF F0; 45631
            //> FF FF FF 00 00 04 28 FF FF 8D 80 12 F1 02 21 80 26 80 F1 12 66 61 80 00 00 00 00 00 00 00 00 00 FF FF FF FF FF FF 80 7F 5B FF FF FF 80 16 FF FF FF 00 80 00 FF 80 00 FF FF FF 80 FF FF FF FF; 45804
            //> 80 80 80 80; 45843
            //> 04 00 04 00; 45876
            //> 04 00 04 FF; 45915

            //Check Hex Messages only
            if (!OnlyHexInString(hexData))
                return;

            //If storage is filled:
            hexData = _hexData + hexData;
            _hexData = string.Empty;

            //Split messages
            string txt = SplitMessage(ref messages, hexData);
            while (!string.IsNullOrEmpty(txt))
            {
                var txt2 = SplitMessage(ref messages, txt);
                if (txt2.Equals(txt))
                    break;
                txt = txt2;
            }

            //if (hexData.Contains("80F1") && !hexData.StartsWith("80F1"))
            //{
            //    messages.Add(hexData.Substring(0, hexData.IndexOf("80F1")));
            //    string rest = hexData.Substring(hexData.IndexOf("80F1"));
            //    if (HexMsgComplete(rest))
            //        messages.Add(rest);
            //    else
            //        TempStoreMsg(rest, timestamp);
            //}
            //else
            //    if (HexMsgComplete(hexData))
            //        messages.Add(hexData);
            //    else
            //        TempStoreMsg(hexData, timestamp);

            foreach(var msg in messages)
            {
                string direction = msg[2] == 'F' ? "<" : ">";
                output.WriteLine($"{direction}{msg};{TimeStampToMilis(timestamp)}");
            }
        }

        private static string SplitMessage(ref System.Collections.Generic.List<string> messages, string hexString)
        {
            try
            {
                string hexWithSpaces = string.Empty;
                int counter = 2;
                foreach(char c in hexString)
                {
                    if(counter++ %2 == 0)
                        hexWithSpaces += c;
                    else
                        hexWithSpaces += $"{c} ";
                }
                var HexValues = EcuEmulator.Translation.HexCalculator.ConvertToBytes(hexWithSpaces);
                //var len = EcuEmulator.Translation.HexCalculator.GetMessageLength(HexValues);
                var end = EcuEmulator.Translation.HexCalculator.GetChecksumPos(HexValues);
                //No Checksum found:
                if (end == -1)
                    return hexString;
                var noOfChars = (end + 1) * 2;
                var msg = hexString.Substring(0, noOfChars);
                if(string.IsNullOrEmpty(msg))
                    return hexString;
                messages.Add(msg);
                return hexString.Remove(0, noOfChars);
            }
            catch(Exception e)
            {
                Console.WriteLine(e);
                _hexData = hexString;
                return string.Empty;
            }
        }

        private static string TimeStampToMilis(string timestamp)
        {
            int timestampInt;
            if (!Int32.TryParse(timestamp, out timestampInt))
            {
                return timestamp;
            }
            TimeSpan t = TimeSpan.FromMilliseconds(timestampInt);
            return string.Format("{0:D2}:{1:D2}:{2:D2}.{3:D3}",
                                    t.Hours,
                                    t.Minutes,
                                    t.Seconds,
                                    t.Milliseconds);
        }
        private static bool OnlyHexInString(string hexValue)
        {
            // For C-style hex notation (0xFF) you can use @"\A\b(0[xX])?[0-9a-fA-F]+\b\Z"
            return System.Text.RegularExpressions.Regex.IsMatch(hexValue, @"\A\b[0-9a-fA-F]+\b\Z");
        }
        private static bool HexMsgComplete(string hexValue)
        {
            //ToDo: everything...
            return true;
        }

        private static string _timestamp = string.Empty;
        private static string _hexData = string.Empty;
        private static void TempStoreMsg(string timestamp, string hexData)
        {
            _timestamp = timestamp;
            _hexData = hexData;
        }
    }
}
