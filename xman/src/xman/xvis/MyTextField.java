// $Id$
package xman.xvis;

import java.awt.Color;
import javax.swing.JTextField;

//This class extends TextField to put background and foreground
//colors to the TextField and make it editable according to the options.
public class MyTextField extends JTextField {
  public MyTextField (boolean b) {super (); setup (b);}
  public MyTextField (String text, boolean b) {super (text); setup (b);}
  public MyTextField (int columns, boolean b) {super (columns); setup (b);}
  public MyTextField (String text, int columns, boolean b) {
    super (text, columns);
    setup (b);
  }

  void setup (boolean b) {
    this.setBackground (Color.white);
    this.setForeground (Color.black);
    this.setEditable (b);
  }
}
