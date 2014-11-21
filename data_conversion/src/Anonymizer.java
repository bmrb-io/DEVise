import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.util.HashMap;
import java.util.Map;


public class Anonymizer {
	
	private String dirPath = "C:\\DEVise\\";
	private String inputFileName = "example.csv";
	private String outputFileName = "exampleOutput.csv";
	private String inputFilePath = dirPath + inputFileName;
	private String outputFilePath = dirPath + outputFileName;
	private String firstLineStartsWith = "#";
	private Map<String, String> users = new HashMap<String, String>();
	private Map<String, String> orgs = new HashMap<String, String>();
	private final static String USER = "user";
	private final static String ORG = "org";

	public static void main(String[] args) {
		Anonymizer anonymizer = new Anonymizer();
		anonymizer.anonymize();
	}

	private void anonymize() {
		BufferedReader reader;
		try {
			reader = new BufferedReader(new FileReader(inputFilePath));
			String line = null;
			String [] strArray = new String[6];
			while ((line = reader.readLine()) != null) {
				if(line.startsWith(firstLineStartsWith))
			    	continue;
				strArray = line.split(",");
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
				strArray[0] = user;
				strArray[1] = orgFromMap;
				writeToFile(strArray);
			}
		} catch (FileNotFoundException e){
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	private void writeToFile(String[] strArray) {
		PrintWriter writer;
		try {
			writer = new PrintWriter(new BufferedWriter(new FileWriter(outputFilePath, true)));
			for(int i=0; i< strArray.length; i++){
				if(i != 2) {
					writer.print(strArray[i] + ( i < strArray.length-1 ? "," : "" ));
				}
			}
			writer.println();
			writer.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	static void log(Object... obj) {
		for (Object o : obj) {
			System.out.println(o);
		}
	}
}
