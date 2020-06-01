using System;
using System.Timers;

namespace IrrigCtlApiTest.Services
{
    public class ZoneService : IZoneService
    {
        private int _activeZone = 0;
        private double _tankLevel = 36.0;
        private readonly Timer _timer = new Timer(500);

        public ZoneService()
        {
            _timer.Elapsed += TimerElapsed;
        }

        private void TimerElapsed(object sender, ElapsedEventArgs e)
        {
            if (!(_tankLevel > 0))
                return;

            _tankLevel -= _activeZone * 0.01;
            
            if (_tankLevel < 0)
            {
                _tankLevel = 0;
                _timer.Stop();
            }
        }

        public int GetActiveZone()
        {
            return _activeZone;
        }

        public double GetTankLevel()
        {
            return _tankLevel;
        }

        public void Start(int zone)
        {
            if (zone < 1 || zone > 6)
                return;

            _timer.Start();

            _activeZone = zone;
        }

        public void Stop()
        {
            _timer.Stop();
            _activeZone = 0;
        }
    }
}
