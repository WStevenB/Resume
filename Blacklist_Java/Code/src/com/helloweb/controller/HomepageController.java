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

@WebServlet("/HomepageController")
public class HomepageController extends ControllerTemplate {
	private static final long serialVersionUID = 1L;
  
    public HomepageController() {
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
    		String address = "/WEB-INF/jsp/fileUpload.jsp";
    		Integer actionCode = Integer.valueOf(request.getParameter("actionCode"));
    		switch(actionCode) {
    		//user navigated to file upload page
    		case 1:
    			address = "/WEB-INF/jsp/fileUpload.jsp";
    			break;
    		//user navigated to chatroom page
    		case 2:
    			String targetId = user.ID;
        		if(targetId.equals("1")) targetId = "2";
    			user.messages = getMessages(targetId);
    			address = "/WEB-INF/jsp/chatroom.jsp";
    			break;
    		//user navigated to update account page
    		case 3:
    			address = "/WEB-INF/jsp/updateAccount.jsp";
    			break;
    		//user signed out, back to landing page
    		case 4:
    			session = user.clearSession(session);
    			address = "index.jsp";
    			break;
    		}	
    		//prepare response
    		request = user.prepareResponse(request);
    		if(actionCode != 4) session = user.updateSession(session);
    		RequestDispatcher rd =  request.getRequestDispatcher(address);
    		request.setAttribute("Error","");
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
