package com.helloweb.controller;

import java.io.IOException;
import java.util.Random;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;
import com.java.classes.ControllerTemplate;
import com.java.classes.User;


import javax.servlet.RequestDispatcher;


@WebServlet("/LoginController")
public class LoginController extends ControllerTemplate {
	private static final long serialVersionUID = 1L;



	public LoginController() {
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
				String address = "index.jsp";
				RequestDispatcher rd = request.getRequestDispatcher(address);
				request.setAttribute("Error","You've performed too much activity on the website for now!");
				rd.forward(request, response);
				return;
			}
			//clear session in case user had signed in previously and navigated backwards without signing out
			session = user.clearSession(session);
			Random rand = new Random();
			int  n = rand.nextInt(1000000);
			user.sessionID = String.valueOf(n);
			
			//default navigation
			String address = "/WEB-INF/jsp/homepage.jsp";
			String error = "";
			Integer actionCode = Integer.valueOf(request.getParameter("actionCode"));
			
			switch(actionCode) {
			//user attempted to sign in
			case 1:
				String status = getCredentials(user);
				//database error
				if(status.equals("Fail")) {
					address = "/index.jsp";
					error = "Something went wrong! Please try again later...";
				}
				//incorrect password
				else if(status.equals("NotPassword")) {
					user.passwordsGuessed++;
					if(user.passwordsGuessed > 3) {
						user.maxRequests = 1000;
						address = "index.jsp";
						RequestDispatcher rd = request.getRequestDispatcher(address);
						request.setAttribute("Error","You've performed too much activity on the website for now!");
						rd.forward(request, response);
						return;
					}
					address = "/index.jsp";
					error = "Did not match our records...";
				}
				//email not found
				else if(status.equals("NoEmail")) {
					user.passwordsGuessed++;
					if(user.passwordsGuessed > 3) {
						user.maxRequests = 1000;
						address = "index.jsp";
						RequestDispatcher rd = request.getRequestDispatcher(address);
						request.setAttribute("Error","You've performed too much activity on the website for now!");
						rd.forward(request, response);
						return;
					}
					address = "/index.jsp";
					error = "Did not match our records...";
				}
				//email and password are correct, account not confirmed though
				else if(status.equals("Confirm")) {
					String cCode = changeConfirmCode(user);
					emailSender.sendEmail(user.email, "noReply@BabcockTechnologies.com", cCode, "Confirmation Code");
					address = "/WEB-INF/jsp/confirmCode.jsp";
					request.setAttribute("ConfirmButton", "<button class=\"_button _button-2\" type=\"submit\" name=\"actionCode\" value=\"1\">Submit</button>");
				}
				//sign in success
				else if(status.equals("Success")) {
					user.signedInFully = "1";
					String targetId = user.ID;
					//will place extra functions in chatroom for admin
	        		if(targetId.equals("1")) {
	        			user.chatPartner = "2";
	        			targetId = "2";
	        			user.partners = getChatPartners();
	        			user.alertCheckbox = "<input type=\"checkbox\" name=\"alertCheck\" value\"IsAlert\">Send as alert";
	        		}
	        		//email sent to admin for user log-in
	        		else {
	        			String info = user.username + " has just logged in\r\n\r\nEmail: " + user.email;
	        			emailSender.sendEmail("walterstevenbabcock@gmail.com", "noReply@BabcockTechnologies.com", info, "User Login");
	        		}		
					address = "/WEB-INF/jsp/homepage.jsp";
				}
				break;
			//user navigated to create account page
			case 2:
				address = "/WEB-INF/jsp/newAccount.jsp";
				request.setAttribute("AccountButton", "<button class=\"_button _button-1\" type=\"submit\" name=\"actionCode\" value=\"1\">Create Account</button>");
				break;
			//user navigated to forgot password page
			case 3:
				address = "/WEB-INF/jsp/forgotPassword.jsp";
				break;
			}	
			//prepare response
			request = user.prepareResponse(request);
			session = user.updateSession(session);
			RequestDispatcher rd = request.getRequestDispatcher(address);
			request.setAttribute("Error",error);
			request.setAttribute("Action","Cancel");
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
