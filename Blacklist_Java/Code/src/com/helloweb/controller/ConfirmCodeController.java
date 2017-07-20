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


@WebServlet("/ConfirmCodeController")
public class ConfirmCodeController extends ControllerTemplate {
	private static final long serialVersionUID = 1L;


    public ConfirmCodeController() {
        super();
    }



    @Override
	protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
    	try {
    		//prepare user object
    		HttpSession session = request.getSession(false);
    		User user = new User(request, session);
    		
    		//check for too much activity
    		user.numberOfRequests++;
			if(user.numberOfRequests > user.maxRequests) {
				String address = "index.jsp";
				RequestDispatcher rd = request.getRequestDispatcher(address);
				request.setAttribute("Error","You've performed too much activity on the website for now!");
				rd.forward(request, response);
				return;
			}
			//default navigation is confirm code page
    		String address = "/WEB-INF/jsp/confirmCode.jsp";
    		String error = "";
    		Integer actionCode = Integer.valueOf(request.getParameter("actionCode"));
    		
    		switch(actionCode) {
    		//user has sent code to check
    		case 1:
    			request.setAttribute("ConfirmButton", "<button class=\"_button _button-2\" type=\"submit\" name=\"actionCode\" value=\"1\">Submit</button>");
    			String resp = confirmCode(user);
    			//success - send continue button back
    			if(resp.equals("Success")) {
    				error = "Your account has been confirmed!";
    				request.setAttribute("ConfirmButton", "");
    				request.setAttribute("ConfirmButton2", "<button class=\"_button _button-2\" type=\"submit\" name=\"actionCode\" value=\"2\">Continue</button>");
    			}
    			//fail - send error message back
    			else {
    				error = "That was not the code we sent. Please check your email and try again.";
    			}
    			break;
    		//user continues to homepage
    		case 2:
    			user.signedInFully = "1";
    			address = "/WEB-INF/jsp/homepage.jsp";
    			String targetId = user.ID;
    			//user is admin - default chat partner is second user
        		if(targetId.equals("1")) {
        			user.chatPartner = "2";
        			targetId = "2";
        			user.partners = getChatPartners();
        			user.alertCheckbox = "<input type=\"checkbox\" name=\"alertCheck\" value\"IsAlert\">Send as alert";
        		}
        		//user is not admin - send email to admin that someone has signed in
        		else {
        			String info = user.username + " has just logged in\r\n\r\nEmail: " + user.email;
        			emailSender.sendEmail("walterstevenbabcock@gmail.com", "noReply@BabcockTechnologies.com", info, "User Login");
        		}
    			error = "";
    			break;
    		}
    		//prepare response
    		request = user.prepareResponse(request);
    		session = user.updateSession(session);
    		RequestDispatcher rd =  request.getRequestDispatcher(address);
    		request.setAttribute("Error",error);
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
