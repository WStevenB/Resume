package com.helloweb.controller;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import javax.servlet.RequestDispatcher;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;
import org.springframework.util.FileCopyUtils;
import com.java.classes.ChatAlerts;
import com.java.classes.ControllerTemplate;
import com.java.classes.User;


@WebServlet("/ChatAlertsController")
public class ChatAlertsController extends ControllerTemplate {
	private static final long serialVersionUID = 1L;
	
	//holds alerts from admin to users
    private static ArrayList<ChatAlerts> alertList = new ArrayList<>();
	
    public ChatAlertsController() {
        super();
    }
    
    @Override
	protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
    	try {
    		//new alert is being added by admin
    		//key is checked then new size of alert list is returned
    		if(request.getParameterMap().containsKey("key")) {
    			String key = request.getParameter("key").toString();
        		if(key.equals("iufh8h938h4hahsdfg62876t736y2guhygdygvfcfraddadedw54")) {
        			Integer id = Integer.valueOf(request.getParameter("arg1"));
        			String message = request.getParameter("arg2").toString();
        			alertList.add(new ChatAlerts(id, message));
        			response.setContentType("text/plain");      
            		response.setCharacterEncoding("UTF-8");
            		String f = String.valueOf(alertList.size());
            		InputStream is = new ByteArrayInputStream(f.getBytes(StandardCharsets.UTF_8));
            		FileCopyUtils.copy(is, response.getOutputStream());
            	    response.flushBuffer();
        		}
    		}
    		//alerts are being requested by user
    		//list is linearly probed, if ids match then message is returned
    		//if no ids match, none is returned
    		else {   		
    	    	HttpSession session = request.getSession(false);
    	    	User user = new User(request, session);
    	    	int id = Integer.valueOf(user.ID);
    	    	int foundOne = -1;
    	    	for(int i = 0; i<alertList.size(); i++) {
    	    		if(alertList.get(i).Id == id) {
    	    			response.setContentType("text/plain");      
    	    	    	response.setCharacterEncoding("UTF-8");
    	    	    	String f = alertList.get(i).message;
    	    	    	InputStream is = new ByteArrayInputStream(f.getBytes(StandardCharsets.UTF_8));
    	    	    	FileCopyUtils.copy(is, response.getOutputStream());
    	    	    	response.flushBuffer();
    	    	    	foundOne = i;
    	    	    	break;
    	    		}
    	    	}
    	    	//none found for this id
    	    	if(foundOne == -1) {
    	    		response.setContentType("text/plain");      
    	        	response.setCharacterEncoding("UTF-8");
    	        	String f = "None";
    	        	InputStream is = new ByteArrayInputStream(f.getBytes(StandardCharsets.UTF_8));
    	        	FileCopyUtils.copy(is, response.getOutputStream());
    	        	response.flushBuffer();
    	    	}
    	    	//remove alert from list
    	    	else alertList.remove(foundOne);
    		}
    	}
    	//unexpected error occurred
    	catch(Exception er) {
    		response.setContentType("text/plain");      
    		response.setCharacterEncoding("UTF-8");
    		String f = "Fail";
    		InputStream is = new ByteArrayInputStream(f.getBytes(StandardCharsets.UTF_8));
    		FileCopyUtils.copy(is, response.getOutputStream());
    	    response.flushBuffer();
		}
		
	}


    @Override
	protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
    	
		
	}

}
