import java.util.*;
import java.lang.*;

public class Cluster
{
    // number of predicates in this cluster
    private static int pred_num = 0;

    // vecter of subscriptions, 
    // each element in vector is an int array,
    // first element is SubID, 
    // others are predicates' index in bit vector 
    public Vector subs = new Vector(); 

    public Cluster (int predicate) 
    {
	pred_num = predicate;
    }

    public void addSubscript(int sid, int index[])
    {
	int array[];
	
	array = new int[pred_num+1];
	// first element is the SubID
	array[0] = sid;
	for (int i=1; i<=pred_num; i++) 
	    array[i] = index[i-1];

	subs.add(array);
    }
	
    public Vector clusterMatch(BitSet bitset)
    {
	Vector matched = new Vector();
	int array[];

	for (int i = 0; i < subs.size(); i++) {
	    boolean match = true;
	    array = (int[])subs.elementAt(i);

	    for (int j=1; j<=pred_num; j++) 
		if (! bitset.get(array[j])) {
		    match = false;
		    break;
		}

	    if (match)
		// have to use Integer object, stupid
		matched.add(new Integer(array[0]));
	}

	return matched;
    }

}
		    
