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


@WebServlet("/UpdateAccountController")
public class UpdateAccountController extends ControllerTemplate {
	private static final long serialVersionUID = 1L;
       
	
    public UpdateAccountController() {
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
			if(user.numberOfRequests > user.maxRequests || !user.signedInFully.equals("1")) {
				String address = "index.jsp";
				RequestDispatcher rd = request.getRequestDispatcher(address);
				request.setAttribute("Error","You've performed too much activity on the website for now!");
				rd.forward(request, response);
				return;
			}
			//default navigation
    		String address = "/WEB-INF/jsp/updateAccount.jsp";
    		String error = "";
    		Integer actionCode = Integer.valueOf(request.getParameter("actionCode"));
    		
    		switch(actionCode) {
    		//user submitted email and username to update account
    		case 1:
    			String newEmail = request.getParameter("newEmail");
    			String newUsername = request.getParameter("newUsername");
    			if(!checkEmail(newEmail)) error = "You did not enter a valid email address!";
    			else {
    				//update database
    				String resp = updateUserInfo(newEmail, newUsername, user);
    				//update fail
    				if(resp.contains("Fail")) error = "An error has occurred. Please try again later...";
    				//update success
    				else {
    					user.email = newEmail;
    					user.username = newUsername;
    					error = "Your information was successfully updated!";
    				}
    			}	
    			request.setAttribute("Error1",error);
    			break;
    		//user sent passwords to be updated
    		case 2:
    			//password criteria fails
    			if(!user.password.equals(user.confirmPassword)) {
    				error = "Your passwords did not match!";	
    			}
    			else if(user.password.length() < 8) {
    				error = "Your password must be at least eight characters long!";
    			}
    			//passwords approved
    			else {
    				//update database
    				String resp = updateUser(user);
    				//update fail
    				if(resp.contains("Fail")) {
    					error = "Something went wrong. Please try again later!";
    				}
    				//update success
    				else {
    					error = "Your password was successfully updated!";
    				}
    			}
    			request.setAttribute("Error2",error);
    			break;
    		//user navigated back to homepage
    		case 3:
    			address = "/WEB-INF/jsp/homepage.jsp";
    			break;
    		}	
    		//prepare response
    		request = user.prepareResponse(request);
    		session = user.updateSession(session);
    		RequestDispatcher rd = request.getRequestDispatcher(address);
    		rd.forward(request, response);
    	}
    	//unexpected error occurred
    	catch(Exception er) {
			String address = "/WEB-INF/jsp/updateAccount.jsp";
			RequestDispatcher rd = request.getRequestDispatcher(address);
			request.setAttribute("Error1","Your session may have expired or something unexpected happened! Please try again...");
			rd.forward(request, response);
		}
		
	}

}
