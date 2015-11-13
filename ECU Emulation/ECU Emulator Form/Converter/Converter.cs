using System;
using System.Globalization;
using System.Linq;
using System.Windows.Data;

namespace EcuEmulatorForm.Converter
{
    [ValueConversion(typeof(bool), typeof(System.Windows.Visibility))]
    public class NegatingConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return !((bool)value);
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return !((bool)value);
        }
    }

    [ValueConversion(typeof(int), typeof(System.Windows.Visibility))]
    public class NotNegativeConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return ((int)value) >= 0;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return ((int)value) >= 0;
        }
    }

    [ValueConversion(typeof(string), typeof(DateTime))]
    public class DateTimeConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return ((DateTime)value).ToString(new CultureInfo("de-DE"));
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            DateTime dt;
            //DateTime.TryParse(value.ToString(), out dt);
            DateTime.TryParse(value.ToString(), CultureInfo.CurrentCulture, DateTimeStyles.AdjustToUniversal, out dt);
            return dt;
        }
    }

    [ValueConversion(typeof(byte), typeof(string))]
    public class ByteToHexConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return ((byte)value).ToString("X").PadLeft(2, '0');
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return System.Convert.ToByte((string) value, 16);
        }
    }

    [ValueConversion(typeof(byte[]), typeof(string))]
    public class ByteArrayToHexConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null)
                return string.Empty;
            var byteString = ((byte[]) value).Aggregate(string.Empty, (current, e) => current + e.ToString("X2") + " ");
            if (byteString.EndsWith(" "))
                byteString = byteString.Remove(byteString.Length - 1, 1);

            return byteString;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null)
                return null;
            var bytes = new byte[value.ToString().Split(' ').Count()];
            var i = 0;
            foreach (var s in value.ToString().Split(' '))
            {
                bytes[i++] = System.Convert.ToByte(s, 16);
            }
            return bytes;
        }
    }
}
