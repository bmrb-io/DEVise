package Iterators;

import java.io.*;
import Types.*;

public interface Iterator {

	/** getFirst should be called before getNext */

	Tuple getFirst() throws IOException;

	/** getNext returns a valid tuple object or null to signal the end of
	iterator */

	Tuple getNext() throws IOException;
}
