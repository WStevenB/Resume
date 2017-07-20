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

@WebServlet("/ConfirmForgotController")
public class ConfirmForgotController extends ControllerTemplate {
	private static final long serialVersionUID = 1L;

    public ConfirmForgotController() {
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
			//default navigation is change password page
    		String address = "/WEB-INF/jsp/changeCredentials.jsp";
    		String error = "";
    		Integer actionCode = Integer.valueOf(request.getParameter("actionCode"));
    		
    		switch(actionCode) {
    		//user sent code to check
    		case 1:
    			String resp = confirmForgotCode(user);
    			//fail - redirect navigation and send error message
    			if(!resp.equals("Success")) {
    			    address = "/WEB-INF/jsp/forgotCode.jsp";
    				error = "That was not the code we sent you!";
    			}
    			break;
    		//user navigated back to landing page
    		case 2:
    			address = "index.jsp";
    			error = "";
    			break;
    		default:
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
