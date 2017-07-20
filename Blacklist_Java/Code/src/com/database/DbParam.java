package com.database;

//this class stores a value and type to be sent to DatabaseHelper as a query parameter
//four types currently available: string, integer, double, float
//date is to be sent as string
public class DbParam {
	
	public Object value;
	public String type;
	
	public DbParam(String v, String t) {
		value = v;
		type = t;
	}
	
	public DbParam(Integer v, String t) {
		value = v;
		type = t;
	}
	
	public DbParam(Double v, String t) {
		value = v;
		type = t;
	}
	
	public DbParam(Float v, String t) {
		value = v;
		type = t;
	}

}
