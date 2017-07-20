package com.database;

import java.sql.Connection;
import java.sql.DriverManager; 
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import com.database.DbParam;

//this class simplifies code for database processes
public class DatabaseHelper {
	
	//connection object
	public Connection conn;
	
	public DatabaseHelper() {
	}
	
	//opens connection to database
	//currently using sqlite, but can easily change this code for other dbms
	public void openDb() {
		try {
			Class.forName("org.sqlite.JDBC");
			conn = DriverManager.getConnection("jdbc:sqlite:sqlite444.db");
		}
		catch(Exception er){}
	}
	
	//closes database connection
	public void closeDb() {
		try {
			if(conn != null) conn.close();
		}
		catch(Exception er){}
	}
	
	//accepts a query and parameters to insert or update a database record
	//if fails (database might be busy) will recursively call itself to try again with count +=1
	//will stop trying after count >= 100
	//will open and close database
	 public String insertUpdate(String query, ArrayList<DbParam> parameters, int count) {
		 openDb();
		 try {
			 PreparedStatement prep = conn.prepareStatement(query);
			 for(int i = 0; i<parameters.size(); i++) {
				 if(parameters.get(i).type.equals("string")) prep.setString(i+1, parameters.get(i).value.toString());
				 else if(parameters.get(i).type.equals("integer")) prep.setInt(i+1, Integer.valueOf(parameters.get(i).value.toString()));
				 else if(parameters.get(i).type.equals("double")) prep.setDouble(i+1, Double.valueOf(parameters.get(i).value.toString()));
				 else if(parameters.get(i).type.equals("float")) prep.setFloat(i+1, Float.valueOf(parameters.get(i).value.toString()));
			 }
			 int rows = prep.executeUpdate();
			 if(prep != null) prep.close();
			 closeDb();			 
			 if(rows > 0) return "Success";
			 else return "Fail no rows";
		 }
		 catch(Exception er) {
			 if(count < 100) {
				 try {
					 closeDb();
					 Thread.sleep(50);
					 return insertUpdate(query, parameters, count +1);
				 }
				 catch(Exception err) {return "Fail";}
			 }
			 else {
				 System.out.println(er.getMessage());
				 closeDb();			
				 return "Fail ";
			 }
		 }
	}
	 
	 //method for database select statement, returns a result set
	 //recursively calls itself until count >= 100
	 //only opens database, close must be called by calling procedure after iterating through result set
	 public ResultSet select(String query, ArrayList<DbParam> parameters, int count) {
		 openDb();
		 try {
			 PreparedStatement prep = conn.prepareStatement(query);
			 for(int i = 0; i<parameters.size(); i++) {
				 if(parameters.get(i).type.equals("string")) prep.setString(i+1, parameters.get(i).value.toString());
				 else if(parameters.get(i).type.equals("integer")) prep.setInt(i+1, Integer.valueOf(parameters.get(i).value.toString()));
				 else if(parameters.get(i).type.equals("double")) prep.setDouble(i+1, Double.valueOf(parameters.get(i).value.toString()));
				 else if(parameters.get(i).type.equals("float")) prep.setFloat(i+1, Float.valueOf(parameters.get(i).value.toString()));
			 }
			 return prep.executeQuery();
		 }
		 catch(Exception er) {
			 if(count < 100) {
				 try {
					 closeDb();
					 Thread.sleep(50);
					 return select(query, parameters, count +1);
				 }
				 catch(Exception err) {
					 ResultSet results = null;
					 return results;
				 }
			 }
			 else {
				 System.out.println(er.getMessage());
				 ResultSet results = null;
				 return results; 
			 }
		 }
	 }
}
