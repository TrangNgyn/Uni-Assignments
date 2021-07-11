import java.sql.*;

class task4
{
  public static void main (String args [])
       throws SQLException, ClassNotFoundException
  {
    // Load the Oracle JDBC driver
    Class.forName ("oracle.jdbc.driver.OracleDriver");
    Connection conn = DriverManager.getConnection
       ("jdbc:oracle:thin:@localhost:1521:db",  "tpchr", "oracle");
      System.out.println( "Connected as tpchr user");
  try{
	PreparedStatement pstmt1 = conn.prepareStatement( 
          "CREATE TABLE BRAND55 AS " +
            "(SELECT * FROM PART WHERE P_BRAND= 'Brand#55')" );
	pstmt1.execute();

	PreparedStatement pstmt2 = conn.prepareStatement( 
           "CREATE TABLE NICKEL AS " +
             "(SELECT * FROM PART WHERE P_TYPE= 'ECONOMY BRUSHED NICKEL')" );
	pstmt2.execute();

	PreparedStatement pstmt12 = conn.prepareStatement( 
           "CREATE TABLE TEMP12 AS " +
             "(SELECT * " +
             " FROM BRAND55 " +
             " WHERE NOT EXISTS " +
             "           (SELECT * " +
             "            FROM NICKEL " + 
             "            WHERE BRAND55.P_PARTKEY = NICKEL.P_PARTKEY) )" );
	pstmt12.execute();

        Statement stmt = conn.createStatement();
        ResultSet rset = stmt.executeQuery(
           "SELECT P_PARTKEY " +
           "FROM BRAND55 " +
           "WHERE P_PARTKEY NOT IN " +
           "      ( SELECT P_PARTKEY " +
           "        FROM TEMP12 ) "+
           "ORDER BY P_PARTKEY" );

        while ( rset.next() )
          System.out.println("Part key: " + rset.getInt(1) );

	stmt.executeUpdate("DROP TABLE BRAND55 PURGE");
        stmt.executeUpdate("DROP TABLE NICKEL PURGE");
        stmt.executeUpdate("DROP TABLE TEMP12 PURGE");

        System.out.println( "Done." ); 
    }
   catch (SQLException e )
   {
     String errmsg = e.getMessage();
     System.out.println( errmsg );
   }
  }
}
