package Optimizer;

import Iterators.*;
import Types.*;
import java.util.*;

public interface PlanNode {
     TypeDesc[] getOutputTypes();
     Vector getProjList();
	Iterator createIterator() throws java.io.IOException;
}
