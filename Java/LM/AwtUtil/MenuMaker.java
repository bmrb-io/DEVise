/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class MenuMaker.java
****************************************************************************/

//Packages 

package AwtUtil;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;
import java.lang.reflect.*;
import COM.objectspace.jgl.*;

// Class
/**
 * The <CODE>MenuMaker</CODE> class
 *
 * @author  Shilpa Lawande
 */
 /*
class menutest {
  test() { }
  public void OnFileNew() { System.err.println("FileNew");}
  public void OnFileOpen() { System.err.println("FileOpen"); }
  public void OpenMe(CheckboxMenuItem cbmi) { 
    System.err.println("OpenMe: " + cbmi.getState()); 
  }
  public void OnFileClose() { System.err.println("FileClose");} 
  public void OnViewNew() { System.err.println("ViewNew");}
  public void OnViewDelete() { System.err.println("ViewDelete");} 
  public void DeleteMe(CheckboxMenuItem cbmi) { 
    System.err.println("DeleteMe: " + cbmi.getState()); 
  }
}
class MouseHandler extends MouseAdapter {
  PopupMenu m_popup;
  public MouseHandler(PopupMenu popup) {
    m_popup = popup;
  }
  public void mousePressed(MouseEvent e) {
    if (e.isAltDown()) {
      m_popup.show((Component)e.getSource(), e.getX(), e.getY());
    }
  }
}
*/
public class MenuMaker implements ActionListener, ItemListener
{
  private static final int PARSE_OK = 0;
  private static final int PARSE_FAILURE = -1;
  private static final int PARSE_PROCEED = -2;
  private static final int PARSE_EOF = -3;

  /* // Test code
  public static void main(String args[]) {
    MenuMaker mk = new MenuMaker(new menutest(), "sample.res");

    //System.err.println("Created menus...");
    Frame f = new Frame("Test");
    f.setSize(500,500);
    ScrollPane pane = new ScrollPane();
    pane.setSize(100,100);
    MenuBar mb = mk.createMenuBar(null);
    f.setMenuBar(mb);
    f.add(pane, "Center");
    
    Panel p = new Panel();
    p.setSize(500, 500);
    pane.add(p);

    PopupMenu popup = mk.getPopupMenuByName("PopupTest");
    p.add(popup);
    p.addMouseListener(new MouseHandler(popup)); 

    f.pack();
    f.show();
  }
  */

  /** Grammar is as follows (BNF)
    ResourceFile :- MenuList
                    MenubarList
   MenuList :- Menu 
              | Menu MenuList 
    Menu :- BEGINMENU(MenuName) 
            MenuItemList
            ENDMENU |
            BEGINPOPUPMENU(MenuName)
            MenuItemList
            ENDMENU
    MenuItemList :- MenuItem 
                  | MenuItem MenuItemList
    MenuItem :- Menu 
              | MENUITEM name [functionname] 
              | CHECKMENUITEM name [functionname]
              | SEPARATOR

    MenubarList :- Menubar 
                 | Menubar MenubarList 
    Menubar :- BEGINMENUBAR 
               MenuNameList 
               ENDMENUBAR

    MenuNameList :- MenuNameItem
              | MenuNameItem MenuNameList 

    MenuNameItem :- MENU MenuName 
    MenuName :- any Java String 
   */

  /* Public members */
  public String m_menuFile;  // an input file specifying how Menu appears
  public Object m_parent;    // an instance of  a class
                             // that implements the menu commands
  private DList m_menus = new DList();  // creates menus specified in the file
  private DList m_popupMenus = new DList(); // creates popup menus
  private DList m_menuBarList = new DList(); // creates menubars from menus in
                                             // file
  Menu m_currentMenu;        // current menu being created
  Stack m_menuStack;         // stack of menus for creating cascaded menus

  private int m_state;
  
  public MenuMaker(Object parent, String menuFile) {
    m_parent = parent;
    m_menuFile = menuFile;
    m_menuStack = new Stack();
    if (!readFileAndParse()) {
      return;
    }
  }

  public MenuBar createMenuBar(String labels[]) {
    MenuBar mb = new MenuBar();
    createMenuBar(mb, labels);
    return mb;
  }

  public void createMenuBar(MenuBar mb, String labels[]) {
    if (labels == null) {  // use all menus
      for (DListIterator d = m_menus.begin(); d.hasMoreElements(); ) {
        Menu m = (Menu)d.nextElement();
        mb.add(m);
      } 
    } else {
      for (int i = 0; i < labels.length; i++) {
        for (DListIterator d = m_menus.begin(); d.hasMoreElements(); ) {
          Menu m = (Menu)d.nextElement();
          if (m.getLabel().equals(labels[i])) {
            mb.add(m);
            break;
          }
        }
      }
    }
  }

  public Menu getMenu(int index) {
    return (Menu)m_menus.at(index);
  }
  public PopupMenu getPopupMenu(int index) {
    return (PopupMenu)m_popupMenus.at(index);
  }
  public PopupMenu getPopupMenuByName(String name) {
    for (DListIterator d = m_popupMenus.begin(); d.hasMoreElements(); ) {
      PopupMenu m = (PopupMenu)d.nextElement();
      if (m.getLabel().equals(name)) {
        return m; 
      }
    } 
    return null;
  }
  public void actionPerformed(ActionEvent e) {
    String command = e.getActionCommand(); 
    try {
      Method handler = m_parent.getClass().getMethod(command, null);
      handler.invoke(m_parent, null);
    } catch(NoSuchMethodException x) {
        System.err.println(x + ":" + command);
    } catch(IllegalAccessException x) {
        System.err.println(x + ":" + command);
    } catch(InvocationTargetException x) {
        System.err.println(x.getTargetException() + ":" + command);
    }
  }
  public void itemStateChanged(ItemEvent e) {
    CheckboxMenuItem cbmi = (CheckboxMenuItem) e.getItemSelectable();
    String command = cbmi.getActionCommand();
    try {
      Method handler = m_parent.getClass().getMethod(command, new Class[] { cbmi.getClass()});
      handler.invoke(m_parent, new Object[] { cbmi });
    } catch(NoSuchMethodException x) {
        System.err.println(x.getMessage() + ":" + command);
    } catch(Exception x) {
        System.err.println(x.getMessage() + ":" + command);
    }
  }

  // read the menu file and create the menus and toolbars
  private boolean readFileAndParse() {
     // open the file    
    FileReader f;
    try {
      f = new FileReader(m_menuFile);
    }
    catch(FileNotFoundException e) {
      System.err.println("File Not Found" + e.getMessage()); 
      return false;
    }
    StreamTokenizer s = new StreamTokenizer(f);
    s.eolIsSignificant(true);
    if (parse(s) != PARSE_OK) {
      return false;
    }
    return true;
  }

  String nextToken(StreamTokenizer s) {
    int token;
    while (true) {
	    try {
	      token = s.nextToken();
	    } catch (IOException e) {
	      System.err.println(e.getMessage());
	      return null;
	    }
	    if (s.ttype == StreamTokenizer.TT_EOF) {
	      return null;
	    } else if (s.ttype == StreamTokenizer.TT_WORD) {
	      //System.err.println("Next token -> " + s.sval);
	      return s.sval;
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
    //System.err.println("Parse");
    return parseMenuList(s);
  }
  int parseMenuList(StreamTokenizer s) {
    //System.err.println("ParseMenuList");
    while(true) {
      int status = parseMenu(s, null);
      if (status == PARSE_OK) {
        continue;
      } else if (status == PARSE_PROCEED) {
        return PARSE_OK;
      } else {
        return PARSE_FAILURE;
      }
    }
  }
  int parseMenu(StreamTokenizer s, Menu parent) {
    //System.err.println("ParseMenu");
    boolean popup;
    String menuType = nextToken(s);
    if (menuType == null) {
      return PARSE_EOF;
    }
    if (menuType.equals("BEGINMENU")) {
      popup = false; 
    } else if (menuType.equals("BEGINPOPUPMENU")) {
      popup = true;
    } else {
	    s.pushBack();
	    return PARSE_PROCEED;
	  }
    String name = nextToken(s);
    if (isKeyWord(name)) {
      return PARSE_FAILURE;
    }
    m_currentMenu = createMenu(parent, name, popup);
		if (parseMenuItemList(s) != PARSE_OK) {
       return PARSE_FAILURE; 
    }
	  if (matchToken(s,"ENDMENU") != PARSE_OK) {
	    System.err.println("Parse Error: ENDMENU expected");
	    return PARSE_FAILURE;
	  }
    if (m_menuStack.empty()) {
      if (popup) {
        m_popupMenus.add(m_currentMenu);
      } else {
	      m_menus.add(m_currentMenu);
      }
    }
	  return PARSE_OK;
	}
  int parseMenuItemList(StreamTokenizer s) {
    //System.err.println("ParseMenuItemList");
	  while(true) {
	    int status = parseMenuItem(s);
	    if (status == PARSE_OK) {
	      continue;
	    } else if (status == PARSE_PROCEED) {
	      return PARSE_OK;
	    } else {
	      return PARSE_FAILURE;
	    }
	  }
	}
  int parseMenuItem(StreamTokenizer s) {
    //System.err.println("ParseMenuItem");
	  String t1 = nextToken(s);
    if (t1 == null) {
      return PARSE_EOF;
    }
	  if (t1.equals("BEGINMENU")) {
	    s.pushBack();
      if (m_currentMenu  instanceof PopupMenu) {
        System.err.println("Cannot have normal menu as item in a PopupMenu");
        return PARSE_FAILURE;
      }
	    m_menuStack.push(m_currentMenu);
	    if (parseMenu(s, m_currentMenu) != PARSE_OK) {
	      return PARSE_FAILURE;
	    }
	    m_currentMenu = (Menu)m_menuStack.pop();
    } else if (t1.equals("BEGINPOPUPMENU")) {
      s.pushBack();
      if (!(m_currentMenu instanceof PopupMenu)) {
        System.err.println("Cannot have popup menu as item in a Menu");
        return PARSE_FAILURE;
      }
      m_menuStack.push(m_currentMenu);
      if (parseMenu(s, m_currentMenu) != PARSE_OK) {
        return PARSE_FAILURE;
      }
      m_currentMenu = (Menu)m_menuStack.pop();
	  } else if (t1.equals("ENDMENU")) {
	    s.pushBack();
	    return PARSE_PROCEED;
	  } else if (t1.equals("MENUITEM") || t1.equals("CHECKMENUITEM")) {
	    String name = nextToken(s);
      if (name == null) {
        return PARSE_EOF;
      }
	    if (isKeyWord(name)) {
	      if (t1.equals("MENUITEM") && name.equals("SEPARATOR")) {
	        m_currentMenu.addSeparator();
	        return PARSE_OK;
	      } else {
	        System.err.println("Parse Error: MENUITEM/CHECKMENUITEM name expected");
	        return PARSE_FAILURE;
	      }
	    } else {
	      String fn = nextToken(s);
        if (fn == null) {
          return PARSE_EOF;
        }
	      if (isKeyWord(fn)) {
          createMenuItem(m_currentMenu, t1, name, null);
	        s.pushBack();
	      } else {
          createMenuItem(m_currentMenu, t1, name, fn);
	      }
	      return PARSE_OK;
	    }
	  }
    return PARSE_OK;
	}
	
  private Menu createMenu(Menu parent, String label, boolean popup) {
    String prefix = "";
    if (parent != null) {
      prefix = parent.getActionCommand(); 
    }
    Menu m;
    if (popup) {
      m = new PopupMenu(label);
    } else {
      m = new Menu(label);
    }
    m.setActionCommand(prefix + label);
    if (parent != null) {
      parent.add(m);
    }
    return m;
  }

  private void createMenuItem(Menu parent, String itemType, 
                              String label, String function) 
  { 
    String prefix = parent.getActionCommand();
    MenuItem mi;
    if (itemType.equals("MENUITEM")) {
      mi = new MenuItem(label);
      mi.addActionListener(this);
    } else {
      mi = new CheckboxMenuItem(label);
      ((CheckboxMenuItem)mi).addItemListener(this);
    }
    if (function == null) {
      mi.setActionCommand("On" + prefix + label);
    } else {
      mi.setActionCommand(function);
    }
    parent.add(mi);
  }

  private boolean isKeyWord(String word) {
    return (word.equals("BEGINMENUBAR") ||
            word.equals("ENDMENUBAR") ||
            word.equals("BEGINMENU") ||
            word.equals("BEGINPOPUPMENU") ||
            word.equals("ENDMENU") ||
            word.equals("MENU") ||
            word.equals("MENUITEM")||
            word.equals("CHECKMENUITEM") ||
            word.equals("SEPARATOR"));
  }
}
