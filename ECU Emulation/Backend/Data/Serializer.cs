using System;
using System.Collections;
using System.ComponentModel;
using System.IO;
using System.Runtime.CompilerServices;
using System.Runtime.Serialization;
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
            Json,
            Yaml
        }
    }
    public class Serializer<T> : INotifyPropertyChanged
    {
        #region INotifyPropertyChanged implementation

        public event PropertyChangedEventHandler PropertyChanged;

        [NonSerialized]
        private volatile bool _modified;
        [XmlIgnore]
        [JsonIgnore]
        //[YamlIgnore]
        [Browsable(false)]
        public bool Modified
        {
            get
            {
                var mod = _modified;
                _modified = false;
                if (!mod && IsCollection)
                    return CollectionModified(this);
                return mod;
            }
        }

        protected void Notify([CallerMemberName] string propertyName = "")
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
            _modified = true;
            //System.Diagnostics.Debug.WriteLine("PropertyChanged: " + propertyName + "\t" + this);
        }

        #endregion INotifyPropertyChanged implementation

        private const string FilePrefix = "file:\\";

        private static string FolderDelimeter => @"\";

        public static string GetDefaultFilePath()
        {
            return Path.Combine(GetDefaultDirectoryName(), GetDefaultFileName());
        }
        public static string GetDefaultFileName()
        {
            if (!(typeof(T) is ISerializable))
                return typeof(T).Name;

            //If SerializableList<> take name from its type within the collection
            if (typeof(T).IsGenericType && typeof(T).GetGenericTypeDefinition() == typeof(SerializableList<>))
            {
                var item = typeof(T).GetProperty("Items")?.PropertyType;
                if (item != null && item.IsGenericType && item.GetGenericTypeDefinition() ==
                    typeof(System.Collections.ObjectModel.ObservableCollection<>))
                {
                    return item.GetGenericArguments()[0].Name + "List";
                }
            }
            return typeof(T).Name;
        }

        public static string GetDefaultFileNameWithExtension(Serializer.ConversionTypes conversionType = Serializer.ConversionTypes.Json)
        {
            return GetDefaultFileName() + GetFileExtension(conversionType);
        }

        public static string GetDefaultFilePathWithExtension(Serializer.ConversionTypes conversionType = Serializer.ConversionTypes.Json)
        {
            return Path.Combine(GetDefaultDirectoryName(), GetDefaultFileNameWithExtension(conversionType));
        }

        public static string GetDefaultDirectoryName()
        {
            var path = System.Reflection.Assembly.GetExecutingAssembly().GetName().CodeBase;
            path = Path.GetDirectoryName(path);
            if (path.Contains(FilePrefix))
                path = path.Remove(0, FilePrefix.Length);
            if (!path.EndsWith(FolderDelimeter))
                path += FolderDelimeter;
            return path;

            //Roaming/Local...
            //return Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData),
            //                    Path.GetFileName(System.Reflection.Assembly.GetEntryAssembly().GetName().Name));

            //Application-Data
            //return Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), Application.Current.MainWindow.GetType().Assembly.GetName().Name);

            //CommonAppData
            //return Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData);
        }

        private static string GetFileExtension(Serializer.ConversionTypes conversionType = Serializer.ConversionTypes.Json)
        {
            return string.Format($".{conversionType.ToString().ToLower()}");
        }

        public void Save(Serializer.ConversionTypes conversionType)
        {
            Save(GetDefaultDirectoryName(), conversionType);
        }

        public void Save(string fileName = "", Serializer.ConversionTypes conversionType = Serializer.ConversionTypes.Json)
        {
            AutocompleteFilename(ref fileName, ref conversionType);
            try
            {
                var fs = new FileStream(fileName, FileMode.Create, FileAccess.ReadWrite, FileShare.ReadWrite);
                switch (conversionType)
                {
                    case Serializer.ConversionTypes.Json:
                        var jstr = JsonConvert.SerializeObject(this);
                        fs.Write(Encoding.Default.GetBytes(jstr), 0, Encoding.Default.GetByteCount(jstr));
                        break;
                    case Serializer.ConversionTypes.Xml:
                        var xmlSer = new XmlSerializer(typeof(T));
                        xmlSer.Serialize(fs, this);
                        break;
                    case Serializer.ConversionTypes.Yaml:
                        var serializer = new YamlDotNet.Serialization.Serializer();
                        var ystr = serializer.Serialize(this);
                        fs.Write(Encoding.Default.GetBytes(ystr), 0, Encoding.Default.GetByteCount(ystr));
                        break;
                }
                fs.Close();
            }
            catch (IOException e)
            {
                throw new Exception("File is locked", e);
            }
            catch (Exception ex)
            {
                throw new Exception("Serialize", ex);
            }
        }

        public static T Load(Serializer.ConversionTypes conversionType)
        {
            return Load(string.Empty, conversionType);
        }

        public static T Load(string fileName = "", Serializer.ConversionTypes conversionType = Serializer.ConversionTypes.Json)
        {
            AutocompleteFilename(ref fileName, ref conversionType);

            T data = default(T);

            if (File.Exists(fileName))
            {
                try
                {
                    var fs = new FileStream(fileName, FileMode.Open, FileAccess.ReadWrite, FileShare.ReadWrite);
                    switch (conversionType)
                    {
                        case Serializer.ConversionTypes.Json:
                            using (var reader = new StreamReader(fs))
                                data = (T)JsonConvert.DeserializeObject(reader.ReadToEnd(), typeof(T));
                            break;
                        case Serializer.ConversionTypes.Xml:
                            var xmlSer = new XmlSerializer(typeof(T));
                            data = (T)xmlSer.Deserialize(fs);
                            break;
                        case Serializer.ConversionTypes.Yaml:
                            using (var reader = new StreamReader(fs))
                            {
                                var deserializer = new YamlDotNet.Serialization.DeserializerBuilder()
                                    .IgnoreUnmatchedProperties().Build();
                                data = (T)deserializer.Deserialize(reader.ReadToEnd(), typeof(T));
                            }
                            break;
                    }
                    fs.Close();
                }
                catch (IOException e)
                {
                    throw new Exception("File is locked", e);
                }
                catch (InvalidOperationException ioe)
                {
                    throw new Exception("Invalid Deserialize Operation", ioe.InnerException);
                }
                catch (Exception ex)
                {
                    throw new Exception("Deserialize", ex);
                }
            }
            ResetProperty(data);
            return data != null ? data : Activator.CreateInstance<T>();
        }

        private static void AutocompleteFilename(ref string fileName, ref Serializer.ConversionTypes conversionType)
        {
            if (string.IsNullOrEmpty(fileName))
                fileName = GetDefaultFilePathWithExtension(conversionType);
            if (fileName.StartsWith(FilePrefix))
                fileName = fileName.Remove(0, FilePrefix.Length);

            //Change conversion type, if necessary
            UpdateConversionTypeByFileName(fileName, ref conversionType);

            //Check if only filename
            if (!fileName.Contains(FolderDelimeter))
                fileName = Path.Combine(GetDefaultDirectoryName(), fileName);
            //Check if Directory exists:
            var dir = Path.GetDirectoryName(fileName);
            if (!string.IsNullOrEmpty(dir))
                if (FilePermissionGranted())
                    if (!Directory.Exists(dir))
                        Directory.CreateDirectory(dir);

            //Only Folder submitted:
            if (fileName.EndsWith(FolderDelimeter))
                fileName = Path.Combine(fileName, GetDefaultFileNameWithExtension(conversionType));

            var ending = GetFileExtension(conversionType);
            if (!Path.HasExtension(fileName))
                fileName += ending;
        }

        private static void UpdateConversionTypeByFileName(string fileName, ref Serializer.ConversionTypes conversionType)
        {
            if (string.IsNullOrEmpty(fileName))
                return;
            var extension = Path.GetExtension(fileName);
            if (string.IsNullOrEmpty(extension))
                return;
            switch (extension.ToLower())
            {
                case ".json":
                    conversionType = Serializer.ConversionTypes.Json;
                    break;
                case ".yaml":
                    conversionType = Serializer.ConversionTypes.Yaml;
                    break;
                case ".xml":
                    conversionType = Serializer.ConversionTypes.Xml;
                    break;
            }
        }

        public static bool FilePermissionGranted()
        {
            return true;
        }

        private bool? _isCollection;
        public bool IsCollection
        {
            get
            {
                if (_isCollection == null)
                    _isCollection = IsCollectionCheck();
                return (bool)_isCollection;
            }
        }
        private static bool IsCollectionCheck()
        {
            if (!(typeof(T) is ISerializable))
                return false;
            if (typeof(T).IsGenericType && typeof(T).GetGenericTypeDefinition() == typeof(SerializableList<>))
            {
                var item = typeof(T).GetProperty("Items")?.PropertyType;
                if (item != null && item.IsGenericType && item.GetGenericTypeDefinition() ==
                    typeof(System.Collections.ObjectModel.ObservableCollection<>))
                {
                    return true;
                }
            }
            return false;
        }

        private static bool CollectionModified(object data, bool resetOnly = false)
        {
            if (!(typeof(T) is ISerializable))
                return false;
            if (typeof(T).IsGenericType && typeof(T).GetGenericTypeDefinition() == typeof(SerializableList<>))
            {
                var prop = typeof(T).GetProperty("Items")?.PropertyType;
                if (prop != null && prop.IsGenericType && prop.GetGenericTypeDefinition() ==
                    typeof(System.Collections.ObjectModel.ObservableCollection<>))
                {
                    var items = typeof(T).GetProperty("Items")?.GetValue(data);
                    if (items != null)
                        foreach (var item in (IEnumerable)items)
                        {
                            var t = item.GetType().BaseType;
                            if (t != null)
                            {
                                var fld = t.GetField("_modified", System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance);
                                if (fld != null)
                                {
                                    if ((bool)fld.GetValue(item))
                                    {
                                        fld.SetValue(item, false);
                                        if (!resetOnly)
                                            return true;
                                    }
                                }
                            }
                        }
                }
            }
            return false;
        }

        private static void ResetProperty(T data)
        {
            //Clear Modified Property after initialization
            if (data == null)
                return;
            var t = typeof(T).BaseType;
            if (t == null)
                return;
            var fld = t.GetField("_modified", System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance);
            if (fld != null)
            {
                fld.SetValue(data, false);
            }

            //Clear each Modified in Collection
            CollectionModified(data, true);
        }
    }
}
