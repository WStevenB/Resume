package com.database;

//this class parses the input from an uploaded respondent file
//file is assumed to be tab-delimited: FirstName \t LastName \t Email \t Phone \t Zip
//each record is hashed in four ways for query efficiency
public class InputParser {
	
	//raw string value of file
	public String input = "";
	
	//removes - ( etc from phone number
	private String filterPhone(String phone) {
		String output = "";
		for(int i = 0; i<phone.length(); i++) {
			char c = phone.charAt(i);
			if(c >= 48 && c <= 57) output += c;
		}
		return output;
	}
	
	//builds query string with hashes to represent each record in file
	public String buildQueryForCount() {
		try {
			input = input.trim();
			input = input.replace("\\\r", "");
			String emailHashes = "";
			String phoneHashes = "";
			String zipHashes = "";
			String otherHashes = "";
			String[] lines = input.split("\\\n");
			for(int i = 0; i<lines.length-1; i++) {
				try {
					String[] items = lines[i].split("\\\t");
					String fifth = "";
					String phone = filterPhone(items[3]);
					if(items.length == 5) fifth = items[4];
					emailHashes += getHash(items[0], items[1], items[2]) + ", ";
					phoneHashes += getHash(items[0], items[1], phone) + ", ";
					zipHashes += getHash(items[0], items[1], fifth) + ", ";
					otherHashes += getHash(items[2], phone, fifth) + ", ";
				}
				catch (Exception err) {}
			}
			try {
				int i = lines.length-1;
				String[] items = lines[i].split("\\\t");
				String fifth = "";
				if(items.length == 5) fifth = items[4];
				emailHashes += getHash(items[0], items[1], items[2]);
				phoneHashes += getHash(items[0], items[1], items[3]);
				zipHashes += getHash(items[0], items[1], fifth);
				otherHashes += getHash(items[2], items[3], fifth);
			}
			catch(Exception errr) {}
			 
			return  "SELECT FirstName, LastName, Email, Address, City, State, Zip, Phone, DateAdded, Comments " + 
					"FROM Blacklist WHERE FirstName || LastName || Email IN (" + emailHashes + ") " +
					"OR FirstName || LastName || Phone IN (" + phoneHashes + ") " + 
					"OR FirstName || LastName || Zip IN (" + zipHashes + ") " + 
					"OR Email || Phone || Zip IN (" + otherHashes + ");";
		}
		catch(Exception er) {
			System.out.println(er.getMessage());
			return "Error";
		}
		
	}
	
	//returns a hash value when provided with three fields
	private String getHash(String one, String two, String three) {
		if(one.length() == 0 || two.length() == 0 || three.length() == 0) return "-9";
		return "'" + one + two + three + "'";
	}
	
}
