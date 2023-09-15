using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.Data.SqlClient;


namespace FYP_Alert_Server
{
    internal class SQL_DAO
    {




        //  List<SQL_update> sql_updates = new List<SQL_update>();
        string connectionString = "Data Source=DESKTOP-5UQ2P7L\\SQLEXPRESS_2019;Initial Catalog=MAC_ADDRESS;Integrated Security=True;TrustServerCertificate=True";

        public List<SQL_update> getAllSQL_updates()
        {
            List<SQL_update> returnThese = new List<SQL_update>();

            using (SqlConnection connection = new SqlConnection(connectionString))
            {
                connection.Open();

                string query = "SELECT * FROM Tag_Fall"; // Adjust query and column name
                using (SqlCommand command = new SqlCommand(query, connection))

                using (SqlDataReader reader = command.ExecuteReader())
                {
                    while (reader.Read())
                    {
                        SQL_update a = new SQL_update
                        {
                            Tag_MAC_Address = reader.GetString(0),
                            Fall_Detected = reader.GetString(1),
                            Fall_ID = reader.GetString(2),
                            Orientation_ID = reader.GetString(3),
                            Fall_Time = reader.GetTimeSpan(4),
                            Fall_Date = reader.GetDateTime(5),
                            
                        };
                        SQL_update b = new SQL_update
                        {
                            Tag_MAC_Address = reader.GetString(0),
                            Heartrate = reader.GetString(1),
                            HR_Time = reader.GetTimeSpan(2),
                            HR_Date = reader.GetDateTime(3),
                            Motion_ID = reader.GetString(4),
                        };
                        returnThese.Add(a);
                        returnThese.Add(b);
                    }
                }
                connection.Close();

                return returnThese;
            }
        }        
    }
}
