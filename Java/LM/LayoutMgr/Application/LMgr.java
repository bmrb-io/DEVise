/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsn-Madison.
Author : Shilpa Lawande
class LMgr.java
****************************************************************************/

//Packages 

package LayoutMgr.Application;

import java.lang.*;
import java.lang.reflect.*;
import java.io.*;
import java.awt.*;
import java.awt.event.*;
import COM.objectspace.jgl.*;
import AwtUtil.*;
import LayoutEditor.*;
import LayoutMgr.MetaData.*;

// Class
/**
 * The <CODE>LMgr</CODE> class
 *
 * @author  Shilpa Lawande
 */

// The Application
public class LMgr extends Frame 
{
  public static final String VERSION = "1.0";
  public static final String SESSION_DIR = "/u/s/s/ssl/Work/LM/Session/";
  public static final String SCHEMA_DIR ="/u/s/s/ssl/Work/LM/Schemas/";
  public static final String CATALOG_FILE ="/u/s/s/ssl/Work/LM/Catalog";

  public static double MINSCALEX = 0.25;
  public static double MINSCALEY = 0.25;

  private static int thedisplayWidth, thedisplayHeight;
  private static double globalScaleX, globalScaleY;

  public static int getDisplayWidth() { return thedisplayWidth; }
  public static int getDisplayHeight() { return thedisplayHeight; }
  public static double getGlobalScaleX() { return globalScaleX; }
  public static double getGlobalScaleY() { return globalScaleY; }

  public static final int MODE_DISPLAY = 0;
  public static final int MODE_LAYOUT = 1;
  
  public boolean m_showVisualLinks;
  public boolean m_showRecordLinks;
  public boolean m_showCursors;
  public boolean m_showMappings;
  public boolean m_showTDatas;

  static int numInstances = 0;

  DisplayPanel m_display;
  EditorPanel m_editor;
  SideCardPanel m_sidePanel;

  MenuMaker m_menuMaker;  // stuff that creates toolbars, menus etc
  MenuBar m_menuBar;      // menu bar
  
  // other info
  private int m_mode;  // current mode - Link, TData, Display
  DEViseSessionPeer m_thisSession; // the current session

  public static void main(String[] args)
  {
    new LMgr(1078, 768, 0.4, 0.4);
  }
  
  /* Constructor */
  public LMgr(int displayWidth, int displayHeight,
              double xscale, double yscale) {
    super();
    setLayout(new BorderLayout());
    thedisplayWidth = displayWidth;
    thedisplayHeight = displayHeight;
    globalScaleX = xscale;
    globalScaleY = yscale;
    
    int scaledDispWidth = (int)(thedisplayWidth * globalScaleX);
    int scaledDispHeight = (int)(thedisplayHeight * globalScaleY);

    m_display = new DisplayPanel(this, scaledDispWidth, scaledDispHeight);
    m_editor = new EditorPanel(this, scaledDispWidth, scaledDispHeight);
    m_sidePanel = new SideCardPanel(this, 225, scaledDispHeight);

    m_menuBar = new MenuBar();
    setMenuBar(m_menuBar);
    m_menuMaker = new MenuMaker(this, "LMgr.menus");

    changeMode(MODE_DISPLAY);

    OnSessionNew();
    
    numInstances++;
    setSize(getPreferredSize());

    m_showVisualLinks = 
    m_showRecordLinks = 
    m_showCursors = 
    m_showTDatas = 
    m_showMappings = true;
    show();
  }
  
  public Dimension getPreferredSize() {
    return new Dimension(700, 400);
  }

  public void createMenuBar(MenuBar menuBar) {
    int count = menuBar.getMenuCount();
    for (int i = 0; i < count; i++) {
      menuBar.remove(0);
    }
    m_menuMaker.createMenuBar(menuBar, null);
  }
  public Panel displayPanel() {
    return m_display.displayPanel();
  }
  public VisualLinkPanel visualLinkPanel() {
    return m_sidePanel.visualLinkPanel();
  }
  public RecordLinkPanel recordLinkPanel() {
    return m_sidePanel.recordLinkPanel();
  }
  public CursorPanel cursorPanel() {
    return m_sidePanel.cursorPanel();
  }
  public MappingPanel mappingPanel() {
    return m_sidePanel.mappingPanel();
  }
  public TDataPanel tdataPanel() {
    return m_sidePanel.tdataPanel(); 
  }

  // Menu commands 
  public void OnNew() {
    new  LMgr(thedisplayWidth, thedisplayHeight, globalScaleX, globalScaleY);
  }
  
  public void OnClose() {
    if (--numInstances == 0) {
      System.exit(0);
    } else {
      LMgr.this.dispose();
    } 
  }
  public void OnExit() {
    System.exit(0);
  }
  public void OnSessionNew() {  
    // Save old session
    if (m_thisSession != null) {
      if (!OnSessionClose()) {
        return;
      }
    }
    try {
      m_thisSession = new DEViseSessionPeer(this);
    } catch(Exception e) {
      System.out.println("OnSessionNew " + e);
    }
    Runtime.getRuntime().traceMethodCalls(false);
    updateUI(this);
  }
  public void OnSessionOpen() {
    if (m_thisSession != null) {
      if (!OnSessionClose()) {
        return;
      }
    }
    FileDialog dlg = new FileDialog(new Frame(), 
                                    "Open an existing session file", 
                                    FileDialog.LOAD);
    dlg.setDirectory(LMgr.SESSION_DIR);
    dlg.setModal(true);
    dlg.show();
    System.out.println("Selected file : " + dlg.getFile());
    String file = dlg.getDirectory() + dlg.getFile();
    if (file == null) {
      return;
    }
    try {
      m_thisSession = new DEViseSessionPeer(this, file);
    } catch (IOException e) {
      System.out.println(e);
      LMError.report("Error in opening session from file" + file);
      return;
    } catch (ClassNotFoundException e) {
      System.out.println(e);
      LMError.report("Error in opening session from file" + file);
      return;
    }
    updateUI(this);
  }
  public boolean OnSessionClose() {
    MessageDialog dlg 
      = new MessageDialog("Save current session ?", 
                          "Do you want to save the current session ?", 
                           MessageDialog.YESNOCANCEL, 0);
    int choice = dlg.doModal();
    if (choice == 0) {
      if (!OnSessionSave()) {
        return false;
      }
    } else if (choice == 2) {
      return false;
    }
    m_thisSession.selfDestruct();  // remove session from list of activei
                                   // sessions
    m_thisSession = null;
    displayPanel().repaint();
    updateUI(this);
    return true;
  }

  public boolean OnSessionSaveAs() {
    FileDialog dlg = new FileDialog(new Frame(), "Save session to a file", 
                                    FileDialog.SAVE);
    dlg.setDirectory(LMgr.SESSION_DIR);
    dlg.setModal(true);
    dlg.show();
    System.out.println("Selected file : " + dlg.getFile());
    String file = dlg.getDirectory() + dlg.getFile();
    if (file == null) {
      return false;
    }
    try {
      m_thisSession.save(file);
    } catch (IOException e) {
      LMError.report("Error in saving session to file" + file);
      return false;
    }
    updateUI(this);
    return true;
  }

  public boolean OnSessionSave() {
    String file = m_thisSession.getSessionFile();
    if (file == null) {
      return OnSessionSaveAs();
    }
    try {
      m_thisSession.save(file);
    } catch (IOException e) {
      LMError.report("Error in saving session to file" + file);
      return false;
    }
    updateUI(this);
    return true;
  }


  public void OnViewNew() {
    m_thisSession.OnViewNew();
  }
  public void OnViewDelete() {
    m_thisSession.OnViewDelete();
  }
  public void OnViewProperties() {
    m_thisSession.OnViewProperties();
   // m_display.repaint();
  }
  public void OnViewLayoutMode() {
    DEViseViewPeer currentView = m_thisSession.getCurrentView();
    if (currentView == null) {
      LMError.report("Please select a view by double clicking on it");
      return; 
    }
    LayoutModeDlg dlg = new LayoutModeDlg(currentView);
    m_thisSession.updateLayout(currentView);
    repaint();
  }
  public void OnLayoutCustom() {
    m_thisSession.OnLayoutCustom();
  }
  public void OnLayoutGrid() {
    m_thisSession.OnLayoutGrid();
  }
  public void OnLayoutEdit() {
    m_thisSession.OnLayoutEdit();
  }
  public void OnLayoutPile() {
    m_thisSession.OnLayoutPile();
  }
  public void OnLayoutUnpile() {
    m_thisSession.OnLayoutUnpile();
  }
  public void OnLayoutFlipForward() {
    m_thisSession.OnLayoutFlipForward();
  }
  public void OnLayoutFlipBackward() {
    m_thisSession.OnLayoutFlipBackward();
  }
  public void OnLayoutSwap() {
    m_thisSession.OnLayoutSwap();
  }
  public void changeMode(int newMode) {
    removeAll();
    if (newMode == MODE_DISPLAY) {
      setTitle("DEVise Layout Manager(Display)");
      createMenuBar(m_menuBar);
      add(m_sidePanel, "West");
      add(m_display, "Center");
    } else if (newMode == MODE_LAYOUT) {
      setTitle("DEVise Layout Manager(Edit Layout)");
      m_editor.createMenuBar(m_menuBar);
      m_editor.Init(getCurrentView());
      add(m_editor, "Center");
    }
    m_mode = newMode;
    validate();
  }

  public void OnLinkViews() {
    m_thisSession.OnLinkViews();
  }
  public void OnUnlinkViews() {
    m_thisSession.OnUnlinkViews();
  }
  public void OnVisualLinkShow(CheckboxMenuItem mi) {
    m_showVisualLinks = !m_showVisualLinks;
    displayPanel().repaint();
  }
  public void OnRecordLinkNew() {
    m_thisSession.OnRecordLinkNew();
  }
  public void OnRecordLinkDelete() {  
    m_thisSession.OnRecordLinkDelete();
  }
  public void OnRecordLinkSetMaster() {
    m_thisSession.OnRecordLinkSetMaster();
  }
  public void OnRecordLinkSetSlave() {
    m_thisSession.OnRecordLinkSetSlave();
  }
  public void OnRecordLinkUnsetMaster() {
    m_thisSession.OnRecordLinkUnsetMaster();
  }
  public void OnRecordLinkUnsetSlave() {
    m_thisSession.OnRecordLinkUnsetSlave();
  }
  public void OnRecordLinkShow(CheckboxMenuItem mi) {
    m_showRecordLinks = !m_showRecordLinks;
    displayPanel().repaint();
  }
  public void OnCursorNew() {
    m_thisSession.OnCursorNew();
  }
  public void OnCursorDelete() {
    m_thisSession.OnCursorDelete();
  }
  public void OnCursorSetSource() {
    m_thisSession.OnCursorSetSource();
  }
  public void OnCursorSetDestination() {
    m_thisSession.OnCursorSetDestination();
  }
  public void OnCursorShow(CheckboxMenuItem mi) {
    m_showCursors = !m_showCursors;
    displayPanel().repaint();
  }
  public void OnTDataNew() {
    m_thisSession.OnTDataNew();
  }
  public void OnMappingNew() {
    m_thisSession.OnMappingNew();
  }
  public void OnTDataDelete() {
    m_thisSession.OnTDataDelete();
  }
  public void OnMappingDelete() {
    m_thisSession.OnMappingDelete();
  }
  public void OnSetViewTData() {
    m_thisSession.OnSetViewTData();
  }
  public void OnSetViewMapping() {
    m_thisSession.OnSetViewMapping();
  }
  public void OnUnsetViewTData() {
    m_thisSession.OnUnsetViewTData();
  }
  public void OnUnsetViewMapping() {
    m_thisSession.OnUnsetViewMapping();
  }
  public void OnMappingShow(CheckboxMenuItem mi) {
    m_showMappings = !m_showMappings;
    displayPanel().repaint();
  }
  public void OnTDataShow(CheckboxMenuItem mi) {
    m_showTDatas = !m_showTDatas;
    displayPanel().repaint();
  }
  public void OnBrowseSchemas() {
    SchemaBrowser dlg = new SchemaBrowser();
  }
  public void OnBrowseCatalog() {
    CatalogBrowser dlg = new CatalogBrowser();
  }
  public boolean showVisualLinks() { return m_showVisualLinks; } 
  public boolean showRecordLinks() { return m_showRecordLinks; } 
  public boolean showCursors() { return m_showCursors; } 
  public boolean showMappings() { return m_showMappings; }
  public boolean showTDatas() { return m_showTDatas; }

  public DEViseViewPeer getCurrentView() {
    return m_thisSession.getCurrentView();
  }
  public DEViseSession getCurrentSession() {
    return m_thisSession.getCurrentSession();
  }
  public void update(Graphics g) {
    if (m_mode == MODE_DISPLAY) {
      updateUI(this);
    } else if (m_mode == MODE_LAYOUT) {
      updateUI(m_editor);
    }
    super.update(g);
  }

  // update state of the menu items depending on the current view
  // this is actually dirty but Java has no clean way to update
  // Menu item state locally.
  // Ideal way to do this would be to intercept whatever even the
  // menu item receives before posting the menu and let each menuitem
  // handle its state change...
  public void updateUI(Object uiObj) {
    System.out.println("updateUI");
    int menuc = m_menuBar.getMenuCount();
    for (int i = 0; i < menuc; i++) {
      Menu m = m_menuBar.getMenu(i);
      updateState(uiObj, m, m.getLabel());
      int itemc = m.getItemCount();
      for (int j = 0; j < itemc; j++) {
        MenuItem mi = m.getItem(j);
        updateState(uiObj, mi, m.getLabel()+mi.getLabel());
      }
    }
  }
  private void updateState(Object uiObj, MenuItem mi, String label) {
    try {
      uiObj.getClass().getMethod("On"+label+"UI", new Class[] { mi.getClass() }).invoke(uiObj, new Object[] { mi });
    } 
    catch (NoSuchMethodException e) {
    }
    catch (IllegalAccessException e) {
      System.out.println(e + label);
    } 
    catch (InvocationTargetException e) {
      System.out.println(e.getTargetException() + label);
      mi.setEnabled(false);
    }
  }
  public boolean checkCurrentSession(MenuItem m) {
    if (m_thisSession == null) {
      m.setEnabled(false);
      return false;
    }
    m.setEnabled(true);
    return true;
  }

  public void checkCurrentView(MenuItem m) {
    DEViseViewPeer currentView = m_thisSession.getCurrentView();
    if (currentView == null) {
      m.setEnabled(false);
    }else {
      m.setEnabled(true);
    }
  }
  public void OnViewNewUI(MenuItem m) { checkCurrentView(m);}
  public void OnViewDeleteUI(MenuItem m) { checkCurrentView(m);}
  public void OnViewPropertiesUI(MenuItem m) { checkCurrentView(m);}
  public void OnViewLayoutModeUI(MenuItem m) { checkCurrentView(m); }
  //public void OnLayoutFlipBackwardUI(MenuItem m) { checkCurrentView(m); }
  //public void OnLayoutFlipForwardUI(MenuItem m) { checkCurrentView(m); }
  //public void OnLayoutSwapUI(MenuItem m) { checkCurrentView(m); }

  public void OnLayoutEditUI(MenuItem m) {
    DEViseViewPeer currentView = m_thisSession.getCurrentView();
    if (currentView == null) {
      m.setEnabled(false);
      return;
    }
    DEViseLayout layout = currentView.getDEViseLayout(); 
    if (!(layout instanceof DEViseCustomLayout)) {
      m.setEnabled(false);
    } else {
      m.setEnabled(true);
    }
  }
  public void OnLayoutGridUI(MenuItem m) { 
    DEViseViewPeer currentView = m_thisSession.getCurrentView();
    if (currentView == null) {
      m.setEnabled(false);
      return;
    }
    DEViseLayout layout = currentView.getDEViseLayout();
    if (layout instanceof DEViseGridLayout) {
      m.setEnabled(false);
    } else {
      m.setEnabled(true);
    }
  }
  public void OnLayoutPileUI(MenuItem m) {
    DEViseViewPeer currentView = m_thisSession.getCurrentView();
    if (currentView == null) {
      m.setEnabled(false);
      return;
    }
    DEViseLayout layout = currentView.getDEViseLayout(); 
    if (!(layout instanceof DEVisePileLayout)) {
      m.setEnabled(true);
    } else {
      m.setEnabled(false);
    }
  }
  public void OnLayoutUnpileUI(MenuItem m) {
    DEViseViewPeer currentView = m_thisSession.getCurrentView();
    if (currentView == null) {
      m.setEnabled(false);
      return;
    }
    DEViseLayout layout = currentView.getDEViseLayout(); 
    if (layout instanceof DEVisePileLayout) {
      m.setEnabled(true);
    } else {
      m.setEnabled(false);
    }
  }

  public void OnViewUI(Menu m) { checkCurrentSession(m); }
  public void OnLayoutUI(Menu m) { checkCurrentSession(m); }

  public void OnRecordLinkUI(Menu m) {
    if (!checkCurrentSession(m)) {
      return;
    }
    if (m_sidePanel.isRecordLinkMode()) {
      m.setEnabled(true);
    } else {
      m.setEnabled(false);
    }
  }
  public void OnVisualLinkUI(Menu m) {
    if (!checkCurrentSession(m)) {
      return;
    }
    if (m_sidePanel.isVisualLinkMode()) {
      m.setEnabled(true);
    } else {
      m.setEnabled(false);
    }
  }
  public void OnCursorUI(Menu m) {
    if (!checkCurrentSession(m)) {
      return;
    }
    if (m_sidePanel.isCursorMode()) {
      m.setEnabled(true);
    } else {
      m.setEnabled(false);
    }
  }
  public void OnTDataUI(Menu m) {
    if (!checkCurrentSession(m)) {
      return;
    }
    if (m_sidePanel.isTDataMapMode()) {
      m.setEnabled(true);
    } else {
      m.setEnabled(false);
    }
  }
}
