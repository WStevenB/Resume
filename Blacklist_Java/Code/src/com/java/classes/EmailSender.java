package com.java.classes;



import java.util.*;
import javax.mail.*;
import javax.mail.internet.*;

//assists in sending user emails about confirmation codes and website admin about logins/messages
//employs SendGrid api and mail.jar
//methods run on background thread because they take a few seconds
public class EmailSender {



	public EmailSender() {
	}
	
	public void sendEmail(String to, String from, String content, String subject) {
            
		try {
			Runnable run = new Runnable() {
	            @Override
	            public void run() {
	            	try {
	            		Properties properties = new Properties();
		                properties.put("mail.transport.protocol", "smtp");
		                properties.put("mail.smtp.host", "smtp.sendgrid.net");
		                properties.put("mail.smtp.port", 587);
		                properties.put("mail.smtp.auth", "true");   
		                Authenticator auth = new SMTPAuthenticator();
		                Session mailSession = Session.getDefaultInstance(properties, auth);
		                MimeMessage message = new MimeMessage(mailSession);
		                Multipart multipart = new MimeMultipart("alternative");
		                BodyPart part1 = new MimeBodyPart();
		                part1.setText(content);
		                multipart.addBodyPart(part1);
		                message.setFrom(new InternetAddress(from));
		                message.addRecipient(Message.RecipientType.TO, new InternetAddress(to));
		                message.setSubject(subject);
		                message.setContent(multipart);
		                Transport transport = mailSession.getTransport();
		                transport.connect();
		                transport.sendMessage(message, message.getAllRecipients());
		                transport.close();
	            	}
	            	catch(Exception err) {}
	            	
	            }
			};
			Thread thread = new Thread(run);
	        thread.start();
		}
		catch(Exception er) {System.out.println(er.getMessage());}	 
	}
	
	
	public void sendEmail(String to, String content) {
		try {
			Runnable run = new Runnable() {
	            @Override
	            public void run() {
	            	try {
	            		Properties properties = new Properties();
		                properties.put("mail.transport.protocol", "smtp");
		                properties.put("mail.smtp.host", "smtp.sendgrid.net");
		                properties.put("mail.smtp.port", 587);
		                properties.put("mail.smtp.auth", "true");   
		                Authenticator auth = new SMTPAuthenticator();
		                Session mailSession = Session.getDefaultInstance(properties, auth);
		                MimeMessage message = new MimeMessage(mailSession);
		                Multipart multipart = new MimeMultipart("alternative");
		                BodyPart part1 = new MimeBodyPart();
		                part1.setText(content);
		                multipart.addBodyPart(part1);
		                message.setFrom(new InternetAddress("noReply@BabcockTechnologies.com"));
		                message.addRecipient(Message.RecipientType.TO, new InternetAddress(to));
		                message.setSubject("Account Update");
		                message.setContent(multipart);
		                Transport transport = mailSession.getTransport();
		                transport.connect();
		                transport.sendMessage(message, message.getAllRecipients());
		                transport.close();
	            	}
	            	catch(Exception err) {}
	            	
	            }
			};
			Thread thread = new Thread(run);
	        thread.start();
		}
		catch(Exception er) {System.out.println(er.getMessage());}	 
	}
	
	

}

class SMTPAuthenticator extends javax.mail.Authenticator {
	public PasswordAuthentication getPasswordAuthentication() {
		String username = "scubasteve623";
		String password = "freebird623";
		return new PasswordAuthentication(username, password);
	}
}
