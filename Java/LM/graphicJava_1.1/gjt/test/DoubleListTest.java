package gjt.test;

import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import gjt.*;

/**
 *
 * @version 1.0, Feb, 1997
 * @author  David Geary
 * @see     gjt.test.UnitTest
 * @see     gjt.DoubleList
 */
public class DoubleListTest extends UnitTest {
	private Button   printButton = new Button("Print ...");
	private String[] leftStrs = { "1", "2", "3", "4", "5", 
	                              "6", "7", "8", "9", "10"};
	private String[] rightStrs = { "one", "two", "three", "four", 
	                               "five", "six", "seven", 
								   "eight", "nine", "ten"};
	DoubleList dlist = new DoubleList(leftStrs, rightStrs);

    public String title() { 
        return "DoubleList Test";             
    }
    public Panel centerPanel() { 
		Panel panel      = new Panel();
		Panel printPanel = new Panel();

		printPanel.setLayout(new BorderLayout(5,5));
		printPanel.add("Center", printButton);
		printPanel.add("South",  new Separator());

		panel.setLayout(new BorderLayout(5,5));
		panel.add("North", printPanel);
		panel.add("Center", dlist);

		printButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent event) {
				String[] left  = dlist.getLeftSideItems();
				String[] right = dlist.getRightSideItems();
				String[] sleft = dlist.getLeftSideSelectedItems();
				String[] sright= dlist.getRightSideSelectedItems();

				System.out.println("Left Side Items:  ");
				for(int i=0; i < left.length; ++i)
					System.out.println(left[i]);
				System.out.println();
	
				System.out.println("Right Side Items:  ");
				for(int i=0; i < right.length; ++i)
					System.out.println(right[i]);
				System.out.println();
	
				System.out.println("Left Side Selected Items:  ");
				for(int i=0; i < sleft.length; ++i)
					System.out.println(sleft[i]);
				System.out.println();
	
				System.out.println("Right Side Selected Items:  ");
				for(int i=0; i < sright.length; ++i)
					System.out.println(sright[i]);
				System.out.println();
			}
		});
		return panel;
	}
}
