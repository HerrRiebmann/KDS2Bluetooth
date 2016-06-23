using System;
using System.Reflection;
using System.Text;

namespace Backend.Helper
{
    public static class Converter
    {
        #region Hex and Byte
        /// <summary>
        /// method to convert hex string into a byte array
        /// </summary>
        /// <param name="msg">string to convert</param>
        /// <returns>a byte array</returns>
        public static byte[] HexToByte(string msg)
        {
            if(String.IsNullOrEmpty(msg))
                return new byte[0];
            //remove any spaces from the string
            msg = msg.Replace(" ", "");
            //create a byte array the length of the
            //divided by 2 (Hex is 2 characters in length)
            var comBuffer = new byte[msg.Length / 2];
            //loop through the length of the provided string
            for (int i = 0; i < msg.Length; i += 2)
                //convert each set of 2 characters to a byte
                //and add to the array
                comBuffer[i / 2] = Convert.ToByte(msg.Substring(i, 2), 16);
            //return the array
            return comBuffer;
        }
        
        /// <summary>
        /// method to convert a byte array into a hex string
        /// </summary>
        /// <param name="comByte">byte array to convert</param>
        /// <returns>a hex string</returns>
        public static string ByteToHex(byte[] comByte)
        {
            if (comByte.Length >= 1)
            {
                //create a new StringBuilder object
                var builder = new StringBuilder(comByte.Length * 3);
                //loop through each byte in the array
                foreach (byte data in comByte)
                    //convert the byte to a string and add to the stringbuilder
                    builder.Append(Convert.ToString(data, 16).PadLeft(2, '0').PadRight(3, ' '));
                //remove last ' '
                builder.Remove(builder.Length - 1, 1);
                //return the converted value
                return builder.ToString().ToUpper();
            }
            return "ByteArray is Empty";
            //return "00";
        }

        public static string ByteToHex(byte comByte)
        {
            //create a new StringBuilder object
            var builder = new StringBuilder(3);
            //convert the byte to a string and add to the stringbuilder
            builder.Append(Convert.ToString(comByte, 16).PadLeft(2, '0'));
            //return the converted value
            return builder.ToString().ToUpper();
        }
        #endregion

        public static Enum[] EnumToArray(Enum enumeration)
        {
            //get the enumeration type
            var et = enumeration.GetType();

            //get the public static fields (members of the enum)
            var fi = et.GetFields(BindingFlags.Static | BindingFlags.Public);

            //create a new enum array
            var values = new Enum[fi.Length];

            //populate with the values
            for (var iEnum = 0; iEnum < fi.Length; iEnum++)
            {
                values[iEnum] = (Enum)fi[iEnum].GetValue(enumeration);
            }

            //return the array
            return values;
        }
    }
}
