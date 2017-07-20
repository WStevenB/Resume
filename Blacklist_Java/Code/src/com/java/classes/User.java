package com.java.classes;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpSession;

//contains relevant info about a user of the website
//beginning of each controllers' POST: User properties are populated from both the http request and session
//end of controllers' POST: methods will update the request and session after some computation is performed
//this class eliminates many redundancies of putting the right fields in requests, and streamlines access to request/session data
//not every property is placed in the session, and not every property is placed in requests - only the relevant ones to each
//eg password is only populated from request, numberOfRequests is only stored is session, while username is involved in everything
public class User {
	
	public Integer numberOfRequests = 0;
	public Integer maxRequests = 150;
	public Integer accountsCreated = 0;
	public Integer passwordsGuessed = 0;
	public Integer forgotRequests = 0;
	public String sessionID = "";
	public String ID = "";
	public String email = "";
	public String username = "";
	public String password;
	public String confirmPassword = "";
	public String rowNum = "";
	public String confirmationCode = "";
	public String chatPartner = "1";
	public String partners = "";
	public String messages = "";
	public String firstMessage = "1";
	public String matchCount = "";
	public String matchFileContents = "";
	public String matchTableContents = "";
	public String alertCheckbox = "";
	public String signedInFully = "0";
	
	//populates properties given an http request and session
	//some properties from the session may be overwritten if a new value comes in, eg - chatPartner
	//so request is processed second
	public User(HttpServletRequest request, HttpSession session) {
		if(session.getAttribute("numberOfRequests") != null) numberOfRequests = Integer.valueOf(session.getAttribute("numberOfRequests").toString());
		if(session.getAttribute("accountsCreated") != null) accountsCreated = Integer.valueOf(session.getAttribute("accountsCreated").toString());
		if(session.getAttribute("passwordsGuessed") != null) passwordsGuessed = Integer.valueOf(session.getAttribute("passwordsGuessed").toString());
		if(session.getAttribute("forgotRequests") != null) forgotRequests = Integer.valueOf(session.getAttribute("forgotRequests").toString());
		if(session.getAttribute("ID") != null) ID = session.getAttribute("ID").toString();
		if(session.getAttribute("sessionID") != null) sessionID = session.getAttribute("sessionID").toString();
		if(session.getAttribute("email") != null) email = session.getAttribute("email").toString();
		if(session.getAttribute("username") != null) username = session.getAttribute("username").toString();
		if(session.getAttribute("confirmationCode") != null) confirmationCode = session.getAttribute("confirmationCode").toString();
		if(session.getAttribute("chatPartner") != null) chatPartner = session.getAttribute("chatPartner").toString();
		if(session.getAttribute("partners") != null) partners = session.getAttribute("partners").toString();
		if(session.getAttribute("messages") != null) messages = session.getAttribute("messages").toString();
		if(session.getAttribute("firstMessage") != null) firstMessage = session.getAttribute("firstMessage").toString();
		if(session.getAttribute("signedInFully") != null) signedInFully = session.getAttribute("signedInFully").toString();
		
		
		if(request.getParameterMap().containsKey("sessionID")) sessionID = request.getParameter("sessionID");
		if(request.getParameterMap().containsKey("email")) {
			if(email.equals("")) email = request.getParameter("email");
		}
		if(request.getParameterMap().containsKey("username"))username = request.getParameter("username");
		if(request.getParameterMap().containsKey("password"))password = request.getParameter("password");
		if(request.getParameterMap().containsKey("confirmPassword"))confirmPassword = request.getParameter("confirmPassword");
		if(request.getParameterMap().containsKey("rowNum"))rowNum = request.getParameter("rowNum");
		if(request.getParameterMap().containsKey("confirmationCode"))confirmationCode = request.getParameter("confirmationCode");
		if(request.getParameterMap().containsKey("chatPartner") && ID.equals("1")) chatPartner = request.getParameter("chatPartner");
		
		if(ID.equals("1")) alertCheckbox = "<input type=\"checkbox\" name=\"alertCheck\" value\"IsAlert\">Send as alert";
	}
	
	//places relevant properties in scope of http response
	public HttpServletRequest prepareResponse(HttpServletRequest request) {
		request.setAttribute("sessionID",sessionID);
		request.setAttribute("email",email);
		request.setAttribute("username",username);
		request.setAttribute("confirmationCode",confirmationCode);
		request.setAttribute("partners",partners);
		request.setAttribute("messages",messages);
		request.setAttribute("alertCheckbox", alertCheckbox);
		
		return request;
	}
	
	//updates session values because some computation may be happened elsewhere
	public HttpSession updateSession(HttpSession session) {
		session.setAttribute("numberOfRequests", String.valueOf(numberOfRequests));
		session.setAttribute("accountsCreated", String.valueOf(accountsCreated));
		session.setAttribute("passwordsGuessed", String.valueOf(passwordsGuessed));
		session.setAttribute("forgotRequests", String.valueOf(forgotRequests));
		session.setAttribute("ID", ID);
		session.setAttribute("sessionID", sessionID);
		session.setAttribute("email", email);
		session.setAttribute("username", username);
		session.setAttribute("confirmationCode", confirmationCode);
		session.setAttribute("chatPartner", chatPartner);
		session.setAttribute("partners", partners);
		session.setAttribute("messages", messages);
		session.setAttribute("firstMessage", firstMessage);
		session.setAttribute("signedInFully", signedInFully);
		
		return session;
	}
	
	//occurs when user signs out of website
	public HttpSession clearSession(HttpSession session) {
		session.setAttribute("ID", "");
		session.setAttribute("sessionID", "");
		session.setAttribute("email", "");
		session.setAttribute("username", "");
		session.setAttribute("confirmationCode", "");
		session.setAttribute("rowNum", "");
		session.setAttribute("chatPartner", "1");
		session.setAttribute("partners", "");
		session.setAttribute("messages", "");
		session.setAttribute("firstMessage", "");
		session.setAttribute("matchCount", "");
		session.setAttribute("matchFileContents", "");
		session.setAttribute("matchTableContents", "");
		session.setAttribute("signedInFully", "0");
		
		return session;
	}
}
