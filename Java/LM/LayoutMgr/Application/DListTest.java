import java.lang.*;
import COM.objectspace.jgl.*;



public class DListTest {
  public static void main(String args[]) {
    DList list = new DList();
    for (int i = 0; i < 10; i++) {
      list.add(String.valueOf(i));
    }

    for (DListIterator d1 = list.begin(); d1.hasMoreElements(); ) {
      String elt1 = (String)d1.nextElement();
      for (DListIterator d2 = list.begin(); d2.hasMoreElements(); ) {
        String elt2 = (String)d2.nextElement();
        System.out.println("Distance = " + d1.distance(d2));
        if (d2.index() <= d1.index()) {
          continue;
        }
        System.out.println(elt1 + "," + elt2);
      }
    }
  }
}
