/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class MenuMaker.java
****************************************************************************/

//Packages 

package AwtUtil;

import java.lang.*;
import java.io.*;
import java.net.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import java.lang.reflect.*;
import java.util.*;
import COM.objectspace.jgl.*;
import gjt.*;
import AwtUtil.*;

// Class
/**
 * The <CODE>MenuMaker</CODE> class
 *
 * @author  Shilpa Lawande
 */
/*
class test {
  test() { }
  public void OnFly() { System.out.println("Fly");}
  public void OnEagle() { System.out.println("Eagle");}
  public void OnSnail() { System.out.println("Snail");}
}
*/
public class ToolBarMaker 
{
  private static final int PARSE_OK = 0;
  private static final int PARSE_FAILURE = 1;
  private static final int PARSE_PROCEED = 2;
  private static final int PARSE_EOF = 3;
  

  // Test code
  public static void main(String args[]) {
    ToolBarMaker tbmk = new ToolBarMaker(new test(), "sampleTB.res");
    Frame f = new Frame();
    f.setSize(500,500);
    ScrollPane pane = new ScrollPane();
    pane.setSize(100,100);
    f.add(pane, "Center");

    NiceToolbar tb = tbmk.getToolBarByName("Test");
    if (tb.isVertical()) {
      f.add(tb, "West");
    } else {
      f.add(tb, "North");
    }
    
    Panel p = new Panel();
    p.setSize(500, 500);
    pane.add(p);

    f.pack();
    f.show();
  }

  /** Grammar is as follows (BNF)
    ResourceFile :- ToolBarList
    ToolBarList :- ToolBar 
              | ToolBar ToolBarList 
    ToolBar :- BEGINTOOLBAR(ToolBarName) ToolBarType
               ToolBarButtonList
               ENDTOOLBAR

    ToolBarButtonList :- ToolBarButton 
                  | ToolBarButton ToolBarButtonList

    ToolBarButton :-  SPRINGYBUTTON Label GifPath HelpStr ActionName
                  |   STICKYBUTTON  Label GifPath HelpStr ActionName
                  |   DUALBUTTON Label GifPath HelpStr SingleModeAction MultiModeAction RaiseAction
                  | SPACER size
    
    ToolBarName :- any Java String
    GifPath :- any quoted Java String 
    HelpStr :- any quoted Java String
    ActionName :- any Java String
    SingleModeAction :- any Java String
    MultiModeAction :- any Java String
    RaiseAction :- any Java String

   */

  /* Public members */
  public String m_toolBarFile;  // an input file specifying how Toolbar appears
  public Object m_parent;    // an instance of  a class
                             // that implements the toolBar commands
  private DList m_toolBarList = new DList(); // creates toolbars in file
  NiceToolbar m_currentToolBar;        // current ToolBar being created

  public ToolBarMaker(Object parent, String toolbarFile) {
    m_parent = parent;
    m_toolBarFile = toolbarFile;
    if (!readFileAndParse()) {
      return;
    }
  }

  public NiceToolbar getToolBarByName(String name) {
    for (DListIterator d = m_toolBarList.begin(); d.hasMoreElements(); ) {
      NiceToolbar tb = (NiceToolbar)d.nextElement();
      if (tb.getName().equals(name)) {
        return tb;
      }
    }
    return null;
  }

  // read the toolBar file and create the toolBars and toolbars
  private boolean readFileAndParse() {
     // open the file    
    FileReader f;
    try {
      f = new FileReader(m_toolBarFile);
    }
    catch(FileNotFoundException e) {
      System.out.println("File Not Found: " + e.getMessage()); 
      return false;
    }
    StreamTokenizer s = new StreamTokenizer(f);
    s.slashStarComments(true);
    s.slashSlashComments(true);
    s.quoteChar(34);
    s.eolIsSignificant(true);
    if (parse(s) != PARSE_OK) {
      System.out.println("Parse Error!!");
      return false;
    }
    return true;
  }

  int nextIntToken(StreamTokenizer s) {
    int token;
    while (true) {
	    try {
	      token = s.nextToken();
	    } catch (IOException e) {
	      System.out.println(e.getMessage());
	      return -1;
	    }
	    if (s.ttype == StreamTokenizer.TT_EOF) {
	      return -1;
	    } else if (s.ttype == StreamTokenizer.TT_WORD) {
	      //System.out.println("Next token -> " + s.nval);
	      return (int)s.nval;
	    }
    }
  }
  String nextToken(StreamTokenizer s) {
    int token;
    while (true) {
	    try {
	      token = s.nextToken();
	    } catch (IOException e) {
	      System.out.println(e.getMessage());
	      return null;
	    }
	    if (s.ttype == StreamTokenizer.TT_EOF) {
	      return null;
	    } else if (s.ttype == StreamTokenizer.TT_WORD) {
	      //System.out.println("Next token -> " + s.sval);
	      return s.sval;
	    } else if (s.ttype == 34) {
        //System.out.println("Next token -> " + s.sval);
        return s.sval;
      } else {
        //System.out.println("Token type =" +  token);
      }

    }
  }

  int matchToken(StreamTokenizer s, String expTok) {
    String token = nextToken(s);
    if (token == null) return PARSE_EOF;
    if (token.equals(expTok)) {
      return PARSE_OK;
    } else {
      return PARSE_FAILURE;
    }
  }

  int parse(StreamTokenizer s) {
    //System.out.println("Parse");
    while(true) {
      int status = parseToolBar(s);
      if (status == PARSE_OK) {
        continue;
      } else if (status == PARSE_EOF) {
        return PARSE_OK;
      } else {
        return PARSE_FAILURE;
      }
    }
  }

  int parseToolBar(StreamTokenizer s) {
    //System.out.println("ParseToolBar");
	  int status = matchToken(s,"BEGINTOOLBAR");
    if (status != PARSE_OK){
	    return status;
	  } 
    String name = nextToken(s);
    if (isKeyWord(name)) {
      return PARSE_FAILURE;
    }
    String type = nextToken(s);
    Orientation orient;
    if (type.equals("VERTICAL")){
      orient = Orientation.VERTICAL;
    } else if (type.equals("HORIZONTAL")) {
      orient = Orientation.HORIZONTAL;
    } else {
      return PARSE_FAILURE;
    }
    m_currentToolBar = new NiceToolbar(name, orient);
		if (parseButtonList(s) != PARSE_OK) {
       return PARSE_FAILURE; 
    }
	  if (matchToken(s,"ENDTOOLBAR") != PARSE_OK) {
	    System.out.println("Parse Error: ENDTOOLBAR expected");
	    return PARSE_FAILURE;
	  }
    m_toolBarList.add(m_currentToolBar);
	  return PARSE_OK;
	}

  int parseButtonList(StreamTokenizer s) {
    //System.out.println("parseButtonList");
	  while(true) {
	    int status = parseButton(s);
	    if (status == PARSE_OK) {
	      continue;
	    } else if (status == PARSE_PROCEED) {
	      return PARSE_OK;
	    } else {
	      return PARSE_FAILURE;
	    }
	  }
	}

  int parseButton(StreamTokenizer s) {
    //System.out.println("ParseButton");
	  String t1 = nextToken(s);
    if (t1 == null) {
      return PARSE_EOF;
    }
	  if (t1.equals("ENDTOOLBAR")) {
	    s.pushBack();
	    return PARSE_PROCEED;
	  } 
    if (t1.equals("SPACER")) {
      int size = nextIntToken(s);
      if (size < 0) {
        return PARSE_FAILURE;
      }
      createSpacer(size);
    } else if (!(t1.equals("STICKYBUTTON") || t1.equals("SPRINGYBUTTON") ||
                 t1.equals("DUALBUTTON"))) {
      return PARSE_FAILURE;
    }
    String label = nextToken(s);
    if ((label == null) || isKeyWord(label)) {
      return PARSE_FAILURE;
    }
	  String gifPath = nextToken(s);
    if ((gifPath == null) || isKeyWord(gifPath)) {
      return PARSE_FAILURE;
    }
	  String helpStr = nextToken(s);
    if ((helpStr == null) || isKeyWord(helpStr)) {
      return PARSE_FAILURE;
    }
    String fn1 = nextToken(s);
    if ((fn1 == null) || isKeyWord(fn1)) {
      return PARSE_FAILURE;
    }
    if (t1.equals("DUALBUTTON")) {
      String fn2 = nextToken(s);
      if ((fn2 == null) || isKeyWord(fn2)) {
        return PARSE_FAILURE;
      } 
      String fn3 = nextToken(s);
      if ((fn3 == null) || isKeyWord(fn3)) {
        return PARSE_FAILURE;
      }
      createDualButton(label, gifPath, helpStr, fn1, fn2, fn3);
    } else {
      createSimpleButton(t1, label, gifPath, helpStr, fn1);
    }
    return PARSE_OK;
	}
  private ImageProducer getImageProducer(String imagePath) {
    URL url;
    try { 
      url = new URL("file:" + imagePath);
    } catch (Exception e) {
      System.out.println("Malformed url");
      return null;
    }
    ImageProducer ip;
    try {
      ip = (ImageProducer)url.getContent();
    } catch (Exception e) {
      System.out.println("Io Exception");
      return null;
    }
    return ip;
  }
  private void createDualButton(String label, String imagePath, 
                                String bubbleTxt, String simpleAction,
                                String multiAction, String raiseAction)
  {
    ImageProducer ip = getImageProducer(imagePath);
    ImageButton but = new DualModeStickyImageButton(
       m_currentToolBar.createImage(ip).
           getScaledInstance(m_currentToolBar.getButtonWidth(), 
                             m_currentToolBar.getButtonHeight(), 
                             Image.SCALE_AREA_AVERAGING),
           m_currentToolBar.getThickness(), 
           new DualModeStickyImageButtonListener());
    but.setBubbleHelp(bubbleTxt);
    but.addActionListener(new ComplexActionListener(m_parent, simpleAction, 
                                                    multiAction, raiseAction));
    m_currentToolBar.add(but, label);
  }

  private void createSimpleButton(String type, String label,
                             String imagePath, String bubbleTxt, 
                             String actionFn) {
    ImageButtonListener listener;
    if  (type.equals("STICKYBUTTON")) {
      listener = new StickyImageButtonListener();
    } else {
      listener = new SpringyImageButtonListener();
    }
    ImageProducer ip = getImageProducer(imagePath);

    ImageButton but = new ImageButton(
        m_currentToolBar.createImage(ip).
            getScaledInstance(m_currentToolBar.getButtonWidth(),
                              m_currentToolBar.getButtonHeight(),
                              Image.SCALE_AREA_AVERAGING), 
            m_currentToolBar.getThickness(),  
            listener);

    but.setBubbleHelp(bubbleTxt);
    but.addActionListener(new SimpleActionListener(m_parent, actionFn));

    m_currentToolBar.add(but, label);
  }
  public void createSpacer(int size) {
    m_currentToolBar.addSpacer(size);
  }

  private boolean isKeyWord(String word) {
    return (word.equals("BEGINTOOLBAR") ||
            word.equals("ENDTOOLBAR") ||
            word.equals("STICKYBUTTON")||
            word.equals("SPRINGYBUTTON")||
            word.equals("DUALBUTTON")||
            word.equals("VERTICAL") ||
            word.equals("HORIZONTAL") ||
            word.equals("SPACER"));
  }

  private class SimpleActionListener implements ActionListener {
    private Object m_parent;
    private String m_action;

    public SimpleActionListener(Object parent, String action) {
      m_parent = parent;
      m_action = action;
    }
    public void actionPerformed(ActionEvent ae) { 
      ToolBarMaker.doAction(m_parent, m_action);
    }
  }

  private class ComplexActionListener extends DualModeActionListener {
    private Object m_parent;
    private String m_singleAction;
    private String m_multiAction;
    private String m_raiseAction;
    
    public ComplexActionListener(Object parent, String singleAction,
                                 String multiAction, String raiseAction) {
      m_parent = parent;
      m_singleAction = singleAction;
      m_multiAction = multiAction;
      m_raiseAction = raiseAction;
    }
    public void doSingleModeActivated() {
      doAction(m_parent, m_singleAction);
    }
    public void doMultiModeActivated() {
      doAction(m_parent, m_multiAction);
    }
    public void doRaised() {
      doAction(m_parent, m_raiseAction);
    }
  }
  static void doAction(Object objToInvoke, String action) {    
    try {
	    Method handler = objToInvoke.getClass().getMethod(action, null);
	    handler.invoke(objToInvoke, null);
	  } catch(NoSuchMethodException x) {
	      System.out.println(x.getMessage() + ":" + action);
	  } catch(Exception x) {
	      System.out.println(x.getMessage() + ":" + action);
	  }
  }


}
