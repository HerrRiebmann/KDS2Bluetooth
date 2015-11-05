using System;
using System.ComponentModel;
using System.IO;
using System.Text;
using System.Xml.Serialization;
using Newtonsoft.Json;

namespace Backend.Data
{
    public static class Serializer
    {
        public enum ConversionTypes
        {
            Xml,
            Json
        }
    }

    public class Serializer<T> : INotifyPropertyChanged
    {
        #region INotifyPropertyChanged implementation

        public event PropertyChangedEventHandler PropertyChanged;

        protected void Notify(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        #endregion INotifyPropertyChanged implementation

        public static string GetDefaultFileName()
        {
            return Path.Combine(GetDefaultDirectoryName(), typeof(T).Name);
        }

        public static string GetDefaultFileNameWithExtension()
        {
            return Path.Combine(GetDefaultDirectoryName(), typeof(T).Name + ".xml");
        }

        public static string GetDefaultDirectoryName()
        {
            var path = System.Reflection.Assembly.GetExecutingAssembly().GetName().CodeBase;
            path = Path.GetDirectoryName(path);
            return path;

            //Roaming/Local...
            //return Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData),
            //                    Path.GetFileName(System.Reflection.Assembly.GetEntryAssembly().GetName().Name));

            //Application-Data
            //return Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), Application.Current.MainWindow.GetType().Assembly.GetName().Name);

            //CommonAppData
            //return Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData);
        }

        public void Save(string fileName, Serializer.ConversionTypes conversionType)
        {
            if (fileName == null)
                fileName = string.Empty;
            try
            {
                 fileName = fileName.Remove(0, 6);

                 var ending = conversionType == Serializer.ConversionTypes.Json ? ".json" : ".xml";

                if (fileName.IndexOf(@"\", StringComparison.Ordinal) >= 0)
                    if (!Directory.Exists(Path.GetDirectoryName(fileName)))
                        Directory.CreateDirectory(Path.GetDirectoryName(fileName));
                if (!Path.HasExtension(fileName))
                    fileName = Path.Combine(fileName, typeof(T).Name + ending);

                var fs = new FileStream(fileName, FileMode.Create, FileAccess.ReadWrite, FileShare.ReadWrite);
                if (conversionType == Serializer.ConversionTypes.Xml)
                {
                    var xmlSer = new XmlSerializer(typeof (T));
                    xmlSer.Serialize(fs, this);
                }
                if (conversionType == Serializer.ConversionTypes.Json)
                {
                    var jstr = JsonConvert.SerializeObject(this);
                    fs.Write(Encoding.Default.GetBytes(jstr), 0, Encoding.Default.GetByteCount(jstr));
                }
                fs.Close();
            }
            catch (IOException e)
            {
                throw new Exception("File is locked", e);
            }
            catch (Exception ex)
            {
                throw new Exception("Store settings", ex);
            }
        }

        public void Save(Serializer.ConversionTypes conversionType)
        {
            Save(GetDefaultDirectoryName(), conversionType);
        }

        public void Save(string fileName)
        {
            Save(fileName, Serializer.ConversionTypes.Xml);
        }

        public void Save()
        {
            Save(GetDefaultDirectoryName());
        }

        public static T Load(string fileName, Serializer.ConversionTypes conversionType)
        {
            fileName = fileName.Remove(0, 6);

            T data = default(T);
            var ending = conversionType == Serializer.ConversionTypes.Json ? ".json" : ".xml";
            if (!Path.HasExtension(fileName))
                fileName = Path.Combine(fileName, typeof(T).Name + ending);
            if (File.Exists(fileName))
            {
                try
                {
                    var fs = new FileStream(fileName, FileMode.Open, FileAccess.ReadWrite, FileShare.ReadWrite);
                    if (conversionType == Serializer.ConversionTypes.Xml)
                    {
                        var xmlSer = new XmlSerializer(typeof (T));
                        data = (T) xmlSer.Deserialize(fs);
                    }
                    if (conversionType == Serializer.ConversionTypes.Json)
                    {
                        using (var reader = new StreamReader(fs))
                        {
                            data = (T)JsonConvert.DeserializeObject(reader.ReadToEnd(), typeof(T));
                        }
                        
                    }
                    fs.Close();
                }
                catch (IOException e)
                {
                    throw new Exception("File is locked", e);
                }
                catch (InvalidOperationException ioe)
                {
                    throw new Exception("XML Deserialize", ioe.InnerException);
                }
                catch (Exception ex)
                {
                    throw new Exception("Load XML", ex);
                }
            }

            return data != null ? data : Activator.CreateInstance<T>();
        }

        public static T Load(string fileName)
        {
            return Load(fileName, Serializer.ConversionTypes.Xml);
        }

        public static T Load(Serializer.ConversionTypes conversionType)
        {
            return Load(GetDefaultDirectoryName(), conversionType);
        }

        public static T Load()
        {
            return Load(GetDefaultDirectoryName());
        }
    }
}
