import DataSources.*;
import java.io.*;

public class Register {
	public static void main(String[] args) throws IOException {
		RelationManager relMngr =
			new RelationManager("idFile.dte", "defFile.dte");

		Reader r = new InputStreamReader(System.in);
		StreamTokenizer streamTok = new StreamTokenizer(r);
		StandardTable stable = new StandardTable();
		stable.read(streamTok);
		RelationId relId = relMngr.registerNewRelation(stable);
		System.out.print("Registered " + relId);
		StandardTable checkT = 
			(StandardTable) relMngr.createDataSource(relId);
		System.out.println(" as " + checkT.getString());
	}
}
