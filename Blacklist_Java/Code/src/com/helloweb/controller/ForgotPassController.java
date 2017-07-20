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
import com.java.classes.EmailSender;
import com.java.classes.User;
import com.java.classes.WebLookup;


@WebServlet("/ForgotPassController")
public class ForgotPassController extends ControllerTemplate {
	private static final long serialVersionUID = 1L;

    public ForgotPassController() {
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
    		String error = "";
    		//default navigation
    		String address = "/WEB-INF/jsp/forgotPassword.jsp";
    		Integer actionCode = Integer.valueOf(request.getParameter("actionCode"));
    		switch(actionCode) {
    		//user requested a confirmation code to be sent
    		case 1:
    			//prevent too many confirmation requests
    			user.forgotRequests++;
				if(user.forgotRequests > 2) {
					user.maxRequests = 1000;
					address = "index.jsp";
					RequestDispatcher rd = request.getRequestDispatcher(address);
					request.setAttribute("Error","You've performed too much activity on the website for now!");
					rd.forward(request, response);
					return;
				}
				//reset confirmation code
    			String text = createCode(user);
    			if(text.contains("Fail")) {
    				error = "Something went wrong while processing your request. Please try again later.";
    			}
    			//send code to user's email
    			else {
    				emailSender.sendEmail(user.email, text);
    				address = "/WEB-INF/jsp/forgotCode.jsp";
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
