using EcuEmulator.Data;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EcuEmulator.Initialization
{
    public class DTC
    {
        public static DiagnosticTroubleCodes InitializeDTC()
        {
            var dtc = new DiagnosticTroubleCodes();
            InitStandard(ref dtc);
            InitSuzuki(ref dtc);
            InitKawasaki(ref dtc);
            return dtc;
        }

        private static void InitStandard(ref DiagnosticTroubleCodes dtc)
        {

        }

        private static void InitSuzuki(ref DiagnosticTroubleCodes dtc)
        {

        }
        private static void InitKawasaki(ref DiagnosticTroubleCodes dtc)
        {
            //dtc.Add('', new byte[] { 0x, 0x }, "", new FTB(0x, ""), ManufacturerEnum.Kawasaki);
            dtc.Add('X', new byte[] { 0x00, 0x75 }, "Air Intake solenoid Valve Malfunction(wiring open or short)", ManufacturerEnum.Kawasaki);
            //dtc.Add('', new byte[] { 0x, 0x }, "", ManufacturerEnum.Kawasaki);
        }
    }
}
