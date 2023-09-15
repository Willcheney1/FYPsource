
//custom nuget packages: 
using NLog;
using QRCoder;
using System.Data;
using System.Text;
using System.Windows.Forms;
using uPLibrary.Networking.M2Mqtt;
//code windows packages:
using uPLibrary.Networking.M2Mqtt.Messages;
using System;
namespace FYP_Alert_Server
{


    public partial class Frm_AlertServer_UI : Form
    {
        // private static MqttClient client;
        private DataSet dtSet;
        // NLog.Logger log;
        private static MqttClient client;
        private Logger log = LogManager.GetCurrentClassLogger();
        private SQL_Upload sqlUpload = new SQL_Upload();
        #region Frm_AlertServer_UI_Core

        BindingSource SQLBindingSource = new BindingSource();
        /// <summary>
        /// 
        /// </summary>
        public Frm_AlertServer_UI()
        {

            InitializeComponent();

            GlobalDiagnosticsContext.Set("Version", "1.0.42");

        }



        // ...

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Form1_Load(object sender, EventArgs e)
        {


        }

        #endregion

        #region Frm_AlertServer_UI_Events


        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnTest1_Click(object sender, EventArgs e)
        {
            try
            {
                //eg1
                log.Debug("Test1 button clicked message");

                //eg2
                var msg1 = new LogEventInfo(LogLevel.Info, "", "Executing Button Press");
                var msg2 = new LogEventInfo(LogLevel.Trace, "", "Button Press Executed");
                msg1.Properties.Add("User", "Ray Donovan");
                msg2.Properties.Add("User", "Ray Donovan");
                log.Info(msg1);
                log.Warn(msg2);
                //eg3
                Console.WriteLine("Form: btn press.");

                SQL_DAO sQL_DAO = new SQL_DAO();

                SQLBindingSource.DataSource = sQL_DAO.getAllSQL_updates();

                dataGridView1.DataSource = SQLBindingSource;




                // MQTT mqttClass = new MQTT();
                //mqttClass.MQTT_Subscribe();

                //SQL

                /* For SQL - First connect to the Microsoft SQL Server
                 * "TrustServerCertificate" allows bypass of authentication by trusting the certificate without validation
                */
                /*string connectionString = "Data Source=DESKTOP-5UQ2P7L\\SQLEXPRESS_2019;Initial Catalog=MAC_ADDRESS;Integrated Security=True;TrustServerCertificate=True";

                using (SqlConnection connection = new SqlConnection(connectionString))
                {
                    try
                    {
                        connection.Open();
                        log.Info("connected to SQL Server");
                        connection.Close();
                    }
                    catch (Exception ex)
                    {
                        log.Warn("Connection failed: " + ex.Message);
                    }
                }*/

                //SQL END

                //QR code Generator

                /* QR code generator from QRCoder package. can use ZXing.NET if needed
                 * To generate all that is required is a file path and data
                 * This snippet uses the "Internal Class QrCodeGenerator"
                 */
                string data = "Hello, this is a QR code generated using QRCoder!";
                string filePath = @"C:\Users\will1\Documents\Visual Studio 2022\FYP\QR.png";

                QrCodeGenerator qrCodeGenerator = new QrCodeGenerator();
                qrCodeGenerator.GenerateQrCode(data, filePath);

                //QR code Generator END


                //MQTT 

                // REMEBER TO CHANGE IP ADDRESS EACH TIME

                /* MqttClient (string brokerHostName, int brokerPort, bool secure, X509Certificate caCert, X509Certificate clientCert, MqttSslProtocols sslProtocol)
                 * Guid - a globally unique identifier
                 * MqttMsgPublishReceived - the += is used to subscribe to an event
                 * (s,e) is lambda expression which represent sender and event arguments
                 * the 's' would refer to the MQTT client
                 * the 'e' would contain additional information about the received message - topic, payload
                 */
                /* log.Debug("start mqtt");
                 string clientId = Guid.NewGuid().ToString();
                 client = new MqttClient("172.20.10.2", 1883, false, null, null, MqttSslProtocols.None);

                 client.MqttMsgPublishReceived += (s, e) =>
                 {
                     int identity = 10452;
                     string S_identity = identity.ToString();
                     string anchor = "86.107.34.4";
                     string topic = e.Topic;
                     string message = Encoding.Default.GetString(e.Message);
                     Console.WriteLine($"Received:{message}");
                     log.Warn($"distance in meters = {message}");

                     //File system

                     string filePath = @"C:\CoreFTPServer\ftp\DistanceRecords.txt"; // Path to the file


                     // Open the file and append additional content to it
                     using (StreamWriter writer = System.IO.File.AppendText(filePath))
                     {
                         DateTime currentTime = DateTime.Now;
                         writer.WriteLine(currentTime.ToString() + $"     ID: {S_identity}     {message}m     {anchor} ");
                         //writer.WriteLine($"Received:{message}");
                     }

                     Console.WriteLine("Content appended to the file successfully.");

                     //File System END
                 };

                 client.Connect(clientId, "mqtt", "willcheney");

                 client.Subscribe(new string[] { "Distance" }, new byte[] { MqttMsgBase.QOS_LEVEL_AT_LEAST_ONCE });*/

                //MQTT END







            }
            catch (Exception ex)
            {
                //eg3
                log.Error(ex, "Ex: btnTest1");
            }
        }


        /*
        static void CreateFile(string filePath)
        {
            // Create a new file or overwrite if it already exists
            using (StreamWriter writer = new StreamWriter(filePath))
            {
                writer.WriteLine("Hello, World!");
                writer.WriteLine("This is an example file.");
            }

            Console.WriteLine("File created successfully.");
        }
        

        static void AppendToFile(string filePath)
        {
            // Open the file and append content to it
            
        }
        

        static void ReadFile(string filePath)
        {
            // Read and display the content of the file
            using (StreamReader reader = new StreamReader(filePath))
            {
                string line;
                while ((line = reader.ReadLine()) != null)
                {
                    Console.WriteLine(line);
                }
            }
        }
        */



        #endregion

        #region QR Code
        //QR code Function

        /* QRCodeData object is created using the CreateQrCode method of the QRCodeGenerator class.
         * QRCodeGenerator.ECCLevel.Q, which specifies the error correction level for the QR code. -
         * In this case, it's set to the Q level, which offers a good balance between error correction and data capacity.
         */
        internal class QrCodeGenerator
        {
            public void GenerateQrCode(string data, string filePath)
            {

                QRCodeGenerator qrGenerator = new QRCodeGenerator();
                QRCodeData qrCodeData = qrGenerator.CreateQrCode(data, QRCodeGenerator.ECCLevel.Q);
                QRCode qrCode = new QRCode(qrCodeData);

                Bitmap qrCodeImage = qrCode.GetGraphic(10); // Adjust the size (10 here) based on your requirements

                // Save the QR code image to a file
                qrCodeImage.Save(filePath, System.Drawing.Imaging.ImageFormat.Png);
            }
        }

        #endregion

        #region Frm_AlertServer_Thread_Comms_Anchor





        private void btnThreadStart_CommsAnchor_Click(object sender, EventArgs e)
        {
            try
            {
                log.Warn("Main thread: Start a second thread.");
                // The constructor for the Thread class requires a ThreadStart
                // delegate that represents the method to be executed on the
                // thread.  C# simplifies the creation of this delegate.
                Thread t = new Thread(new ThreadStart(ThreadProc_CommsAnchor));

                // Start ThreadProc.  Note that on a uniprocessor, the new
                // thread does not get any processor time until the main thread
                // is preempted or yields.  Uncomment the Thread.Sleep that
                // follows t.Start() to see the difference.
                t.Start();
                //Thread.Sleep(0);

                for (int i = 0; i < 4; i++)
                {
                    Console.WriteLine("Main thread: Do some work.");
                    Thread.Sleep(0);
                }

            }
            catch (Exception ex)
            {
                log.Error(ex, "Ex: btnThreadStart_CommsAnchor_Click");
            }
        }


        // The ThreadProc method is called when the thread starts.
        // It loops ten times, writing to the console and yielding
        // the rest of its time slice each time, and then ends.

        public static bool ThreadProc_CommsAnchor_enable = true;
        public static void ThreadProc_CommsAnchor()
        {

            // likely need to need seperate instance of log (nlog) for each thread
            // you need research this: 
            // ref: https://stackoverflow.com/questions/35131659/logging-multiple-threads-with-nlog
            NLog.Logger log = LogManager.GetCurrentClassLogger();

            int i = 0;

            while (ThreadProc_CommsAnchor_enable)
            {
                try
                {

                    log.Debug(string.Format("ThreadProc: Comms Anchor  pass:{0}", i));




                    if (i > (int.MaxValue - 1))
                    {
                        i = 0;
                    }
                    i = i + 1;
                }
                catch (Exception ex)
                {
                    log.Error(ex, "");
                }

                // Yield the rest of the time slice.
                Thread.Sleep(500);
            }
        }

        #endregion
        #region





        #endregion



        public void Frm_AlertServer_UI_FormClosing(object sender, FormClosingEventArgs e)
        {
            try
            {
                //shutdown thread(s) via semaphore
                ThreadProc_CommsAnchor_enable = false;


                //wait until confirm thread closed


                log.Debug("Alert Server Shutdown");

            }
            catch (Exception ex)
            {
                //No NLog        
            }
        }

        private void dataGridView1_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {

        }
    }

    internal class QrCodeGenerator
    {
        public QrCodeGenerator()
        {
        }
    }
}