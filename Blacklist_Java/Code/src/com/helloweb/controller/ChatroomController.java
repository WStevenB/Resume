package com.helloweb.controller;

import java.io.IOException;

import javax.servlet.RequestDispatcher;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;

import com.java.classes.ControllerTemplate;
import com.java.classes.User;
import com.java.classes.WebLookup;

@WebServlet("/ChatroomController")
public class ChatroomController extends ControllerTemplate {
	private static final long serialVersionUID = 1L;
  
    public ChatroomController() {
        super();
    }

    @Override
	protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
    	try {
    		//prepares user object
    		HttpSession session = request.getSession(false);
    		User user = new User(request, session);
    		
    		//check if too much activity is occurring
    		user.numberOfRequests++;
			if(user.numberOfRequests > user.maxRequests || !user.signedInFully.equals("1")) {
				String address = "index.jsp";
				RequestDispatcher rd = request.getRequestDispatcher(address);
				request.setAttribute("Error","You've performed too much activity on the website for now!");
				rd.forward(request, response);
				return;
			}
			//default navigation is chatroom
    		String address = "/WEB-INF/jsp/chatroom.jsp";
    		Integer actionCode = Integer.valueOf(request.getParameter("actionCode"));
    		switch(actionCode) {
    		//user adds new message to chat
    		case 1:
    			String message = request.getParameter("message");
    			insertMessage(message, user);
    			String targetId = user.ID;
        		if(targetId.equals("1")) targetId = user.chatPartner;   		
        		//user is not admin and has not sent any messages this session
        		//email will be sent to admin
        		else if(user.firstMessage.equals("1")) {
        			user.firstMessage = "0";
        			String info = user.username + " just sent you a message\r\n\r\n" + message + "\r\n\r\n\r\nEmail: " + user.email;
        			emailSender.sendEmail("walterstevenbabcock@gmail.com", "noReply@BabcockTechnologies.com", info, "New Message");
        		}
        		
        		//id to check for new messages
        		String flagId = targetId;
        		if(!user.ID.equals("1")) flagId = "A" + flagId;
        		
        		//adds flag to application context that a new message exists in this conversation
        		getServletContext().setAttribute(flagId, "1");
        		
        		//get relevant messages as html table
        		user.messages = getMessages(targetId);
        		
        		//admin has sent this message as an alert
        		//message added to list in ChatAlertsController
        		try {
        			String isAlert = request.getParameter("alertCheck");
        			if(isAlert.equals("on") || isAlert.equals("IsAlert")) {
        				String resp = postAlert(user.chatPartner, message);
        				System.out.println(resp);
        			}
        			else System.out.println(isAlert);
        		}
        		//debugging
        		catch(Exception err){
        			System.out.println("Alert-fail");
        			System.out.println(err.getMessage());
        		}		
    			break;
    		//user navigated back to homepage	
    		case 2:
    			address = "/WEB-INF/jsp/homepage.jsp";
    			break;
    		//admin changed conversation to a different user
    		case 3:
    			user.chatPartner = request.getParameter("partnerId").toString();
    			user.messages = getMessages(user.chatPartner);
    			break;
    			
    		//browser requested update on messages
    		case 901:
    			user.messages = getMessages(user.ID);
    			break;
    		}	
    		
    		//prepare response
    		request = user.prepareResponse(request);
    		session = user.updateSession(session);
    		RequestDispatcher rd =  request.getRequestDispatcher(address);
    		request.setAttribute("Error","");
    		rd.forward(request, response);
    	}
    	//unexpected error occurred
    	catch(Exception er) {
			String address = "index.jsp";
			RequestDispatcher rd = request.getRequestDispatcher(address);
			request.setAttribute("Error","Your session may have expired or something unexpected happened! Please try again...");
			rd.forward(request, response);
		}
		
	}

}
