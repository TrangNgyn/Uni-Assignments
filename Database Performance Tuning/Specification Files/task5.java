import java.sql.*;
import java.io.*;

class task5
{
  public static void main (String args [])
       throws SQLException, ClassNotFoundException
  {
    // Load the Oracle JDBC driver
    Class.forName ("oracle.jdbc.driver.OracleDriver");
    Connection conn = DriverManager.getConnection
       ("jdbc:oracle:thin:@localhost:1521:db",  "tpchr", "oracle");
      System.out.println( "Connected as CSCI317." ); 
  try{
	int count = 0;
	String pname;
        Statement stmt = conn.createStatement();
        ResultSet rset = stmt.executeQuery(
           "SELECT P_NAME " +
           "FROM PART " );

        while ( rset.next() )
	{
          pname = rset.getString(1);

	  BufferedReader in = new BufferedReader( new FileReader("task5.txt") );
	  String str;
	  while ( ( str = in.readLine() ) != null )
	  {
	    if (str.equals(pname)) 
		count++;
	  }
          in.close();
        }
        System.out.println( "Total: " + count );
        System.out.println( "Done." ); 
    }
   catch (SQLException e)
   {
     String errmsg = e.getMessage();
     System.out.println( errmsg );
   }
   catch (IOException io )
   {
     String errmsg = io.getMessage();
     System.out.println( errmsg );
   }

  }
}
