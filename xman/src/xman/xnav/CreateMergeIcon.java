package xman.xnav;

import java.awt.*;
import java.lang.String;
import javax.swing.*;
import java.awt.geom.*;
import java.awt.image.BufferedImage;

public class CreateMergeIcon extends JComponent {
    MergingNode mergeNode;
    ImageIcon mergeIcon;
    BufferedImage im;
    Graphics2D graph;
    int imIndex; // the number shown in the image
    int w = 36;  // width of the image
    int h = 20;  // height of the image
    int rowNumber = 2;
    int colNumber = 4;

    public CreateMergeIcon(MergingNode mergeNode) {
	this.mergeNode = mergeNode;
	imIndex = mergeNode.getListSize();;
	im = new BufferedImage(w, h, BufferedImage.TYPE_3BYTE_BGR);
	graph = im.createGraphics();
	drawMergeIcon();
    }
    
    public void drawMergeIcon() {
	graph.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

	int intervalW = w/colNumber;  // here intervalW = 9
	int intervalH = h/rowNumber;  // here intervalH = 10
	for (int i = 0; i < rowNumber; i++) {
	    for (int j = 0; j < colNumber; j++) {
		graph.setColor(Color.white);

		graph.draw3DRect(j*intervalW, i*intervalH, intervalW, intervalH, false);
		graph.fill3DRect(j*intervalW, i*intervalH, intervalW, intervalH, false);
		String text = String.valueOf(i*colNumber+j+1);
		graph.setPaint(Color.black);
		graph.setFont(new Font(text, Font.PLAIN, 6));
		graph.drawString(text, j*intervalW+intervalW/2-1, i*intervalH+intervalH/2+1);
		
	    }
	}
	graph.setColor(Color.black);
	graph.draw3DRect(0,0,w+1,h+1, false);
	//graph.fill3DRect(0,0,w,h,false);
    }

    public ImageIcon getMergeIcon() {
	mergeIcon = new ImageIcon(im);
	if (mergeIcon == null)
	    System.out.println("MergeIcon got NULL, check it out!!");
	return mergeIcon;
    }
}
 
   

    
