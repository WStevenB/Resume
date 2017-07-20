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



@WebServlet("/NewAccountController")
public class NewAccountController extends ControllerTemplate {
	private static final long serialVersionUID = 1L;
       
	WebLookup webHelper;
	
    public NewAccountController() {
        super();
    }

    @Override
	protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
    	try {
    		//create user object
    		HttpSession session = request.getSession(false);
    		User user = new User(request, session);
    		//check for too much activity
    		user.numberOfRequests++;
			if(user.numberOfRequests > user.maxRequests) {
				user.maxRequests = 1000;
				String address = "index.jsp";
				RequestDispatcher rd = request.getRequestDispatcher(address);
				request.setAttribute("Error","You've performed too much activity on the website for now!");
				rd.forward(request, response);
				return;
			}
			//default navigation
    		String address = "/WEB-INF/jsp/newAccount.jsp";
    		String error = "";
    		String action = "Cancel";
    		//button will initially be to create account
    		String button = "<button class=\"_button _button-1\" type=\"submit\" name=\"actionCode\" value=\"1\">Create Account</button>";
    		Integer actionCode = Integer.valueOf(request.getParameter("actionCode"));
    		
    		switch(actionCode) {
    		//user entered info to create account
    		case 1:
    			//creation fails
    			if(!checkEmail(user.email)) error = "You did not enter a valid email address!";	
    			else if(!user.password.equals(user.confirmPassword)) error = "Your passwords did not match!";	
    			else if(user.password.length() < 8) error = "Your password must be at least eight characters long!";
    			//creation approved
    			else{
    				user.accountsCreated++;
					if(user.accountsCreated > 1) {
						address = "index.jsp";
						RequestDispatcher rd = request.getRequestDispatcher(address);
						request.setAttribute("Error","You've performed too much activity on the website for now!");
						rd.forward(request, response);
						return;
					}
					//insert into database
    				String resp = insertAccount(user);
    				//insert success
    				if(!resp.contains("Fail") && !resp.equals("NetworkError!")) {
    					error = "Account successfully created!";
    					action = "Now Sign In!";
    					//create button erased
    					button = "";
    				}
    				//insert fail
    				else error = resp;
    			}
    			break;
    		//user navigated back to landing page
    		case 2:
    			address = "index.jsp";
    			break;
    		}	
    		//prepare response
    		request = user.prepareResponse(request);
    		session = user.updateSession(session);
    		RequestDispatcher rd =  request.getRequestDispatcher(address);
    		request.setAttribute("Error",error);
    		request.setAttribute("Action",action);
    		request.setAttribute("AccountButton",button);
    		rd.forward(request, response);
    	}
    	//unexpected error
    	catch(Exception er) {
			String address = "index.jsp";
			RequestDispatcher rd = request.getRequestDispatcher(address);
			request.setAttribute("Error","Your session may have expired or something unexpected happened! Please try again...");
			rd.forward(request, response);
		}
    	
	}

}
