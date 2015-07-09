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

// This class is used to aggregate corn field output per sections

// ------------------------------------------------------------------------

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Collections;
import java.util.List;

/*
averages : longitude[0],latitude[1],time[3],moist[7] - weighted average, pass status[8], pass number[9], serial number [10], flag number [15]
sum : flow[2], cycles[4], distance[5], swath[6], bushels [16]
constants : field id [11], load id [12], grain[13], variety [14]
*/

//TEMPTEMP -- please copy comment lines to output

public class Aggregator {
	
	private double aggrDistance = 60; // aggrDistance and aggrSwath represent the dimensions we want to aggregate to
	private double aggrSwath = 60;
	private String dirPath = "C:\\DEVise\\";
	private String inputFileName = "cornYield2.csv";
	private String outputFileName = "cornYieldAggregated2.csv";
	private String inputFilePath = dirPath + inputFileName;
	private String outputFilePath = dirPath + outputFileName;
	private String firstLineStartsWith = "#";
	private final static double EARTH_RADIUS_FOOT = 6371*3280.84;
	private double leftMostLongitude = 0;
	private double bottomMostLatitude = 0;
	private double rightMostLongitude = 0;
	private double topMostLatitude = 0;
	double maxLength = 0;
	double maxBreadth = 0;
	double maxArea = 0;
	private List<List<Section>> field = new ArrayList<List<Section>>();
	private String fieldId = "\"F1: 339\"";
	private String loadId = "\"L1: \"";
	private String grain = "\"Corn\"";
	private String variety = "\"Corn Mix\"";
	private int numOfColumnsInInput = 17;
	private PrintWriter writer;

	public static void main(String[] args) {
		Aggregator aggregator = new Aggregator();
		aggregator.process(args);
	}
	
	private void process(String[] args) {
		if(args.length == 4 && args[0] != null && args[1] != null && args[2] != null && args[3] != null){
			inputFilePath = args[0];
			outputFilePath = args[1];
			aggrDistance = Double.parseDouble(args[2]);
			//TEMPTEMP -- what is this? --- Input from the user
			aggrSwath = Double.parseDouble(args[3]);
		} else {
			log("Incorrect number of arguments. Correct usage : java Aggregator <inputFilePath> <outputFilePath> <aggregateLength> <aggregateBreadth>");
			return;
		}
		try {
			writer = new PrintWriter(new BufferedWriter(new FileWriter(outputFilePath)));
			DateFormat dateFormat = new SimpleDateFormat("dd-MMM-yyyy HH:mm:ss");
			writer.println(firstLineStartsWith + "This aggregation was run on " + dateFormat.format(Calendar.getInstance().getTime()));
		} catch (IOException e) {
			e.printStackTrace();
		}
		findExtremesAndMaximums();
		sectionize();
		addRecordsToSections();
		calculateAggrPerSection();
		writer.close();
	}

	//This method finds the boundaries of the field
	private void findExtremesAndMaximums(){
		BufferedReader reader;
		try {
			reader = new BufferedReader(new FileReader(inputFilePath));
			String line = null;
			String [] strArray = new String[numOfColumnsInInput];
			double longitude = 0;
			double latitude = 0;
			List<Double> longDouble = new ArrayList<Double>();
			List<Double> latDouble = new ArrayList<Double>();
			while ((line = reader.readLine()) != null) {
				strArray = line.split(",");
				if(line.startsWith(firstLineStartsWith)){
					for(int i=0; i< strArray.length; i++){
						writer.print(strArray[i] + ( i < strArray.length-1 ? "," : "" ));
					}
					writer.println();
					continue;
				}
			    longitude = Double.parseDouble(strArray[0]); 
			    latitude  = Double.parseDouble(strArray[1]);
			    longDouble.add(longitude);
			    latDouble.add(latitude);
			}
			Collections.sort(longDouble);
			Collections.sort(latDouble);
			leftMostLongitude = longDouble.get(0);
			bottomMostLatitude = latDouble.get(0);
			rightMostLongitude = longDouble.get(longDouble.size()-1);
			topMostLatitude = latDouble.get(latDouble.size()-1);
			log(leftMostLongitude,rightMostLongitude,topMostLatitude,bottomMostLatitude);
			//TEMPTEMP -- aren't maxLength and maxBreadth the same thing here? - No they are not. Field could be rectangular
			maxLength = getDistanceBetweenCoordinates(topMostLatitude,leftMostLongitude,topMostLatitude,rightMostLongitude);
			maxBreadth = getDistanceBetweenCoordinates(topMostLatitude,leftMostLongitude,bottomMostLatitude,leftMostLongitude);
			maxArea = maxLength*maxBreadth;
			log(maxLength,maxBreadth,maxArea);
		} catch(FileNotFoundException e){
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	
	static double getDistanceBetweenCoordinates(double lat1, double long1, double lat2, double long2){
		double distance = 0;
		lat1 = Math.toRadians(lat1);
		lat2 = Math.toRadians(lat2);
		long1 = Math.toRadians(long1);
		long2 = Math.toRadians(long2);
		double x = (long2-long1) * Math.cos((lat1+lat2)/2);
		double y = (lat2-lat1);
		distance = Math.sqrt(x*x + y*y) * EARTH_RADIUS_FOOT;
		return distance;
	}
	
	//Divide the field into sections of desired size
	private void sectionize() {
		//TEMPTEMP -- is this correct?  what if the size we want to aggregate to doesn't do a good job of tiling the overall area?  I think you have to base the number of sections on the width and height individually as opposed to the overall area
		//-- This takes care of it
		int numOfSections = (int)(maxArea/(aggrDistance*aggrSwath));
		log(numOfSections);
		double remainingArea = maxArea - numOfSections*aggrDistance*aggrSwath;
		log(remainingArea);
		if(remainingArea != 0){
			int sectionsPerRow = (int)Math.ceil((maxLength/aggrDistance));
			numOfSections += (sectionsPerRow+1);
		}
		double [] iterLongLat = new double [] {leftMostLongitude,topMostLatitude};
		List<Section> row = new ArrayList<Section>();
		for(int i=0; i<numOfSections; i++){
			Coordinate topLeft = new Coordinate(iterLongLat[0], iterLongLat[1]);
			Coordinate topRight = findNextCoordinate(topLeft, 90); 
			Coordinate bottomRight = findNextCoordinate(topRight, 180); 
			Coordinate bottomLeft = findNextCoordinate(bottomRight, 270);
			Section section = new Section(topLeft, topRight, bottomRight, bottomLeft);
			row.add(section);
			if(topRight.getLongitude() == rightMostLongitude || i == numOfSections-1){
				iterLongLat = updateTopLeft(row.get(0).getBottomLeftCorner());
				field.add(row);
				row = new ArrayList<Section>();
			} else {
				iterLongLat = updateTopLeft(topRight);
			}
		}
	}
	
	private double[] updateTopLeft(Coordinate topLeft) {
		double iterLong = topLeft.getLongitude();
		double iterLat = topLeft.getLatitude();
		return new double [] {iterLong,iterLat};
		
	}

	private Coordinate findNextCoordinate(Coordinate prevCorner, double bearing) {
		double longitude = prevCorner.getLongitude();
		double latitude = prevCorner.getLatitude();
		double d;
		double c;
		Coordinate nextCorner;
		if(bearing == 90 || bearing == 270){
			d = Math.sin(Math.toRadians(bearing))*aggrDistance;
			c = EARTH_RADIUS_FOOT*2*Math.PI*Math.cos(Math.toRadians(latitude));
			double oldKmEast = (longitude/360)*c;
			double newLong = ((oldKmEast + d)/c)*360;
			if(newLong > rightMostLongitude){
				newLong = rightMostLongitude;
			} 
			nextCorner = new Coordinate(newLong, latitude);
		} else {
			d = aggrSwath; 
			c = EARTH_RADIUS_FOOT*2*Math.PI*Math.sin(Math.abs(Math.toRadians(longitude)));
			double oldKmNorth = (latitude/360)*c;
			double newLat = ((oldKmNorth - d)/c)*360;
			if(newLat < bottomMostLatitude){
				newLat = bottomMostLatitude;
			}
			nextCorner = new Coordinate(longitude, newLat);
		}
		return nextCorner;
	}

	static void log(Object... obj) {
		for (Object o : obj) {
			System.out.println(o);
		}
	}
	
	//Adds records to corresponding sections based on the lat and long
	private void addRecordsToSections() {
		BufferedReader reader;
		try {
			reader = new BufferedReader(new FileReader(inputFilePath));
			String line = null;
			String [] strArray = new String[17];
			double numRecordsAdded = 0;
			List<Record> allRecords = new ArrayList<Record>();
			while ((line = reader.readLine()) != null) {
			    if(line.startsWith(firstLineStartsWith))
			    	continue;
			    strArray = line.split(",");
			    double longitude = Double.parseDouble(strArray[0]);
			    double latitude = Double.parseDouble(strArray[1]);
			    Record record = new Record(strArray, longitude, latitude);
			    allRecords.add(record);
			    outer:
			    for(List<Section> row : field){
			    	for(Section section : row){
			    		Coordinate midPoint = section.getMidPoint();
			    		double distance = getDistanceBetweenCoordinates(latitude, longitude, midPoint.getLatitude(), midPoint.getLongitude());
			            double diagHalf = Math.sqrt(aggrDistance*aggrDistance + aggrSwath*aggrSwath)/2;
			            if(distance <= diagHalf){
			            	section.addRecord(strArray);
			            	record.setAdded(true);
			            	numRecordsAdded++;
			            	break outer;
			            }
			    	}
			    }
			}
			log("Total num of records :" +  allRecords.size());
			log("num records added::" + numRecordsAdded);
			/*for(Record record : allRecords){
				if(!record.isAdded()){
					log("Record::" + record);
				}
			}*/
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		
	}

	private void calculateAggrPerSection() {
		int outputArraySize = 13;
		for(List<Section> row : field){
	    	for(Section section : row){
	    		double outputArray [] = new double [outputArraySize];
	    		List<String []> records = section.getRecords();
	    		int size = records.size();
	    		if(size != 0){
	    			for(String [] strArray : records){
						//TEMPTEMP -- what's special about 7?
	    				// index 7 represents moist, and therefore needs to be calculated per bushel
		    			for(int i=0; i<7; i++){
		    				outputArray[i] += Double.parseDouble(strArray[i]);
		    			}
		    			for(int i=8; i<11; i++){
		    				outputArray[i] += Double.parseDouble(strArray[i]);
		    			}
		    			//Sum flags
						//TEMPTEMP -- why is 15 going to 11 here? -- there are constants such as grain and grain variety in input
		    			outputArray[11] += Double.parseDouble(strArray[15]);
		    			
		    			int distance = Integer.parseInt(strArray[5]);
		    			double bushels = 0;
		    			if(distance != 0){
		    				bushels = Double.parseDouble(strArray[16]);
		    				outputArray[12]  += bushels;
		    			}
		    			//Weighted moisture
		    			double moist = Double.parseDouble(strArray[7]);
		    			outputArray[7] += moist*bushels;
		    		}
		    		//averages : longitude[0],latitude[1],time[3],pass status[8],pass number[9],serial number[10],flag number[11]
		    		for(int i=0;i<2;i++){
		    			outputArray[i] = outputArray[i]/size;
		    		}
		    		outputArray[3] = outputArray[3]/size;
		    		//moist[7] - weighted average
		    		outputArray[7] = outputArray[7]/outputArray[12];
		    		for(int i=8;i<12;i++){
		    			outputArray[i] = outputArray[i]/size;
		    		}
		    		formatOutput(outputArray);
	    		}
	        }
	    }
	}
	
	private void formatOutput(double [] outputArray){
		String [] strArray = new String [13];
		for(int i=0;i<2;i++){
			strArray[i] = String.valueOf(Math.round(outputArray[i]*1000000.0)/1000000.0);
		}
		strArray[2] = String.valueOf(Math.round(outputArray[2]*10.0)/10.0);
		strArray[3] = String.valueOf(Math.round(outputArray[3]));
		for(int i=4;i<7;i++){
			strArray[i] = String.valueOf(Math.round(outputArray[i]));
		}
		strArray[7] = String.valueOf(Math.round(outputArray[2]*100.0)/100.0);
		for(int i=8;i<11;i++){
			strArray[i] = String.valueOf(Math.round(outputArray[i]));
		}
		strArray[11] = String.valueOf(Math.round(outputArray[11]*100.0)/100.0);
		strArray[12] = String.valueOf(outputArray[12]);
		writeToFile(strArray);
	}
	
		
	private void writeToFile(String[] strArray) {
		for(int i=0;i<11;i++){
			writer.print(strArray[i] + ",");
		}
		writer.print(fieldId + "," + loadId + "," + grain + "," + variety + "," + strArray[11] + "," + strArray[12]);
		writer.println();
	}
	
	public double getAggrDistance() {
		return aggrDistance;
	}

	public double getAggrSwath() {
		return aggrSwath;
	}
}

final class Record{
	private final Coordinate coordinate;
	private final String [] data;
	private boolean isAdded = false;

	Record(String [] data, double longitude, double latitude){
		this.data = data;
		this.coordinate = new Coordinate(longitude, latitude);
	}
	
	public Coordinate getCoordinate() {
		return coordinate;
	}
	
	public String [] getData() {
		return data;
	}
	
	
	public boolean isAdded() {
		return isAdded;
	}

	public void setAdded(boolean isAdded) {
		this.isAdded = isAdded;
	}
	
	@Override
	public String toString() {
		return coordinate.toString();
	}
	
	@Override
	public boolean equals(Object obj) {
		if (obj == this) {
            return true;
        }
 
        if (!(obj instanceof Record)) {
            return false;
        }
        Record record = (Record) obj;
        return this.coordinate.equals(record.coordinate);
	}
	
	@Override
	public int hashCode() {
		return coordinate.hashCode();
	}
}

class Section {
	private Coordinate topLeftCorner;
	private Coordinate topRightCorner;
	private Coordinate bottomRightCorner;
	private Coordinate bottomLeftCorner;
	private Coordinate midPoint;
	private List<String []> records = new ArrayList<String[]>();
	
	Section(Coordinate topLeftCorner, Coordinate topRightCorner, Coordinate bottomRightCorner, Coordinate bottomLeftCorner){
		this.topLeftCorner = topLeftCorner;
		this.topRightCorner = topRightCorner;
		this.bottomRightCorner = bottomRightCorner;
		this.bottomLeftCorner = bottomLeftCorner;
	}
	
	public void addRecord(String [] record) {
		 records.add(record);
	}
	
	public List<String []> getRecords() {
		return records;
	}

	private void calculateMidpoint(){
		double midLang = (topLeftCorner.getLongitude() + topRightCorner.getLongitude())/2;
		double midLat = (topLeftCorner.getLatitude() + bottomLeftCorner.getLatitude())/2;
		this.midPoint = new Coordinate(midLang, midLat);
	}
	
	public Coordinate getMidPoint() {
		calculateMidpoint();
		return this.midPoint;
	}
	
	/*double getArea(){
		double area = 0;
		double length = (distanceBetweenCoordinates(topLeftCorner, topRightCorner) + distanceBetweenCoordinates(bottomLeftCorner, bottomRightCorner))/2;
		double breadth = (distanceBetweenCoordinates(topLeftCorner, bottomLeftCorner) + distanceBetweenCoordinates(topRightCorner, bottomRightCorner))/2;
		log(length,breadth);
		area = length*breadth;
		return area;
	}*/
	
	@Override
	public String toString(){
		return "(" + topLeftCorner + "     " + topRightCorner + "\n" + bottomLeftCorner + "     " + bottomRightCorner + ")";
	}

	public Coordinate getTopLeftCorner() {
		return topLeftCorner;
	}

	public Coordinate getTopRightCorner() {
		return topRightCorner;
	}

	public Coordinate getBottomRightCorner() {
		return bottomRightCorner;
	}

	public Coordinate getBottomLeftCorner() {
		return bottomLeftCorner;
	}
	
	static void log(Object... obj) {
		for (Object o : obj) {
			System.out.println(o);
		}
	}
}

final class Coordinate {
	private final double longitude;
	private final double latitude;
	
	Coordinate(double longitude, double latitude){
		this.longitude = longitude;
		this.latitude = latitude;
	}
	
	public double getLongitude() {
		return longitude;
	}

	public double getLatitude() {
		return latitude;
	}
	
	@Override
	public String toString(){
		return longitude + "," + latitude;
	}
	
	@Override
	public boolean equals(Object obj) {
		if (obj == this) {
            return true;
        }
 
        if (!(obj instanceof Coordinate)) {
            return false;
        }
        Coordinate coordinate = (Coordinate) obj;
        if(longitude == coordinate.longitude && latitude == coordinate.latitude){
        	return true;
        }
        return false;
    }
	
	@Override
	public int hashCode() {
		final int prime = 31;
        int result = 1;
        long temp;
        temp = Double.doubleToLongBits(latitude);
        result = prime * result + (int) (temp ^ (temp >>> 32));
        temp = Double.doubleToLongBits(longitude);
        result = prime * result + (int) (temp ^ (temp >>> 32));
        return result;
	}
}	

