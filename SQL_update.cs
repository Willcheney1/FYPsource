using NLog;

using System;
using System.Data;
using System.Text;

using Microsoft.Data.SqlClient;
using uPLibrary.Networking.M2Mqtt;
using uPLibrary.Networking.M2Mqtt.Messages;





namespace FYP_Alert_Server
{
    public class SQL_update
    {
        //Variables for Worker_List
        public int Worker_ID { get; set; } 
        public String First_Name { get; set; } 
        public String Last_Name { get; set; }  
        public int Role_ID { get; set; }
        public String Mobile { get; set; }
        public String Email { get; set; }
        public String Worker_QR_Code { get; set; }

        //Variables for Tag_Fall for SQL
        public String Tag_MAC_Address { get; set; }
        public String Fall_Detected { get; set; }
        public String Fall_ID { get; set; }
        public String Orientation_ID { get; set; }
        public TimeSpan Fall_Time { get; set; }
        public DateTime Fall_Date { get; set; }

        //Variables for Tag_HR for SQL

      //public String Tag_MAC_Address { get; set; }
        public String Heartrate { get; set; }
        public String Motion_ID { get; set; }
        public TimeSpan HR_Time { get; set; }
        public DateTime HR_Date { get; set; }

        //Variables for Anchor_Geolocation
        public String Anchor_MAC_Address { get; set; }
        public String X_Coordinate { get; set; }
        public String Y_Coordinate { get; set; }
        public String Z_Coordinate { get; set; }

        //Variables for Anchor_Info

      //public String Anchor_MAC_Address { get; set; }
        public String Reference_Point { get; set; }
        public String Danger_Level { get; set; }

        //Variables for ESO_Contact

      //public String Worker_ID { get; set; }
      //public String Role_ID { get; set; }
        public String Contact_Info { get; set; }
        public String Time_Start { get; set; }
        public String Time_Stop { get; set; }
        public DateTime Date { get; set; }

        //Variables for Fall_Orientation

      //public String Orientation_ID { get; set; }
        public String Orientation_Description { get; set; }

        //Variables for Fall_Type

      //public String Fall_ID { get; set; }
        public String Severity { get; set; }

        //Variables for Motion_Type

      //public String Motion_ID { get; set; }
        public String Motion_Description { get; set; }

        //Variables for Roles

      //public String Role_ID { get; set; }
        public String Role_Description { get; set; }

        //Variables for Roster

        //public int Worker_ID { get; set; }
        //public String Time_Start { get; set; }
        //public String Time_Stop { get; set; }
        //public DateTime Date { get; set; }
        //public String Contact_Info { get; set; }

        //Variables for Tag_Info 

      //public String Tag_MAC_Address { get; set; }
        public String Tag_QR_Code { get; set; }




        private Logger log = LogManager.GetCurrentClassLogger();

        
        public void UploadTag_Fall(String Tag_MAC_Address, String Fall_Detected, String Fall_ID, String Orientation_ID, TimeSpan Fall_Time, DateTime Fall_Date)
        {
            //SQL
            Fall_Time = DateTime.Now.TimeOfDay;
            Fall_Date = DateTime.Today;


            /// For SQL - First connect to the Microsoft SQL Server
            /// "TrustServerCertificate" allows bypass of authentication by trusting the certificate without validation
            string connectionString = "Data Source=DESKTOP-5UQ2P7L\\SQLEXPRESS_2019;Initial Catalog=MAC_ADDRESS;Integrated Security=True;TrustServerCertificate=True";

           
            using (SqlConnection connection = new SqlConnection(connectionString))
            {
                try
                {
                    connection.Open();
                    log.Info("connected to SQL Server");
                    string insertQuery = "INSERT INTO Tag_Fall (Tag_MAC_Address, Fall_Detected, Fall_ID, Orientation_ID, Fall_Time, Fall_Date) VALUES (@Tag_MAC_Address, @Fall_Detected, @Fall_ID, @Orientation_ID, @Fall_Time, @Fall_Date)";
                    using (SqlCommand command = new SqlCommand(insertQuery, connection))
                    {
                        
                        command.Parameters.AddWithValue("@Tag_MAC_Address", Tag_MAC_Address);
                        command.Parameters.AddWithValue("@Fall_Detected", Fall_Detected);
                        command.Parameters.AddWithValue("@Fall_ID", Fall_ID);
                        command.Parameters.AddWithValue("@Orientation_ID", Orientation_ID);
                        command.Parameters.AddWithValue("@Fall_Time", Fall_Time);
                        command.Parameters.AddWithValue("@Fall_Date", Fall_Date);
                        command.ExecuteNonQuery();
                        log.Debug($"{@First_Name}");
                    }
                }
                catch (Exception ex)
                {
                    log.Warn("Connection failed: " + ex.Message);
                }                               
            }

            //SQL END
        }

        public void UploadTag_HR(String Tag_MAC_Address, String Heartrate, TimeSpan HR_Time, DateTime HR_Date, String Motion_ID)
        {
            HR_Time = DateTime.Now.TimeOfDay;
            HR_Date = DateTime.Today;

            string connectionString = "Data Source=DESKTOP-5UQ2P7L\\SQLEXPRESS_2019;Initial Catalog=MAC_ADDRESS;Integrated Security=True;TrustServerCertificate=True";

            using (SqlConnection connection = new SqlConnection(connectionString))
            {
                try
                {
                    connection.Open();
                    log.Info("connected to SQL Server");
                    string insertQuery = "INSERT INTO Tag_HR (Tag_MAC_Address, Heartrate, HR_Time, HR_Date, Motion_ID) VALUES (@Tag_MAC_Address, @Heartrate, @HR_Time, @HR_Date, @Motion_ID)";
                    using (SqlCommand command = new SqlCommand(insertQuery, connection))
                    {

                        command.Parameters.AddWithValue("@Tag_MAC_Address", Tag_MAC_Address);
                        command.Parameters.AddWithValue("@Heartrate", Heartrate);
                        command.Parameters.AddWithValue("@HR_Time", HR_Time);
                        command.Parameters.AddWithValue("@HR_Date", HR_Date);
                        command.Parameters.AddWithValue("@Motion_ID", Motion_ID);

                        command.ExecuteNonQuery();
                        log.Debug($"{@First_Name}");
                    }
                }
                catch (Exception ex)
                {
                    log.Warn("Connection failed: " + ex.Message);
                }
            }
        }
    }
}
