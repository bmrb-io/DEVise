// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2014
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// TEMP -- add description here

// ------------------------------------------------------------------------


import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.HashMap;
import java.util.Map;


public class Anonymizer {

	private String inputFilePath;
	private String outputFilePath;
	private Map<String, String> users = new HashMap<String, String>();
	private Map<String, String> orgs = new HashMap<String, String>();
	private final static String USER = "user";
	private final static String ORG = "org";
	private String firstLineStartsWith = "#"; // Used multiple times in a class. Hence declaring at class level.

	public static void main(String[] args) {
		Anonymizer anonymizer = new Anonymizer();
		if(args.length < 2){
			log("Incorrect number of arguments. Correct usage : java Anonymizer <inputFilePath> <outputFilePath>");
		}
		else {
			anonymizer.inputFilePath = args[0];
			anonymizer.outputFilePath = args[1];
			anonymizer.anonymize();
		}
		
	}

	private void anonymize() {
		try(BufferedReader reader = new BufferedReader(new FileReader(inputFilePath));
			PrintWriter writer = new PrintWriter(new BufferedWriter(new FileWriter(outputFilePath)))) {
			DateFormat dateFormat = new SimpleDateFormat("dd-MMM-yyyy HH:mm:ss");
			writer.println(firstLineStartsWith + "This conversion was run on " + dateFormat.format(Calendar.getInstance().getTime()));
			String line = null;
			int sizeOfArray = 6;
			String [] strArray = new String[sizeOfArray];
			while ((line = reader.readLine()) != null) {
				strArray = line.split(",");
				if(strArray.length != sizeOfArray){
					log("The line " + line + "does not contain all the required data. Not processing it");
				} else {
					if(line.startsWith(firstLineStartsWith)){
						for(int i=0; i< strArray.length; i++){
							String str = strArray[i];
							if(i==0){
								continue; // Skipping "Firstname" header
							}
							if(i==1){
								str = "User"; //Changing header from "SecondName" to "User" 
							}
							if(str.equalsIgnoreCase("timestamp")){
								str = str + ",Unix TimeStamp"; // Adding "Unix TImeStamp Header"
							}
							writer.print(str + ( i < strArray.length-1 ? "," : "" ));
						}
						writer.println();
						continue;
					}
					String firstName = strArray[0].trim();
					String lastName = strArray[1].trim();
					String org = strArray[2].trim();
					String user = users.get(firstName+lastName);
					String orgFromMap = orgs.get(org);
					if(user == null){
						user = USER+(users.size()+1);
						users.put(firstName+lastName, user);
					}
					if(orgFromMap == null){
						orgFromMap = ORG+(orgs.size()+1);
						orgs.put(org, orgFromMap);
					}
					//Adding unix timestamp
					String time = strArray[4];
					long unixTimeInSeconds = convertTimeToUnixTimeStamp(time);
					
					//Updating array
					strArray[0] = user;
					strArray[1] = orgFromMap;
					strArray[2] = strArray[3];
					strArray[3] = strArray[4];
					strArray[4] = String.valueOf(unixTimeInSeconds);
					writeToFile(strArray, writer);
				}
			}
			writer.close();
		} catch (FileNotFoundException e){
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		} 
	}
	
	private long convertTimeToUnixTimeStamp(String time) {
		DateFormat dfm = new SimpleDateFormat("MM/dd/yy HH:mm");
		try {
			long unixTimeMillis = dfm.parse(time).getTime();
			log("Originnal : " + time + " Unix : " + unixTimeMillis/1000);
			return unixTimeMillis/1000;
		} catch (ParseException e) {
			e.printStackTrace();
		}
		return 0; 
	}

	private void writeToFile(String[] strArray, PrintWriter writer) {
		try{
			for(int i=0; i< strArray.length; i++){
				writer.print(strArray[i] + ( i < strArray.length-1 ? "," : "" ));
			}
			writer.println();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	static void log(Object... obj) {
		for (Object o : obj) {
			System.out.println(o);
		}
	}
}
