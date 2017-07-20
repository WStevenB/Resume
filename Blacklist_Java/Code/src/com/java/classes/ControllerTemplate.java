package com.java.classes;


import java.io.IOException;
import java.util.ArrayList;
import java.util.Date;
import java.util.Random;

import javax.servlet.RequestDispatcher;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.java.classes.EmailSender;
import com.java.classes.PasswordHasher;
import com.java.classes.WebLookup;
import com.database.DatabaseHelper;
import com.database.DbParam;
import com.database.InputParser;

import java.sql.ResultSet;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import com.java.classes.User;


//all controllers on the website extend this class to avoid redundancy
//its properties assist in http, database, email, building html tables, and parsing submitted files

public class ControllerTemplate extends HttpServlet {
	private static final long serialVersionUID = 1L;
	
	WebLookup webHelper;
	DatabaseHelper dbHelper;
	protected EmailSender emailSender;
	PasswordHasher passwordHasher;
	protected TableBuilder tableBuilder;
	protected InputParser inputParser;

    public ControllerTemplate() {
        webHelper = new WebLookup();
        emailSender = new EmailSender();
        passwordHasher = new PasswordHasher();
        tableBuilder = new TableBuilder();
        dbHelper = new DatabaseHelper();
        inputParser = new InputParser();
    }
    
    
    //redirects to index.jsp to discourage users from typing in browser bar
    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
    	String address = "index.jsp";
		RequestDispatcher rd = request.getRequestDispatcher(address);
		request.setAttribute("Error","You've been signed out, and redirected to the landing page. Please don't try to self navigate through the website!");
		rd.forward(request, response);
	}


	protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		
	}
	
	
	//sends a new alert message from one controller to the ChatAlertsController to hold in memory
	protected String postAlert(String id, String message) {
		webHelper.parameters.add(id);
		webHelper.parameters.add(message);
		String resp = webHelper.getWebData("ChatAlertsController");
		return resp;
	}
    

    //hashes password and updates User table in database
	//called from ChangeCredentialsController after user forgot password
    protected String updateUser(User user) {
    	try {
    		String query = "UPDATE User SET PasswordHash = ? WHERE Email = ?;";
    		ArrayList<DbParam> parameters = new ArrayList<>();
    		PasswordHasher ph = new PasswordHasher();
        	parameters.add(new DbParam(ph.hash(user.password.toCharArray()), "string"));
    		parameters.add(new DbParam(user.email, "string"));
    		return dbHelper.insertUpdate(query, parameters, 0);
    	}
    	catch(Exception er) {
    		return "Fail";
    	}
	}
    
    //updates User table in database with new email and username
    //called from UpdateAccountController when user desires to change info
    protected String updateUserInfo(String email, String name, User user) {
    	try {
    		String query = "UPDATE User SET Email = ?, Name = ? WHERE ID = ?;";
    		ArrayList<DbParam> parameters = new ArrayList<>();
    		parameters.add(new DbParam(email, "string"));
    		parameters.add(new DbParam(name, "string"));
    		parameters.add(new DbParam(user.ID, "integer"));
    		return dbHelper.insertUpdate(query, parameters, 0);
    	}
    	catch(Exception er) {
    		return "Fail";
    	}
	}
    
    //updates all user info in database similar to above
    protected String updateWholeUser(User user) {
    	try {
    		String query = "UPDATE User SET PasswordHash = ?, Name = ?, Email = ? WHERE ID = ?;";
    		ArrayList<DbParam> parameters = new ArrayList<>();
    		parameters.add(new DbParam(user.password, "string"));
    		parameters.add(new DbParam(user.username, "string"));
    		parameters.add(new DbParam(user.email, "string"));
    		parameters.add(new DbParam(user.ID, "integer"));
    		return dbHelper.insertUpdate(query, parameters, 0);
    	}
    	catch(Exception er) {
    		return "Fail";
    	}
	}
    
    //updates User table to reflect account has been confirmed via email code
    protected String updateForConfirmation(User user) {
    	try {
    		String query = "UPDATE User SET IsConfirmed = '1' WHERE Email = ?;";
    		ArrayList<DbParam> parameters = new ArrayList<>();
    		parameters.add(new DbParam(user.email, "string"));
    		return dbHelper.insertUpdate(query, parameters, 0);
    	}
    	catch(Exception er) {
    		return "Fail";
    	}
    }
    
    //checks code submitted by user against stored code
    //if matches, then updates database
    //otherwise will return Fail
    protected String confirmCode(User user) {
    	try {
    		String query = "SELECT COUNT(1) AS 'c' FROM User WHERE Email = ? AND ConfirmCode = ?;";
    		ArrayList<DbParam> parameters = new ArrayList<>();
    		parameters.add(new DbParam(user.email, "string"));
        	parameters.add(new DbParam(user.confirmationCode, "string"));
        	ResultSet rs = dbHelper.select(query, parameters, 0);
        	if(!rs.next()) {
        		dbHelper.closeDb();
        		return "Fail";
        	}
        	Integer count = rs.getInt("c");
        	dbHelper.closeDb();
        	if(count != 1) return "Fail";
        	else {
        		return updateForConfirmation(user);
        	}
    	}
    	catch(Exception er) {
    		System.out.println(er.getMessage());
    		return "Fail";
    	}
	}
    
    //clears out any forgot password request for a particular user
    //prevents an attacker from building up tons of requests for a target email
    protected String deleteForgotCode(User user) {
    	try {
    		String query = "DELETE FROM ForgotPasswordRequest WHERE Email = ?;";
    		ArrayList<DbParam> parameters = new ArrayList<>();
    		parameters.add(new DbParam(user.email, "string"));
    		return dbHelper.insertUpdate(query, parameters, 0);
    	}
    	catch(Exception er) {
    		return "Fail";
    	}
    }
    
    //checks if the forgot password code matches the stored data
    //if matches, will allow user to reset account password
    protected String confirmForgotCode(User user) {
    	try {
    		String query = "SELECT COUNT(1) AS 'c' FROM ForgotPasswordRequest WHERE Email = ? AND Code = ?;";
    		ArrayList<DbParam> parameters = new ArrayList<>();
        	parameters.add(new DbParam(user.email, "string"));
        	parameters.add(new DbParam(user.confirmationCode, "string"));
        	ResultSet rs = dbHelper.select(query, parameters, 0);
        	if(!rs.next()) {
        		dbHelper.closeDb();
        		return "Fail";
        	}
        	Integer count = rs.getInt("c");
        	dbHelper.closeDb();
        	if(count != 1) return "Fail";
        	else {
        		return deleteForgotCode(user);
        	}
    	}
    	catch(Exception er) {
    		System.out.println(er.getMessage());
    		return "Fail";
    	}
    }
    
    //retrieves stored info from database for a particular user
    //checks if submitted password matches stored hash
    //checks if account has been confirmed via email code
    protected String getCredentials(User user) {
    	
    	try {
    		String query = "SELECT PasswordHash, Name, IsConfirmed, ID FROM User WHERE Email = ?;";
    		ArrayList<DbParam> parameters = new ArrayList<>();
        	parameters.add(new DbParam(user.email, "string"));
        	ResultSet rs = dbHelper.select(query, parameters, 0);
        	if(!rs.next()) {
        		dbHelper.closeDb();
        		return "NoEmail";
        	}
        	String passwordHash = rs.getString("PasswordHash");
        	user.username = rs.getString("Name");
        	user.ID = rs.getString("ID");
        	String isConfirmed = rs.getString("IsConfirmed");
        	dbHelper.closeDb();
        	PasswordHasher ph = new PasswordHasher();
    		if(ph.authenticate(user.password.toCharArray(), passwordHash)) {
    			if(isConfirmed.equals("1")) return "Success";
    			else return "Confirm";
    		}
    		else {
    			user.username = "";
    			user.ID = "";
    			return "NotPassword";
    		}
    	}
    	catch(Exception er) {
    		System.out.println(er.getMessage());
    		return "Fail";
    	}
    }
	
    //deletes any forgot password request for this user 
    //inserts a random number into database for use with a forgot password request
    protected String createCode(User user) {
    	deleteForgotCode(user);
    	try {
    		String query = "INSERT INTO ForgotPasswordRequest VALUES(?, ?);";
    		ArrayList<DbParam> parameters = new ArrayList<>();
    		parameters.add(new DbParam(user.email, "string"));
    		Random r = new Random();
    		int rand = r.nextInt(1000000) + 1000000;
    		parameters.add(new DbParam(String.valueOf(rand), "string"));
    		String result = dbHelper.insertUpdate(query, parameters, 0);
    		if(result.contains("Fail")) return "Fail";
    		return String.valueOf(rand);
    	}
    	catch(Exception er) {
    		return "Fail";
    	}
	}
	
	
	//uses regex to verify user submitted a valid email address during account creation
    protected boolean checkEmail(String email) {
		String ePattern = "^[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+@((\\[[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\])|(([a-zA-Z\\-0-9]+\\.)+[a-zA-Z]{2,}))$";
        java.util.regex.Pattern p = java.util.regex.Pattern.compile(ePattern);
        java.util.regex.Matcher m = p.matcher(email);
        return m.matches();
	}
	
    //inserts a new record into User database table
    //called from NewAccountController
    protected String insertAccount(User user) {
    	try {
    		String query = "INSERT INTO User (Email, Name, PasswordHash, ConfirmCode, IsConfirmed) VALUES (?,?,?,?,?);";
    		ArrayList<DbParam> parameters = new ArrayList<>();
    		parameters.add(new DbParam(user.email, "string"));
    		parameters.add(new DbParam(user.username, "string"));
        	PasswordHasher ph = new PasswordHasher();
        	parameters.add(new DbParam(ph.hash(user.password.toCharArray()), "string"));
    		Random r = new Random();
    		int rand = r.nextInt(1000000) + 1000000;
    		parameters.add(new DbParam(String.valueOf(rand), "string"));
    		parameters.add(new DbParam("0", "string"));
    		return dbHelper.insertUpdate(query, parameters, 0);
    	}
    	catch(Exception er) {
    		return "Fail";
    	}
	}
	
    //changes confirmation code stored in User table when unconfirmed sign in attempt occurs
    //this helps prevent an attacker from making tons of guesses
    protected String changeConfirmCode(User user) {	
		try {
    		String query = "UPDATE User SET ConfirmCode = ? WHERE Email = ?;";
    		ArrayList<DbParam> parameters = new ArrayList<>();
    		Random r = new Random();
    		int rand = r.nextInt(1000000) + 1000000;
    		parameters.add(new DbParam(String.valueOf(rand), "string"));
    		parameters.add(new DbParam(user.email, "string"));
    		String result = dbHelper.insertUpdate(query, parameters, 0);
    		if(result.contains("Fail")) return "Fail";
    		return String.valueOf(rand);
    	}
    	catch(Exception er) {
    		return "Fail";
    	}
	}
    
    
    //returns an html table containing the conversation of the user and website admin
    protected String getMessages(String targetId) {
    	try {
    		String query = "SELECT FromName, Message, DateSent FROM Chat WHERE FromId = ? OR ToId = ? ORDER BY ID DESC;";
    		ArrayList<DbParam> parameters = new ArrayList<>();
        	parameters.add(new DbParam(targetId, "string"));
        	parameters.add(new DbParam(targetId, "string"));
        	ResultSet results = dbHelper.select(query, parameters, 0);
        	ArrayList<String> headers = new ArrayList<>();
        	headers.add("From");
        	headers.add("Message");
        	headers.add("Date");
        	String messages = tableBuilder.buildTableNonClickable(headers, results);
        	dbHelper.closeDb();
    		return messages;
    	}
    	catch(Exception er) {
    		System.out.println(er.getMessage());
    		dbHelper.closeDb();
    		return "An error occurred...";
    	}
    }
	
    //inserts a new record into the Chat database table
    protected String insertMessage(String message, User user) {	
		try {
    		String query = "INSERT INTO Chat (FromId, ToId, FromName, ToName, Message, DateSent) VALUES (?,?,?,(SELECT Name FROM User WHERE ID = ?),?,?);";
    		ArrayList<DbParam> parameters = new ArrayList<>();
    		parameters.add(new DbParam(user.ID, "integer"));
    		parameters.add(new DbParam(user.chatPartner, "integer"));
    		parameters.add(new DbParam(user.username, "string"));
    		parameters.add(new DbParam(user.chatPartner, "integer"));
    		parameters.add(new DbParam(message, "string"));
    		DateFormat dateFormat = new SimpleDateFormat("MM/dd/yyyy HH:mm");
    		Date date = new Date();
    		String dateString = dateFormat.format(date);
    		parameters.add(new DbParam(dateString, "string"));
    		String result = dbHelper.insertUpdate(query, parameters, 0);
    		if(result.contains("Fail")) return "Fail";
    		return "Success";
    	}
    	catch(Exception er) {
    		return "Fail";
    	}
	}
    
    //returns an html table containing the different users who have chatted with the website admin
    //this table is clickable and will call the above getMessages() for each row
    protected String getChatPartners() {
    	try {
    		String query = "WITH sq AS ( " +
    						"SELECT FromId FROM Chat WHERE ToId = 1 " +
    						"UNION " +
    						"SELECT ToId FROM Chat WHERE FromId = 1 " +
    						") " +
    						"SELECT DISTINCT sq.FromId, User.Name, User.Email FROM sq, User WHERE sq.FromId = User.ID AND sq.FromId != 1;";
    		ArrayList<DbParam> parameters = new ArrayList<>();
        	ResultSet results = dbHelper.select(query, parameters, 0);
        	ArrayList<String> headers = new ArrayList<>();
        	headers.add("Id");
        	headers.add("Name");
        	headers.add("Email");
        	ArrayList<String> hiddens = new ArrayList<>();
        	hiddens.add("partnerId");
        	hiddens.add("partnerName");
        	hiddens.add("partnerEmail");
        	String partners = tableBuilder.buildTableClickable("ChatroomController", headers, hiddens, "3", results);
        	dbHelper.closeDb();
    		return partners;
    	}
    	catch(Exception er) {
    		System.out.println(er.getMessage());
    		dbHelper.closeDb();
    		return "An error occurred...";
    	}
    }
    
    //occurs when user uploads respondent file to server
    //inputParser builds query string for Blacklist database table
    //two result sets are generated, one for file download and another for web display
    protected void getMatches(User user) {
    	
    	try {
    		String query = inputParser.buildQueryForCount();
    		if(query.equals("Error")) user.matchFileContents = "Fail";
    		ArrayList<DbParam> parameters = new ArrayList<>();
        	ResultSet rs = dbHelper.select(query, parameters, 0);
        	ResultSet rsCopy = dbHelper.select(query, parameters, 0);
        	int matchCount = 0;
        	user.matchTableContents = "";
        	user.matchFileContents = "";
        	user.matchCount = "";
        	while(rs.next()) {
        		matchCount++;
        		user.matchFileContents += rs.getString("FirstName") + "\t" + rs.getString("LastName") + "\t" + rs.getString("Email") + "\t" +
        				rs.getString("Address") + "\t" + rs.getString("City") + "\t" + rs.getString("State") + "\t" +
        				rs.getString("Zip") + "\t" + rs.getString("Phone") + "\t" + rs.getString("DateAdded") + "\t" + rs.getString("Comments") + "\n";
        	}
        	
        	ArrayList<String> headers = new ArrayList<>();
        	headers.add("First Name");
        	headers.add("Last Name");
        	headers.add("Email Address");
        	headers.add("Street Address");
        	headers.add("City");
        	headers.add("State");
        	headers.add("Zip Code");
        	headers.add("Phone Number");
        	headers.add("Date Added");
        	headers.add("Comments");
        	user.matchTableContents = tableBuilder.buildTableNonClickable(headers, rsCopy);
        	user.matchCount = String.valueOf(matchCount);
        	dbHelper.closeDb();
    	}
    	catch(Exception er) {
    		System.out.println(er.getMessage());
    		user.matchFileContents = "Fail";
    		user.matchTableContents = "";
    	}
    }
    
}

