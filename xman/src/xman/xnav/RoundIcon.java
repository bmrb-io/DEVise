package xman.xnav;

import java.awt.*;
import java.util.*;
import java.lang.String;
import java.lang.Math;
import javax.swing.*;
import java.awt.geom.*;
import java.awt.image.BufferedImage;

public class RoundIcon extends JComponent {
    JTabbedPane tabbedPane;
    int[] indexList;
    MergingNode mergeNode;
    int activeCnt;
    LinkedList activeList = new LinkedList();
    ImageIcon mergeIcon;
    BufferedImage im;
    Graphics2D graph;
    int w = 20;  // width of the image
    int h = 20;  // height of the image
    int arcNumber = 8;

    public RoundIcon(JTabbedPane tabbedPane) {
	this(null, tabbedPane);
    }

    public RoundIcon(MergingNode mergeNode, JTabbedPane tabbedPane) {
	this.mergeNode = mergeNode;
	this.tabbedPane = tabbedPane;
	this.activeCnt = getActiveCnt(tabbedPane);
	this.indexList = getIndexList(); // for mergeNode or tabList if no mergeNode
	// dynamically change widgets of pie
	arcNumber = activeCnt;
	im = new BufferedImage(w+1, h+1, BufferedImage.TYPE_3BYTE_BGR);
	graph = im.createGraphics();
	if (mergeNode != null)
	    drawMergeIcon();
    }
    public void drawMergeIcon() {
	drawMergeIcon(Color.white);
    }
    public void drawMergeIcon(Color bgColor) {
	if (arcNumber == 0) return;
	graph.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

	int intervalAngle = 360/arcNumber;  // here intervalAngle = 45
	int X = 0;
	int Y = 0;
	graph.setColor(bgColor);
	graph.fillRect(X,Y, w+1, h+1);
	graph.setColor(Color.black);
	graph.fillOval(X,Y,w-1, h-1);

	int prevAngle = 90;
	for (int i = 0; i < arcNumber; i++) {
	    if (i < activeCnt)
		graph.setColor(Color.white);
	    else
		graph.setColor(Color.lightGray);

	    if (isInIndexList(i))
		graph.setColor(Color.red);

	    graph.fillArc(1, 1, w-2, h-2, prevAngle-3, -intervalAngle+6);
	    graph.setColor(Color.black);
	    //graph.drawArc(0, 0, w, h, i*intervalAngle+1, intervalAngle-1);
	    graph.drawArc(0, 0, w-1, h-1, prevAngle, -intervalAngle);
	    //System.out.println("new w is: " +(int)(w/2*(1+Math.cos((double)((Math.PI/2 + i*intervalAngle)/180*Math.PI)))));
	    //System.out.println("new h is: " +(int)(h/2*(1-Math.sin((double)((Math.PI/2 + i*intervalAngle)/180*Math.PI)))));
	    //graph.drawLine(w/2, h/2, (int)(w/2*(1+Math.cos((double)((Math.PI/2 + i*intervalAngle)/180*Math.PI)))), (int)(h/2*(1-Math.sin((double)((Math.PI/2 + i*intervalAngle)/180*Math.PI)))));
	    prevAngle -= intervalAngle;
	   
	}
	//graph.drawLine(w/2, h/2, w/2, h/2);
    }

    public ImageIcon getMergeIcon() {
	mergeIcon = new ImageIcon(im);
	if (activeCnt == 0) return null;
	else return mergeIcon;
    }

    private boolean isInIndexList(int index) {
	if (indexList == null) {
	    System.out.println("indexList is NULL, so return false for checking");
	    return false;
	}
	boolean inList = false;
	for (int i = 0; i < indexList.length; i++) {
	    if (indexList[i] == index) {
		inList = true;
		break;
	    }
	}
	return inList;
    }

    private int indexOfActiveList(String runName) {
	int index = -1;
	
	for (int i = 0; i < activeList.size(); i++) {
	    if (((String)activeList.get(i)).indexOf(noStar(runName)) > -1) {
		index = i;
		break;
	    }
	}

	return index;
    }

    // get the index of tabbedPane with the tabname
    protected int [] getIndexList() {
	if (tabbedPane == null) return null;
	if (mergeNode == null) return null;

	int size = mergeNode.getListSize();
	if (size == 0) return null;

	LinkedList mergingList =  mergeNode.getMergingList();
	int [] indexList = new int[size];

	for (int i = 0; i < mergingList.size(); i++) {
	    String runName = (String)mergingList.get(i);
	    indexList[i] = indexOfActiveList(runName);
	}

	return indexList;
    }
    
    public int getActiveCnt(JTabbedPane tabbedPane) {
	if (tabbedPane == null) return 0;

	int Cnt = tabbedPane.getTabCount();
	int ActiveCnt = 0;
	for (int i = 0; i < Cnt; i++) {
	    String tabName = tabbedPane.getTitleAt(i);
	    if ((tabName.indexOf("+") == -1) && (!tabName.equals("blank"))) {
		ActiveCnt++;
		activeList.add(tabName);
	    }
	}

	return ActiveCnt;
    }

    public ImageIcon updateTabIcon(String tabName, Color bgColor) {
	if (tabbedPane  == null) return null;

	// parse tabName into tabName list for both single or combined
	indexList = getTabNameList(tabName);
	    
	// create icon for each tab
	drawMergeIcon(bgColor);

	return getMergeIcon();
    }

    protected int[] getTabNameList(String tabName) {
	LinkedList tabNameList = new LinkedList();
	if (tabName.indexOf("+") == -1) {
	    tabNameList.add(tabName.trim());
	    System.out.println(tabName);
	}
	else {
	    StringTokenizer token = new StringTokenizer(tabName, "+");
	    int tokenCnt = token.countTokens();
	    while (token.hasMoreTokens()) {
		String singleName = token.nextToken().trim();
		if (singleName.length() > 0) {
		    tabNameList.add(singleName);
		    System.out.println(singleName);
		}
	    }
	}
	int [] nameIndexList = new int[tabNameList.size()];
	for (int j = 0; j < tabNameList.size(); j++) {
	    nameIndexList[j] = indexOfActiveList((String)tabNameList.get(j));
	    //System.out.println("indexList is: "+indexList[j]);
	}
	return nameIndexList;
    }

    protected String noStar(String stringName) {
	// get rid of the last star in the stringName if any
	if (stringName.indexOf("*") == -1)
	    return stringName;
	else 
	    return stringName.substring(0, stringName.length()-1);
    }

}
 
   

    
