import java.util.*;
import java.lang.*;

public class ClusterList
{
    // temp: define largest predicat number
    private static final int PRED_NUM = 100;
    
    public Cluster[] clusters;

    public ClusterList()
    {
	for (int i=0; i<PRED_NUM; i++)
	    clusters[i] = null;
    }

    public void addSubscript(int sid, int[] index, int pred_num)
    {
	if (clusters[pred_num] == null) {
	    // create a new Cluster object 
	    // and insert into the clusterlist array
	    Cluster c = new Cluster(pred_num);
	    clusters[pred_num] = c;
	}
	
	// add the subscript
	clusters[pred_num].addSubscript(sid, index);
    }
	    
    // return the clusters in this list
    // put them in a vector?
    public Vector getClusters()
    {
	Vector v = new Vector();
	
	for (int i=0; i<PRED_NUM; i++) 
	    if (clusters[i] != null)
		v.add(clusters[i]);
      
	return v;
    }
		    
}
