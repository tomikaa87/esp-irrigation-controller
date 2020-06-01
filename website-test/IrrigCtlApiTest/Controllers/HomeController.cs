using System.IO;
using Microsoft.AspNetCore.Mvc;

namespace IrrigCtlApiTest.Controllers
{
    [Route("/")]
    public class HomeController : Controller
    {
        public IActionResult Index()
        {
            return Redirect("index.html");
        }
    }
}
