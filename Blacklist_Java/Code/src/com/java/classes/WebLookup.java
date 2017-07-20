package com.java.classes;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLEncoder;
import java.util.ArrayList;


//streamlines making http requests and getting response
//target is this server
//used to pass info from one controller to another
//query string will be in form ?arg1=something&arg2=something&arg3=...&key=
//arg1 is first entry in parameters list and so on
public class WebLookup {

	private static String baseAddress;
	private static String key;
	public ArrayList<String> parameters;
	
	public WebLookup() {
		baseAddress = "http://127.0.0.1:8080/Blacklist/";
		key = "iufh8h938h4hahsdfg62876t736y2guhygdygvfcfraddadedw54";
		parameters = new ArrayList<String>();
	}
	
	//hashes a password parameter before adding it to list
	public void addPasswordParameter(String password) {
		try {
			PasswordHasher ph = new PasswordHasher();
			String p = ph.hash(password.toCharArray());
			parameters.add(p);
		}
		catch(Exception err) {
			
		}
	}
	
	//builds entire query string and returns http response in form of string
	public  String getWebData(String fileName) {
		try {
			String urlAddress = baseAddress + fileName + insertParameters() + "&key="+key;
			HttpURLConnection connection = (HttpURLConnection) new URL(urlAddress).openConnection();
	        connection.connect();
	        InputStream is = connection.getInputStream();
	        
	        return convertStreamToString(is);
		}
		catch(Exception er) {
			try {
				String urlAddress = baseAddress + fileName + insertParameters() + "&key="+key;
				System.out.println(urlAddress);
			}
			catch(Exception err) {}
		}
        return "NetworkError!";
	}
	
	//loops through parameters list, converts each to url encoding and builds argument part of query string
	public String insertParameters() {
		String output = "?";
		try {
			for(int i = 0; i<parameters.size(); i++) {
				if(i != 0) output += "&";
				output += "arg" + String.valueOf(i+1) + "=" + URLEncoder.encode(parameters.get(i), "UTF-8");
			}
		}
		catch(Exception er) {}
		parameters = new ArrayList<String>(); 
		return output;
	}
	
	
	//converts http response stream to string
	private static String convertStreamToString(InputStream is) {
        BufferedReader reader = new BufferedReader(new InputStreamReader(is));
        StringBuilder sb = new StringBuilder();
        String line = null;
        try {
            while ((line = reader.readLine()) != null) {
                sb.append(line + "\n");
            }
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                is.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        return sb.toString().trim();
    }
}
