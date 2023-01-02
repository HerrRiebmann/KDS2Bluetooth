using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.Linq;
using System.Text;

namespace EcuEmulator.Translation
{
    public static class HexCalculator
    {
        public static readonly byte[] FormatBytes =
        {
            0x80, //With length information
            0x81, //Without length
            0xC0, //Length but no header
            0xC1  //No length and no header
        };

        public static bool ValidateHexText(string hexText)
        {
            try
            {
                foreach (var hex in hexText.Split(' '))
                {
                    var i = int.Parse(hex, NumberStyles.HexNumber);
                    if (i < 0 || i > 256)
                        return false;
                }

                return true;
            }
            catch (Exception e)
            {
                Debug.WriteLine(e);
            }
            return false;
        }

        public static string GetPreText(string hexString)
        {
            if(string.IsNullOrEmpty(hexString) || !ValidateHexText(hexString))
                return string.Empty;
            var bytes = ConvertToBytes(hexString);
            var pos = GetFormatBytePosition(bytes);
            if (pos >= 0)
                return hexString.Substring(0, pos * 3);
            return hexString;
        }

        public static string GetPostText(string hexString)
        {
            if (string.IsNullOrEmpty(hexString) || !ValidateHexText(hexString))
                return string.Empty;
            var bytes = ConvertToBytes(hexString);
            var hexValues = bytes as byte[] ?? bytes.ToArray();
            var pos = GetFormatBytePosition(hexValues);
            if (pos < 0)
                return string.Empty;
            var length = hexValues.Count() - pos;
            var msgBytes = new byte[length];
            Array.Copy(hexValues, pos, msgBytes, 0, length);
            var chkPos = GetChecksumPos(msgBytes);
            if (chkPos >= 0 && hexString.Length > (pos + chkPos + 1) * 3)
                return hexString.Substring(((pos + chkPos +1) * 3)-1);

            return string.Empty;
        }

        public static string GetMessageText(string hexString)
        {
            if (string.IsNullOrEmpty(hexString) || !ValidateHexText(hexString))
                return string.Empty;
            var bytes = ConvertToBytes(hexString);
            var hexValues = bytes as byte[] ?? bytes.ToArray();
            var pos = GetFormatBytePosition(hexValues);
            if (pos < 0)
                return string.Empty;
            var length = hexValues.Count() - pos;
            var msgBytes = new byte[length];
            Array.Copy(hexValues, pos, msgBytes, 0, length);
            var chkPos = GetChecksumPos(msgBytes);
            //If no checksum, take length
            if (chkPos < 0)
                chkPos = length -1;

            var msgLength = ((chkPos + 1) * 3) - 1; //Ignore ending space
            //In case checksum is " 00" and will be cut off by optimization:
            if (msgLength + pos * 3 > hexString.Length)
                msgLength = hexString.Length - pos * 3;
            if (chkPos >= 0 && hexString.Length >= msgLength)
                return hexString.Substring(pos * 3, msgLength);

            return string.Empty;
        }

        public static int GetMessageLength(IEnumerable<byte> hexValues)
        {
            var len = GetMessageLengthPosition(hexValues);
            if (len < 0 || len >= hexValues.Count())
                return 0;
            return hexValues.ElementAt(len);
        }

        public static int GetMessageLengthPosition(IEnumerable<byte> hexValues)
        {
            var format = hexValues.ElementAt(0);
            if (format == FormatBytes[0])
                return 3;
            if (format == FormatBytes[1])
                return 0;
            if (format == FormatBytes[2])
                return 1;
            if (format == FormatBytes[3])
                return 0;
            return 0;
        }

        public static IEnumerable<byte> GetMessageData(IEnumerable<byte> hexValues)
        {
            var enumerable = hexValues as byte[] ?? hexValues.ToArray();
            var pos = GetMessageLengthPosition(enumerable);
            if (pos == 0)
                return new byte[0];
            pos += 3; //Ignore Length, SID & PID
            var length = GetMessageLength(enumerable);
            if (length < 2)
                return new byte[0];
            length -= 2; //Ignore SID & PID
            var msgBytes = new byte[length];
            if (pos < 0 || length == 0)
                return msgBytes;
            if (enumerable.Length < pos + length) //Message is shorter then expected. Leave with zeros... 
                Array.Copy(enumerable, pos, msgBytes, 0, enumerable.Length - pos);
            else
                Array.Copy(enumerable, pos, msgBytes, 0, length);
            var chkPos = GetChecksumPos(msgBytes);
            if (chkPos >= 0)
                return msgBytes;
            return msgBytes;
        }

        public static byte GetMessageChecksum(IEnumerable<byte> hexValues)
        {
            var enumerable = hexValues as byte[] ?? hexValues.ToArray();
            var pos = GetFormatBytePosition(enumerable);
            if (pos < 0)
                return 0x00;
            var length = enumerable.Count() - pos;
            var msgBytes = new byte[length];
            Array.Copy(enumerable, pos, msgBytes, 0, length);
            var chkPos = GetChecksumPos(msgBytes);
            if (chkPos >= 0 && chkPos < enumerable.Length)
                return enumerable.ElementAt(chkPos);

            return CalcCheckSum(msgBytes);
        }

        public static int GetFormatBytePosition(IEnumerable<byte> hexValues)
        {
            var enumerable = hexValues as byte[] ?? hexValues.ToArray();
            for(var i = 0; i < enumerable.Count(); i++)
            {
                var hex = enumerable.ElementAt(i);
                if (FormatBytes.Contains(hex))
                    return i;
            }
            return -1;
        }

        public static int GetChecksumPos(IEnumerable<byte> hexValues)
        {
            var pos = GetMessageLengthPosition(hexValues);
            var len = GetMessageLength(hexValues);
            if (pos > 0 && len > 0)
                return pos + len + 1;

            var enumerable = hexValues as byte[] ?? hexValues.ToArray();
            var cksBytes = new List<byte>();
            for (var i = 0; i < enumerable.Count(); i++)
            {
                var hex = enumerable.ElementAt(i);
                if (hex == CalcCheckSum(cksBytes.ToArray()))
                    return i;
                cksBytes.Add(hex);
            }
            return -1;
        }

        public static byte CalcCheckSum(IEnumerable<byte> hexValues)
        {
            return hexValues.Aggregate(new byte(), (current, hexValue) => (byte)(current + hexValue));
        }

        public static IEnumerable<byte> ConvertToBytes(string hexString)
        {
            hexString = hexString.Trim();
            if (string.IsNullOrEmpty(hexString))
                return new byte[0];
            //return Encoding.UTF8.GetBytes(hexString);
            var hexValuesSplit = hexString.Split(' ');
            var len = hexValuesSplit.Length;
            var hexByte = new byte[len];
            var i = 0;
            foreach (var hex in hexValuesSplit)
            {
                hexByte[i++] = ConvertToByte(hex);
            }

            return hexByte;
        }

        public static byte ConvertToByte(string hexValue)
        {
            hexValue = hexValue.Trim();
            return string.IsNullOrEmpty(hexValue) ? (byte) 0x00 : Convert.ToByte(hexValue, 16);
        }

        public static ulong ConvertToInteger(string hexString)
        {
            var hexValuesSplit = hexString.Trim().Split(' ');
            var len = hexValuesSplit.Length;
            ulong i = 0;
            foreach (var hex in hexValuesSplit)
            {
                i *= 256;
                i += ulong.Parse(hex, NumberStyles.HexNumber);
            }
            return i;
        }

        public static string ConvertToText(string hexString)
        {
            return ConvertToText(ConvertToBytes(hexString));
        }

        public static string ConvertToText(IEnumerable<byte> hexValues)
        {
            return Encoding.UTF8.GetString(hexValues.ToArray());
        }

        public static string ConvertToBinaryString(IEnumerable<byte> hexValues)
        {
            var output = string.Empty;
            foreach (var b in hexValues)
            {
                output += Convert.ToString(b, 2).PadLeft(8, '0'); ;
                output += " ";
            }
            return output.TrimEnd();
        }

        public static string ConvertToHexString(byte b)
        {
            var bytes = new []{b};
            return ConvertToHexString(bytes);
        }

        public static string ConvertToHexString(IEnumerable<byte> bytes)
        {
            var output = string.Empty;
            foreach (var b in bytes)
            {
                //output += byte.Parse(string.Format("{0:x2}", b), NumberStyles.HexNumber);
                output += b.ToString("X").PadLeft(2, '0').ToUpper();
                output += " ";
            }
            //return output.Remove(output.Length-1, 1);
            return output.TrimEnd();
        }
    }
}
