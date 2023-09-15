using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.Data.SqlClient;


namespace FYP_Alert_Server
{
    public class SQL_Upload
    {
        private string connectionString = "Data Source=DESKTOP-5UQ2P7L\\SQLEXPRESS_2019;Initial Catalog=MAC_ADDRESS;Integrated Security=True;TrustServerCertificate=True";

        public List<string> GetTableData()
        {
            List<string> data = new List<string>();

            using (SqlConnection connection = new SqlConnection(connectionString))
            {
                connection.Open();
                string query = "SELECT * FROM Worker_List"; // Adjust query and column name
                using (SqlCommand command = new SqlCommand(query, connection))
                using (SqlDataReader reader = command.ExecuteReader())
                {
                    while (reader.Read())
                    {
                        string rowData = "";

                        for (int i = 0; i < reader.FieldCount; i++)
                        {
                            if (!reader.IsDBNull(i))
                            {
                                rowData += reader.GetValue(i).ToString() + " ";
                            }
                            else
                            {
                                rowData += "NULL ";
                            }
                        }

                        data.Add(rowData.TrimEnd()); // Remove trailing space

                    }

                }
                return data;
            }
        }
    }
}
