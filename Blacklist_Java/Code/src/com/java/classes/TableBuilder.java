package com.java.classes;

import java.util.ArrayList;
import java.sql.ResultSet;

//builds html tables that can be non-clickable, clickable, draggable, or droppable
//methods will all accept a result set from database and list of header names
//methods will all perform a nested loop operation: one loop through data set records, inner loop through header names
//it is assumed header names will correspond to fields in data set
//clickable tables also need the controller to request, action code to send, and hidden values
//properties contain html tags inserted around data set values
public class TableBuilder {

	private final String headerStart = "<div class='tWrapper'><table class='myTable' id=\"IDHere!\" border=\"1\"><thead><tr>";
	private final String headerMember = "<th>MemberHere!</th>";
	private final String headerEnd = "</tr></thead><tbody>";
	
	private final String rowStart = "<tr><form action=\"ControllerHere!\"  method=\"POST\">";
	private final String rowStartNonClick = "<tr>";
	private final String dragRowStart = "<tr id=\"IDHere!\" draggable=\"true\" ondragstart=\"drag(event)\">";
	private final String dropRowStart = "<tr id=\"IDHere!\" ondrop=\"drop(event)\" ondragover=\"allowDrop(event)\">";
	
	private final String rowSubmit = "<td><input style=\"width: 100%;\" type=\"submit\" value=\"SubmitHere!\"/></td>";
	private final String rowMember = "<td>ValueHere!</td>";
	private final String dragMember = "<td>ValueHere!</td>";
	private final String hidden = "<input type=\"hidden\" name=\"NameHere!\" value=\"ValueHere!\" />";
	private final String rowNum = "<input type=\"hidden\" name=\"NameHere!\" value=ValueHere! />";
	private final String rowEnd = "</form></tr>";
	private final String rowEndNonClick = "</tr>";
	
	private final String tableEnd = "</tbody></table></div>";
	
	public String buildTableClickable(String controller, ArrayList<String> headers, ArrayList<String> hiddens, String actionCode, ResultSet data) {
		try {
			String output = headerStart.replace("IDHere!", "table1");
			for(int i = 0; i<headers.size(); i++) output += headerMember.replace("MemberHere!", headers.get(i));
			output += headerEnd;
			int i = 0;
			while(data.next()) {
				i++;
				output += rowStart.replace("ControllerHere!", controller);
				output += rowSubmit.replace("SubmitHere!", data.getString(1));
				output += hidden.replace("NameHere!", hiddens.get(0)).replaceAll("ValueHere!", data.getString(1));
				for(int ii = 1; ii<headers.size(); ii++) {
					output += rowMember.replace("ValueHere!", data.getString(ii+1));
					output += hidden.replace("NameHere!", hiddens.get(ii)).replaceAll("ValueHere!", data.getString(ii+1));
				}
				output += rowNum.replace("NameHere!", "actionCode").replace("ValueHere!", actionCode);
				output += rowNum.replace("NameHere!", "rowNum").replace("ValueHere!", String.valueOf(i));
				output += rowEnd;
			}
			output += tableEnd;
			return output;
		}
		catch(Exception er) {
			return "";
		}
		
	}
	
	public String buildTableNonClickable(ArrayList<String> headers, ResultSet data) {
		try {
			String output = headerStart.replace("IDHere!", "table1");
			for(int i = 0; i<headers.size(); i++) output += headerMember.replace("MemberHere!", headers.get(i));
			output += headerEnd;
			int i = 0;
			while(data.next()) {
				i++;
				output += rowStartNonClick;
				for(int ii = 0; ii<headers.size(); ii++) {
					output += rowMember.replace("ValueHere!", data.getString(ii+1));
				}
				output += rowEndNonClick;
			}
			output += tableEnd;
			return output;
		}
		catch(Exception er) {
			System.out.println(er.getMessage());
			return "";
		}
	}
	
	public String buildTableDraggable(ArrayList<String> headers, ResultSet data, String ID) {
		try {
			String output = headerStart.replace("IDHere!", ID);
			for(int i = 0; i<headers.size(); i++) output += headerMember.replace("MemberHere!", headers.get(i));
			output += headerEnd;
			int i = 0;
			while(data.next()) {
				i++;
				output += dragRowStart.replace("IDHere!", ID + String.valueOf(i));
				for(int ii = 0; ii<headers.size(); ii++) {
					if(ii == 0) output += hidden.replace("ValueHere!", data.getString(ii+1)).replace("NameHere!", ID + "costComingUp" + String.valueOf(i));
					if(ii == headers.size()-1) output += hidden.replace("ValueHere!", data.getString(ii+1)).replace("NameHere!", ID + "valueComingUp" + String.valueOf(i));
					else output += dragMember.replace("ValueHere!", data.getString(ii+1));
				}
				output += rowEndNonClick;			
			}
			output += tableEnd;
			return output;
		}
		catch(Exception er) {
			return "";
		}
	}
	
	public String buildTableDroppable(ArrayList<String> headers, ResultSet data, String ID) {
		try {
			String output = headerStart.replace("IDHere!", ID);
			for(int i = 0; i<headers.size(); i++) output += headerMember.replace("MemberHere!", headers.get(i));
			output += headerEnd;
			int i = 0;
			while(data.next()) {
				i++;
				output += dropRowStart.replace("IDHere!", ID + String.valueOf(i));
				for(int ii = 0; ii<headers.size(); ii++) {
					if(ii == 0) output += hidden.replace("ValueHere!", data.getString(ii+1)).replace("NameHere!", ID + "costComingUp" + String.valueOf(i));
					if(ii == headers.size()-1) output += hidden.replace("ValueHere!", data.getString(ii+1)).replace("NameHere!", ID + "valueComingUp" + String.valueOf(i));
					else output += dragMember.replace("ValueHere!", data.getString(ii+1));
				}
				output += rowEndNonClick;			
			}
			output += tableEnd;
			return output;
		}
		catch(Exception er) {
			return "";
		}
	}
	
	
}
