package xman.xnav;

import java.awt.*;
import java.util.*;
import java.lang.String;
import java.lang.Math;
import javax.swing.*;
import java.awt.geom.*;
import java.awt.image.BufferedImage;

public class ThumbNailIcon extends JComponent {

    int[] thumbnails;  // data array
    int thumbnailCol; // how many data, x axis
    int thumbnailRow; // max data value, y axis
    int zeroCheck;

    ImageIcon thumbIcon;
    BufferedImage im;
    Graphics2D graph;
    int w = 16;  // width of the image
    int h = 18;  // height of the image


    public ThumbNailIcon(int[] thumbnails, int thumbnailRow) {
	this.thumbnails = thumbnails;

	this.thumbnailRow = thumbnailRow;
	this.thumbnailCol = thumbnails.length;
	this.zeroCheck = -1; // haven't checked

	// dynamically change widgets of pie
	im = new BufferedImage(w+5, h+5, BufferedImage.TYPE_3BYTE_BGR);
	graph = im.createGraphics();
	drawThumbNailIcon();
    }
    public void drawThumbNailIcon() {
	drawThumbNailIcon(Color.white);
    }
    public void drawThumbNailIcon(Color bgColor) {

	graph.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

	int X = 0;
	int Y = 0;
	graph.setColor(bgColor);
	graph.fillRect(X,Y, w+5, h+5);
	graph.setColor(Color.blue);
	graph.drawRect(X,Y, w, h);

	int cellw = (int) w/thumbnailCol;
	int cellh = (int) h/thumbnailRow;

	int isZero = 0;
	for (int i = 0; i < thumbnailCol; i++) {
	    // get each data and draw/fill rectangle
	    int data = thumbnails[i];
	    isZero += data;
	    int thiscellh = data * cellh;
	    X = i * cellw;
	    Y = h - thiscellh;
	    graph.fillRect(X, Y, cellw, thiscellh);
	}

	if (isZero == 0)
	    zeroCheck = 0; // zero data
	else
	    zeroCheck = 1; // non zero data

    }

    public ImageIcon getThumbNailIcon() {
	thumbIcon = new ImageIcon(im);
	return thumbIcon;
    }

    public int getZeroCheck() {
	return zeroCheck;
    }

}
