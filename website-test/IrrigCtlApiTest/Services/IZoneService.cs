using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace IrrigCtlApiTest.Services
{
    public interface IZoneService
    {
        int GetActiveZone();
        double GetTankLevel();

        void Start(int zone);
        void Stop();
    }
}
