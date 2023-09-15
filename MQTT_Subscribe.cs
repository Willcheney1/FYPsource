using NLog;
using QRCoder;
using System.Data;
using System.Text;
using System.Windows.Forms;
using uPLibrary.Networking.M2Mqtt;
//code windows packages:
using uPLibrary.Networking.M2Mqtt.Messages;

namespace FYP_Alert_Server
{
    public class MQTT_Subscribe
    {
        private static MqttClient client;
        private Logger log = LogManager.GetCurrentClassLogger();
        private int MQTT_Counter = 0;

        //Variables for Tag_Fall table SQL

        public String Tag_MAC_Address;
        public String Fall_Detected;
        public String Fall_ID;
        public String Orientation_ID;
        public TimeSpan Fall_Time;
        public DateTime Fall_Date;

        //Variables for Tag_HR table SQL

     // public String Tag_MAC_Address;
        public String Heartrate;
        public String Motion_ID;
        public TimeSpan HR_Time;
        public DateTime HR_Date;

        public string topic;
        public string message;
        private SQL_update sqlUpdateInstance;
        public void InitializeMqttConnection()
        {

            string brokerAddress = "192.168.0.107";
            int brokerPort = 1883; // Default MQTT port
            string clientId = Guid.NewGuid().ToString();

            client = new MqttClient(brokerAddress, brokerPort, false, null, null, MqttSslProtocols.None);

            // Handle message received event
            client.MqttMsgPublishReceived += MqttClient_MqttMsgPublishReceived;

            // Connect to MQTT broker
            client.Connect(clientId, "mqtt", "willcheney");

            // Subscribe to the "Distance" topic
            client.Subscribe(new string[] { "#" }, new byte[] { MqttMsgBase.QOS_LEVEL_AT_LEAST_ONCE });
        }



        /// <summary>
        /// 
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void MqttClient_MqttMsgPublishReceived(object sender, MqttMsgPublishEventArgs e)
        {
            ///try and catch

            topic = e.Topic;
            message = Encoding.Default.GetString(e.Message);

            sqlUpdateInstance = new SQL_update();
           // sqlUpdateInstance.UploadMessageToDatabase(message);
            // Display the received message in a message box
            log.Debug($"Received message on topic '{topic}': {message}");

            //Counter for Messages passing through MQTT
            MQTT_Counter++;

            //switch statement #TODO 

            //change "2" to a variable (global constant)

            //Determine which column the data belongs in from the topic
            Tag_Fall();
            Tag_HR();
        }
        public int GetMQTTCounter()
        {
            return MQTT_Counter;
            
        }

        public void Tag_Fall()
        {
            if (topic == "Fall_Detected")
            {
                Fall_Detected = message;
                log.Debug($"{message}");
            }
            else if (topic == "Fall_Detected/Tag_MAC_Address")
            {
                Tag_MAC_Address = message;
                log.Debug($"{message}");
            }
            else if (topic == "Fall_Detected/Fall_ID")
            {
                Fall_ID = message;
                log.Debug($"{message}");
            }
            else if (topic == "Fall_Detected/Orientation_ID")
            {
                Orientation_ID = message;
                log.Debug($"{message}");
            }

            sqlUpdateInstance.UploadTag_Fall($"{Tag_MAC_Address}", $"{Fall_Detected}", $"{Fall_ID}", $"{Orientation_ID}", Fall_Time, Fall_Date);

        }

        public void Tag_HR()
        {
            if (topic == "Heartrate")
            {
                Heartrate = message;
                log.Debug($"{message}");
            }
            else if (topic == "Heartrate/Tag_MAC_Address")
            {
                Tag_MAC_Address = message;
                log.Debug($"{message}");
            }
            else if (topic == "Heartrate/Motion_ID")
            {
                Motion_ID  = message;
                log.Debug($"{message}");
            }
            sqlUpdateInstance.UploadTag_HR($"{Tag_MAC_Address}", $"{Heartrate}", HR_Time, HR_Date, $"{Motion_ID}");

        }
    }


    
}
