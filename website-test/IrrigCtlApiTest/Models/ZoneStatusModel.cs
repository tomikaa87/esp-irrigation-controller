using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace IrrigCtlApiTest
{
    public class ZoneStatusModel
    {
        public int ActiveZone { get; set; }

        public double TankLevel { get; set; }

        public string FirmwareVersion { get; set; }
    }
}
