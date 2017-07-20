package com.helloweb.controller;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.nio.charset.StandardCharsets;
import java.nio.file.FileSystems;
import java.nio.file.Files;
import java.nio.file.Path;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Scanner;

import javax.servlet.RequestDispatcher;
import javax.servlet.ServletException;
import javax.servlet.annotation.MultipartConfig;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;
import javax.servlet.http.Part;

import org.apache.tomcat.util.http.fileupload.IOUtils;
import org.springframework.util.FileCopyUtils;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

import com.java.classes.ControllerTemplate;
import com.java.classes.User;

@WebServlet("/FileUploadController")
@MultipartConfig
public class FileUploadController extends ControllerTemplate {
	private static final long serialVersionUID = 1L;
  
    public FileUploadController() {
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
			//default navigation is file upload page
    		String address = "/WEB-INF/jsp/fileUpload.jsp";
    		Integer actionCode = Integer.valueOf(request.getParameter("actionCode"));
    		String error = "";
    		switch(actionCode) {
    		//user uploaded a file
    		case 1:
    			try {
    				//get file parameter from request to stream
        		    Part filePart = request.getPart("file"); 
        		    InputStream fileContent = filePart.getInputStream();
        		    
        		    //read file into a string sent to input parser
        		    Scanner scanner = new Scanner(fileContent);
        		    scanner.useDelimiter("\\A");
        		    inputParser.input = scanner.hasNext() ? scanner.next() : "";
        		    scanner.close();
        		    fileContent.close();
        		    
        		    //save file to bin/uploads folder
        		    DateFormat dateFormat = new SimpleDateFormat("yyyyMMddHHmmss");
            		Date date = new Date();
            		String dateString = dateFormat.format(date);
            		String outName = "uploads/id_" + user.ID + "_date_" + dateString + ".txt";
            		PrintWriter out = new PrintWriter(outName);
            		out.print(inputParser.input);
            		out.close();
        		    error = "Upload success!";
        		    
        		    //find matches in database
        		    getMatches(user);
        		    if(user.matchFileContents.equals("Fail")) user.matchCount = "Failed to parse input";
        		    //send user download button and html table with matches
        		    else {
        		    	user.matchCount = user.matchCount + " matches found";
        		    	session.setAttribute("matchFileContents", user.matchFileContents);
        		    	request.setAttribute("DownloadButton","<form action=\"FileUploadController\" method=\"post\"><button class=\"_button _button-1\" type=\"submit\" name=\"actionCode\" value=\"3\">Download Matches</button></form>");
        		    }
    			}
    			//file upload fail
    			catch(Exception er) {
    				System.out.println(er.getMessage());
    				user.matchCount = "Upload failed...";
    			}    		    
    			break;
    		//user navigated back to homepage
    		case 2:
    			address = "/WEB-INF/jsp/homepage.jsp";
    			break;
    		//user asked to download file
    		case 3:
    			if(session.getAttribute("matchFileContents") != null) user.matchFileContents = session.getAttribute("matchFileContents").toString();
    			downloadFile(response, user);
    			break;
    		}	
    		//prepare response
    		request = user.prepareResponse(request);
    		session = user.updateSession(session);
    		RequestDispatcher rd =  request.getRequestDispatcher(address);
    		request.setAttribute("Error", error);
    		request.setAttribute("Matches", user.matchCount);
    		request.setAttribute("MatchTable", user.matchTableContents);
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
    
    //put matches file in response stream
    public void downloadFile(HttpServletResponse response, User user) {
    	try {
    		response.setContentType("application/octet-stream");      
    		response.setHeader("Content-Disposition", "attachment; filename=\"results.txt\"");
    		InputStream is = new ByteArrayInputStream(user.matchFileContents.getBytes(StandardCharsets.UTF_8));
    		FileCopyUtils.copy(is, response.getOutputStream());
    	    response.flushBuffer();
    	} 
    	catch (IOException er) {
    		System.out.println(er.getMessage());
    	}

    }

}
