using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using IrrigCtlApiTest.Services;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;

namespace IrrigCtlApiTest.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class ZoneController : ControllerBase
    {
        private readonly IZoneService _zoneService;

        public ZoneController(IZoneService zoneService)
        {
            _zoneService = zoneService;
        }

        [HttpGet("{id?}/start")]
        public IActionResult Start(int id)
        {
            if (id < 1 || id > 6)
            {
                return BadRequest();
            }

            _zoneService.Start(id);

            return NoContent();
        }

        [HttpGet("stop")]
        public IActionResult Stop()
        {
            _zoneService.Stop();

            return NoContent();
        }

        [HttpGet("status")]
        public ZoneStatusModel Status()
        {
            return new ZoneStatusModel()
            {
                ActiveZone = _zoneService.GetActiveZone(),
                TankLevel = _zoneService.GetTankLevel(),
                FirmwareVersion = "1.0.0"
            };
        }
    }
}
