
using NLog.Config;
using NLog.Targets;
using NLog;

using uPLibrary.Networking.M2Mqtt;
using uPLibrary.Networking.M2Mqtt.Messages;
using System.Threading;
using System;
using System.Windows.Forms;

namespace FYP_Alert_Server

{
  class Initialise
  {
        
        /// <summary>
        ///  The main entry point for the application.
        private static Logger log = LogManager.GetCurrentClassLogger();

        
        /// </summary>
        /// 

        [STAThread]
    static void Main()
    {

            //TestSql();
            
            

           


            MQTT_Subscribe mqttSubscriber = new MQTT_Subscribe();
            mqttSubscriber.InitializeMqttConnection();

            SQL_Upload sqlUploader = new SQL_Upload();
            SQL_DAO sqlDAO = new SQL_DAO();

            var config = new LoggingConfiguration();
            var consoleTarget = new ConsoleTarget
            {
                Name = "console",
                Layout = "${longdate}|${level:uppercase=true}|${logger}|${message}",
            };
         
            log.Info("Application started.");
            
            // To customize application configuration such as set high DPI settings or default font,
            // see https://aka.ms/applicationconfiguration.
            ApplicationConfiguration.Initialize();
            Application.Run(new Frm_AlertServer_UI());
           
            
            int MQTT_Counter_Value = mqttSubscriber.GetMQTTCounter();
            log.Debug($"MQTT Counter: {MQTT_Counter_Value}");

    }
        
  }
}