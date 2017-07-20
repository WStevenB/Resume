package com.java.classes;

//this class will hold messages from the admin sent to users as alerts

public class ChatAlerts {
	public Integer Id;
	public String message;
	
	public ChatAlerts(Integer i, String m) {
		Id = i;
		message = m;
	}
}
