namespace EcuEmulator.Initialization
{
    public static class Calculations
    {
        public static Data.Calculations Init()
        {
            var calculations = new Data.Calculations();

            if (System.IO.File.Exists("Suzuki.json"))
                calculations.AddRange(Data.Calculations.Load("Suzuki.json"));
            else
                Suzuki.Init(ref calculations);

            if (System.IO.File.Exists("Kawasaki.json"))
                calculations.AddRange(Data.Calculations.Load("Kawasaki.json"));
            else
                Kawasaki.Init(ref calculations);

            if (System.IO.File.Exists("Honda.json"))
                calculations.AddRange(Data.Calculations.Load("Honda.json"));
            else
                Honda.Init(ref calculations);

            if (System.IO.File.Exists("OBD2.json"))
                calculations.AddRange(Data.Calculations.Load("OBD2.json"));
            else
                OBD2.Init(ref calculations);

            return calculations;
        }
    }
}
