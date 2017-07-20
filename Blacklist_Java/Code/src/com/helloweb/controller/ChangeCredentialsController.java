package com.helloweb.controller;

import java.io.IOException;

import javax.servlet.RequestDispatcher;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;
import com.java.classes.ControllerTemplate;
import com.java.classes.User;


@WebServlet("/ChangeCredentialsController")
public class ChangeCredentialsController extends ControllerTemplate {
	private static final long serialVersionUID = 1L;
       
	
    public ChangeCredentialsController() {
        super();
    }


    @Override
	protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
    	try {
    		HttpSession session = request.getSession(false);
    		User user = new User(request, session);
    		
    		//check if too much activity has been performed
    		user.numberOfRequests++;
			if(user.numberOfRequests > user.maxRequests) {
				String address = "index.jsp";
				RequestDispatcher rd = request.getRequestDispatcher(address);
				request.setAttribute("Error","You've performed too much activity on the website for now!");
				rd.forward(request, response);
				return;
			}
			//default navigation is homepage - change of info should be success
			//will navigate elsewhere otherwise
    		String address = "/WEB-INF/jsp/homepage.jsp";
    		String error = "";
    		Integer actionCode = Integer.valueOf(request.getParameter("actionCode"));
    		
    		switch(actionCode) {
    		//user tries to change password
    		case 1:
    			//fail - do not match
    			if(!user.password.equals(user.confirmPassword)) {
    				address = "/WEB-INF/jsp/changeCredentials.jsp";
    				error = "Your passwords did not match!";	
    			}
    			//fail - not long enough
    			else if(user.password.length() < 8) {
    				address = "/WEB-INF/jsp/changeCredentials.jsp";
    				error = "Your password must be at least eight characters long!";
    			}
    			//try to update database
    			else {
    				String resp = updateUser(user);
    				//fail - database error occurred
    				if(resp.contains("Fail")) {
    					address = "/WEB-INF/jsp/changeCredentials.jsp";
    					error = "Something went wrong. Please try again later!";
    				}
    				//success
    				else {
    					user.signedInFully = "1";
    					getCredentials(user);
    				}
    			}
    			break;
    		//user goes back to landing page
    		case 2:
    			address = "index.jsp";
    			break;
    		}
    		//prepare response and send
    		request = user.prepareResponse(request);
    		session = user.updateSession(session);
    		RequestDispatcher rd = request.getRequestDispatcher(address);
    		request.setAttribute("Error",error);
    		rd.forward(request, response);
    	}
    	//unexpected error happened
    	catch(Exception er) {
			String address = "index.jsp";
			RequestDispatcher rd = request.getRequestDispatcher(address);
			request.setAttribute("Error","Your session may have expired or something unexpected happened! Please try again...");
			rd.forward(request, response);
		}
		
	}

}
