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


@WebServlet("/ChatController")
public class ChatController extends ControllerTemplate {
	private static final long serialVersionUID = 1L;
	
	
       
	
    public ChatController() {
        super();
    }
    
    @Override
	protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
    	try {
    		//prepares user object
    		HttpSession session = request.getSession(false);
    		User user = new User(request, session);
    		String address = "/WEB-INF/jsp/chat.jsp";
    		
    		//goal is to return all messages between admin and particular user
    		//if the user is not the admin, query will target user's own id
    		String targetId = user.ID;
    		//if this is the admin, query will target partner's id
    		if(targetId.equals("1")) targetId = user.chatPartner;
    		
    		//id to check for new messages
    		String flagId = targetId;
    		if(user.ID.equals("1")) flagId = "A" + flagId;
    		
    		//sleeps until 7 seconds are up or new message found
    		String flag = "0";
    		try {flag = getServletContext().getAttribute(flagId).toString();} catch(Exception err){}
    		int count = 0;
    		while(!flag.equals("1") && count < 70) {
    			Thread.sleep(100);
    			try {flag = getServletContext().getAttribute(flagId).toString();} catch(Exception err){}
    			count++;
    		}
    		//remove flag
    		getServletContext().setAttribute(flagId, "0");
    		
    		//gets messages in form of html table
    		user.messages = getMessages(targetId);

    		//prepares response
    		request = user.prepareResponse(request);
    		session = user.updateSession(session);
    		RequestDispatcher rd = request.getRequestDispatcher(address);
    		rd.forward(request, response);
    	}
    	//unexpected error occurred
    	catch(Exception er) {
			String address = "/WEB-INF/jsp/chat.jsp";
			RequestDispatcher rd = request.getRequestDispatcher(address);
			request.setAttribute("messages","Your session may have expired or something unexpected happened! Please try again...");
			rd.forward(request, response);
		}
		
	}


    @Override
	protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
    	try {
    		//prepares user object
    		HttpSession session = request.getSession(false);
    		User user = new User(request, session);
    		String address = "/WEB-INF/jsp/chat.jsp";
    		
    		//goal is to return all messages between admin and particular user
    		//if the user is not the admin, query will target user's own id
    		String targetId = user.ID;
    		
    		//if this is the admin, query will target partner's id
    		if(targetId.equals("1")) targetId = user.chatPartner;
    		
    		//id to check for new messages
    		String flagId = targetId;
    		if(user.ID.equals("1")) flagId = "A" + flagId;
    		
    		//sleeps until 7 seconds are up or new message found
    		String flag = "0";
    		try {flag = getServletContext().getAttribute(flagId).toString();} catch(Exception err){}
    		int count = 0;
    		while(!flag.equals("1") && count < 70) {
    			Thread.sleep(100);
    			try {flag = getServletContext().getAttribute(flagId).toString();} catch(Exception err){}
    			count++;
    		}
    		//remove flag
    		getServletContext().setAttribute(flagId, "0");
    		
    		//gets messages in form of html table
    		user.messages = getMessages(targetId);
    		
    		//prepares response
    		request = user.prepareResponse(request);
    		session = user.updateSession(session);
    		RequestDispatcher rd = request.getRequestDispatcher(address);
    		rd.forward(request, response);
    	}
    	//unexpected error occurred
    	catch(Exception er) {
			String address = "/WEB-INF/jsp/chat.jsp";
			RequestDispatcher rd = request.getRequestDispatcher(address);
			request.setAttribute("messages","Your session may have expired or something unexpected happened! Please try again...");
			rd.forward(request, response);
		}
		
	}

}
