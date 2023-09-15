using NLog;
using NLog.Config;
using NLog.Targets;
using NLog.Fluent;
using System;
using System.Data;
using System.Text;
using QRCoder;
using Microsoft.Data.SqlClient;
using uPLibrary.Networking.M2Mqtt;
using uPLibrary.Networking.M2Mqtt.Messages;
using System.Windows.Forms;
using Microsoft.Extensions.Configuration.Ini;
//code windows packages:
using System.Threading;
using System;
using Microsoft.VisualBasic.ApplicationServices;
using Microsoft.VisualBasic.Devices;
using static System.Net.WebRequestMethods;
using Microsoft.Identity.Client;
using LogLevel = NLog.LogLevel;
using System.Security.Principal;

namespace FYP_Alert_Server
{
    internal class FTP
    {

        public void FTP_Write(string url, string data)
        {
            //File system

            string filePath = @"C:\CoreFTPServer\ftp\DistanceRecords.txt"; // Path to the file


            // Open the file and append additional content to it
            using (StreamWriter writer = System.IO.File.AppendText(filePath))
            {
                DateTime currentTime = DateTime.Now;
              //  writer.WriteLine(currentTime.ToString() + $"     ID: {S_identity}     {message}m     {anchor} ");
                //writer.WriteLine($"Received:{message}");
            }

            Console.WriteLine("Content appended to the file successfully.");

            //File System END
        }
    }
}
