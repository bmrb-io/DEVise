/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DEViseSessionPeer.java
****************************************************************************/

//Packages 

package LayoutMgr.Application;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.util.*;
import COM.objectspace.jgl.*;
import AwtUtil.*;
import Util.*;
import LayoutMgr.MetaData.*;

// Class
/**
 * The <CODE>DEViseSessionPeer</CODE> class
 *
 * @author  Shilpa Lawande
 */

// Maintains the state information of a session layout and also does
// what is needed to create and maintain peer classes for DEViseObjects.

public class DEViseSessionPeer 
{
  private DEViseSession m_session;
  private DEViseViewPeer m_Display;
  private DEViseViewPeer m_currentView;

  private LMgr m_theApp;  // the application of which this session is part
  String m_sessionFile; // file that the session was loaded from or saved to
  boolean m_hasChanged; // something has been modified since the file was
                        // opened
  ViewSelector m_selector; 
  Hashtable m_viewPeerTable;  // maintains a map of view name to peer
                              // for easy access
  Hashtable m_recordLinkPeerTable;  // maintains a map of record links and 
                                    // their peers
  Hashtable m_visualLinkPeerTable; // maintains a map of visual links and 
                                   // their peers
  Hashtable m_cursorPeerTable; // maintains a map of visual links and 
                               // their peers
  Hashtable m_tdataPeerTable;  // maintains a map of tdatas and their
                               // peers
  Hashtable m_mappingPeerTable; // maintains  a map of mappings and their
                                // peers

  /* Constructor */
  DEViseSessionPeer(LMgr theApp) throws IOException, ClassNotFoundException {
    this(theApp, null);
  }

  DEViseSessionPeer(LMgr theApp, String fileName) 
                    throws IOException, ClassNotFoundException {
    m_theApp = theApp;
    m_selector = new ViewSelector(m_theApp.displayPanel());
    m_selector.addMouseListener(new CurrentViewSelector());
    m_sessionFile = fileName;
    m_viewPeerTable = new Hashtable();
    m_visualLinkPeerTable = new Hashtable();
    m_recordLinkPeerTable = new Hashtable();
    m_cursorPeerTable = new Hashtable();
    m_tdataPeerTable = new Hashtable();
    m_mappingPeerTable = new Hashtable();

    if (fileName == null) {
      m_session = DEViseSession.createInstance();
      m_Display = createView("Display", null, 0, 0, 1078, 768,
                           DEViseColor.black, DEViseColor.white);
    } else {
      open(fileName); // initialize the session params from file
    }
    //System.out.println("m_session = " + m_session);
    System.out.println("m_Display = " + m_Display);
    m_theApp.displayPanel().add(m_Display);
    setCurrentView(m_Display);
    m_theApp.displayPanel().validate();
  }
  public void finalize() {
    System.out.println("GoodBye to ->\n" + this + "\n");
  }
  public void selfDestruct() {
    clearSidePanel();
    DEViseView display = m_Display.getDEViseView();
    deleteView(display, true);

    setCurrentView(null);
    DEViseSession.destroyInstance(m_session);
    m_Display = null;
    m_session = null;
  }

  DEViseViewPeer getCurrentView() { return m_currentView; }
  DEViseViewPeer getDisplay() { return m_Display; }
  DEViseSession getCurrentSession() { return m_session; }
  

  public void OnViewNew() {
    if (m_currentView == null) {
      LMError.report("No view is currently selected. Please select a view to insert the new view by double clicking on it");
      return;
    }
    if (m_currentView.getDEViseLayout() instanceof DEViseCustomLayout) {
      m_theApp.changeMode(LMgr.MODE_LAYOUT);
      return;
    }
    createView(m_currentView);
    updateLayout(m_currentView);
    m_currentView.repaint();
  }

  public void updateLayout(DEViseViewPeer view) {
    ((DEViseLayoutPeer)view.getLayout()).layoutContainer(view);
  }
  DEViseViewPeer createView(DEViseViewPeer parent) {
    return createView(m_session.myViews().createUniqueName(),
                      parent,
                      0, 0, 0.5, 0.5, 
                      new DEViseColor(Color.red),
                      new DEViseColor(Color.black));
  }
  DEViseViewPeer createView(String name, DEViseViewPeer parent, 
                            double x, double y, double wRatio, double hRatio,
                            DEViseColor fg, DEViseColor bg)
  {
    DEViseView parentView;
    if (parent != null) {
      parentView = parent.getDEViseView();
    } else {
      parentView = null;
    }
    DEViseView view = new DEViseView(name, parentView, x, y, wRatio, hRatio,
                                     fg, bg);
    m_session.myViews().add(view);
    DEViseGridLayout layout = new DEViseGridLayout();
    return createPeer(view, layout, parent);
  }

  DEViseViewPeer createPeer(DEViseView view, DEViseLayout layout, 
                            DEViseViewPeer parent) {
    DEViseViewPeer viewPeer = new DEViseViewPeer(this, view, layout);
    m_selector.enableSelectorInterface(viewPeer);
    //viewPeer.validate();
    if (parent != null) {
      parent.add(viewPeer);
    }
    m_viewPeerTable.put(view.getName(), viewPeer);
    return viewPeer;
  }

  public void OnViewDelete() {
    if (m_currentView == m_Display) {
      LMError.report("Cannot delete display");
      return;
    }
    DEViseView view = m_currentView.getDEViseView();
	  // for now disallow deleting views with children
	  if (!view.canBeDestroyed()) {
	    MessageDialog dlg = new MessageDialog("View Deletion Question?",
      "View -" + view.getName() + "-has child views/active links & cursors\n Do you want to disassociate the view from its children and links and destroy it ?", MessageDialog.YESNO, 0);
	    if (dlg.doModal() == 1) {
	      return;
	    }
	  }
    DEViseLayoutState lstate 
       = (DEViseLayoutState)((DEViseViewPeer)m_currentView.getParent()).getLayoutState();
    if (lstate != null) {
      lstate.markForDeletion(lstate.findPair(view));
      lstate.deleteMarkedEntries();
    } 
    setCurrentView((DEViseViewPeer)m_currentView.getParent());
    deleteView(view, false);
    System.out.println("LayoutState-> \n" + lstate);
    m_currentView.repaint();
    System.out.println(m_session);
  }

  public void deleteView(DEViseView view, boolean doRecursive) {
    if (doRecursive) {
      for (DListIterator d = view.childList(); d.hasMoreElements(); ) {
        deleteView((DEViseView)d.nextElement(), true);      
      }
      view.doRecursiveDelete(true);
    }
    DEViseViewPeer peer = findViewPeerByName(view.getName());
    m_viewPeerTable.remove(view.getName());
    java.awt.Container parent = peer.getParent();
    parent.remove(peer);
    m_selector.disableSelectorInterface(peer);
    peer.selfDestruct();
    m_session.myViews().destroy(view);
  }

  public void OnViewProperties() {
    if (m_currentView == m_Display) {
      LMError.report("Cannot edit display");
      return; 
    }
    System.out.println("DEViseSessionPeer : OnViewEdit " + m_currentView);
    EditDlg dlg = new EditDlg(m_currentView);
    dlg.doModal();
  }
  public void setCurrentView(DEViseViewPeer view) {
    /* not needed.. in fact screws up multiple selections
    if (m_currentView != null) {
      m_currentView.setState(Selectable.UNSELECTED);
    }
    */
    m_currentView = view;
    System.out.println("currentView = " + view);
    if (view != null) {
      if (view.getState() != Selectable.SELECTED) {
        view.setState(Selectable.SELECTED);
      }
    }
    m_theApp.updateUI(m_theApp);
  }
  
  public void OnLinkViews() {
    Object links[]= m_theApp.visualLinkPanel().getSelectedLinks();
    if (links == null) {
      LMError.report("No link type was selected");
      return;
    }
    DList leaves = getSelectedLeafViews();
    if (leaves.size() < 0) {
      return;
    }
    for (int i = 0; i < links.length; i++) {
      System.out.println("Selected link " + ((ImageItem)links[0]).getLabel());
      createLink(m_theApp.visualLinkPanel().getType(links[i]), leaves);
    }
    System.out.println(m_session);
    if (m_theApp.showVisualLinks()) {
      m_theApp.displayPanel().repaint();
    }
  }
  
  private void createLink(int type, DList views) {
    System.out.println("createLink ("  + type + ")");
    DList newViews = new DList();
    DEViseVisualLink newLink = null;
    for (DListIterator d = views.begin(); d.hasMoreElements(); ) {
      DEViseView view = (DEViseView)d.nextElement();
      DEViseVisualLink link = view.findVisualLink(type);
      if (link != null) {
        if (newLink == null) {
          newLink = link;
        } else {
          if (newLink != link) {
            newLink.expand(link);
            m_visualLinkPeerTable.remove(link.getName());
            m_session.myVisualLinks().delete(link);
          }
        }
      } else {
        newViews.add(view);
      }
    }

    System.out.println("newViews = " + newViews.size());
    if (newLink == null) {
      newLink 
        = new DEViseVisualLink(m_session.myVisualLinks().createUniqueName(), 
                               type);
      m_session.myVisualLinks().add(newLink);
      DEViseVisualLinkPeer peer = new DEViseVisualLinkPeer(this, newLink);
      m_visualLinkPeerTable.put(newLink.getName(), peer);
    }
    for (DListIterator d = newViews.begin(); d.hasMoreElements(); ) {
      System.out.println("Adding view : ");
      newLink.addView((DEViseView)d.nextElement());
    }
  }

  public void OnUnlinkViews() {
    Object links[]= m_theApp.visualLinkPanel().getSelectedLinks();
    if (links == null) {
      LMError.report("No link type was selected");
      return;
    }
    if (links.length != 1) {
      LMError.report("Select exactly one link");
      return;
    }
    DList leaves = getSelectedLeafViews();
    if (leaves.size() == 0) {
      return;
    }
    for (int i = 0; i < links.length; i++) {
      System.out.println("Selected link " + ((ImageItem)links[0]).getLabel());
      breakLink(m_theApp.visualLinkPanel().getType(links[i]), leaves);
    }
    System.out.println(m_session);
    if (m_theApp.showVisualLinks()) {
      m_theApp.displayPanel().repaint();
    }
  }

  private void breakLink(int type, DList views) {
    DList oldLinks = new DList();
    DList newLinks = new DList();
    System.out.println("breakLink (" + type + ")");
    for (DListIterator d = views.begin(); d.hasMoreElements(); ) {
      DEViseView view = (DEViseView)d.nextElement();
      DEViseVisualLink link = view.findVisualLink(type);
      if (link == null) {
        continue;
      }
      link.removeView(view);
      int index = oldLinks.indexOf(link);
      if (index != -1) {
        ((DEViseVisualLink)newLinks.at(index)).addView(view); 
      } else {
        oldLinks.add(link);
        DEViseVisualLink newLink
          = new DEViseVisualLink(m_session.myVisualLinks().createUniqueName(), type);
        m_session.myVisualLinks().add(newLink);
        DEViseVisualLinkPeer peer = new DEViseVisualLinkPeer(this, newLink);
        m_visualLinkPeerTable.put(newLink.getName(), peer);
        newLinks.add(newLink);
        newLink.addView(view);
      }
    }
  }

  public void OnRecordLinkNew() {
    // for now use default attribute (recId)
    DEViseRecordLink newLink 
      = new DEViseRecordLink(m_session.myRecordLinks().createUniqueName());
    m_session.myRecordLinks().add(newLink);
    DEViseRecordLinkPeer peer = new DEViseRecordLinkPeer(this, newLink);
    m_recordLinkPeerTable.put(newLink.getName(), peer);
    m_theApp.recordLinkPanel().addItem(peer); 
  }

  public void OnRecordLinkDelete() {  
    Object links[] = m_theApp.recordLinkPanel().getSelectedLinks();
    if (links == null) {
      LMError.report("No record link was selected");
      return;
    }
    for (int i = 0; i < links.length; i++) {
      DEViseRecordLink recLink = (DEViseRecordLink)links[i];
      System.out.println("Selected link : " + recLink);
      if (recLink.numViewsInLink() != 0) {
        MessageDialog dlg 
          = new MessageDialog("Link contains views", 
                              "The recordlink " + recLink.getName() + 
                              " contains views - Unlink views and delete link?",
                              MessageDialog.YESNO, 0);
        int choice = dlg.doModal();
        if (choice == 1) {
          continue;
        }
      }
      m_theApp.recordLinkPanel().delItem(recLink);
      m_recordLinkPeerTable.remove(recLink.getName());
      m_session.myRecordLinks().destroy(recLink);
    }
    if (m_theApp.showRecordLinks()) {
      m_theApp.displayPanel().repaint();
    }
  }

  public void OnRecordLinkSetMaster() {
    Object links[] = m_theApp.recordLinkPanel().getSelectedLinks();
    if (links == null){
      LMError.report("No record link was selected");
      return;
    }
    DList leaves = getSelectedLeafViews();
    if (leaves.size() == 0) {
      return;
    }
    if (leaves.size() != 1) {
      LMError.report("Select exactly one view to be master");
      return;
    }
    for (int i = 0; i < links.length; i++) {
      DEViseRecordLink recLink = (DEViseRecordLink)links[i]; 
      DEViseView master = (DEViseView)leaves.at(0);
      recLink.setMaster(master);
      System.out.println(recLink);
    }
    if (m_theApp.showRecordLinks()) {
      m_theApp.displayPanel().repaint();
    }
  }

  public void OnRecordLinkSetSlave() {
    Object links[] = m_theApp.recordLinkPanel().getSelectedLinks();
    if (links == null){
      LMError.report("No record link was selected");
      return;
    }
    DList views = getSelectedLeafViews();
    if (views.size() == 0) {
      return;
    }
    for (int i = 0; i < links.length; i++) {
      DEViseRecordLink recLink = (DEViseRecordLink)links[i]; 
      for (DListIterator d = views.begin(); d.hasMoreElements(); ) {
        DEViseView slave = (DEViseView)d.nextElement();
        recLink.addSlave(slave);
      }
      System.out.println(recLink);
    }
    if (m_theApp.showRecordLinks()) {
      m_theApp.displayPanel().repaint();
    }
  }

  public void OnRecordLinkUnsetMaster() {
    Object links[] = m_theApp.recordLinkPanel().getSelectedLinks();
    if (links == null){
      LMError.report("No record link was selected");
      return;
    }
    for (int i = 0; i < links.length; i++) {
      DEViseRecordLink recLink = (DEViseRecordLink)links[i]; 
      recLink.unsetMaster();
      System.out.println(recLink);
    }
    if (m_theApp.showRecordLinks()) {
      m_theApp.displayPanel().repaint();
    }
  }

  public void OnRecordLinkUnsetSlave() {
    Object links[] = m_theApp.recordLinkPanel().getSelectedLinks();
    if (links == null){
      LMError.report("No record link was selected");
      return;
    }
    DList views = getSelectedLeafViews();
    if (views == null) {
      LMError.report("No views were selected");
      return;
    }
    for (int i = 0; i < links.length; i++) {
      DEViseRecordLink recLink = (DEViseRecordLink)links[i]; 
      for (DListIterator d = views.begin(); d.hasMoreElements(); ) {
        DEViseView slave = (DEViseView)d.nextElement();
        recLink.removeSlave(slave);
        System.out.println(recLink);
      }
    }
    if (m_theApp.showRecordLinks()) {
      m_theApp.displayPanel().repaint();
    }
  }

  public void OnCursorNew() {
    DEViseCursor cursor
      = new DEViseCursor(m_session.myCursors().createUniqueName());
    DEViseCursorPeer peer = new DEViseCursorPeer(this, cursor);
    m_session.myCursors().add(cursor);
    m_cursorPeerTable.put(cursor.getName(), peer);
    m_theApp.cursorPanel().addItem(peer);
  }
  
  public void OnCursorDelete() {
    Object cursors[] = m_theApp.cursorPanel().getSelectedCursors();
    if (cursors == null) {
      LMError.report("No cursors were selected");
      return;
    }
    for (int i = 0; i < cursors.length; i++) {
      DEViseCursor cursor = (DEViseCursor)cursors[i];
      System.out.println("Selected cursor : " + cursor);
      m_cursorPeerTable.remove(cursor.getName());
      m_theApp.cursorPanel().delItem(cursor);
      m_session.myCursors().destroy(cursor);
    }
  }

  public void OnCursorSetSource() {
    Object cursors[] = m_theApp.cursorPanel().getSelectedCursors();
    if (cursors == null) {
      LMError.report("No cursor was selected");
    }
    DList leaves = getSelectedLeafViews();
    if (leaves.size() == 0) {
      return;
    }
    if (leaves.size() != 1) {
      LMError.report("Select exactly one view to be source");
      return;
    }
    for (int i = 0; i < cursors.length; i++) {
      DEViseCursor cursor = (DEViseCursor)cursors[i];
      DEViseView source = (DEViseView)leaves.at(0);
      cursor.setSrc(source);
      System.out.println(cursor);
    }
    if (m_theApp.showCursors()) {
      m_theApp.displayPanel().repaint();
    }
  }

  public void OnCursorSetDestination() {
    Object cursors[] = m_theApp.cursorPanel().getSelectedCursors();
    if (cursors == null) {
      LMError.report("No cursor was selected");
    }
    DList leaves = getSelectedLeafViews();
    if (leaves.size() == 0) {
      return;
    }
    if (leaves.size() != 1) {
      LMError.report("Select exactly one view to be destination");
      return;
    }
    for (int i = 0; i < cursors.length; i++) {
      DEViseCursor cursor = (DEViseCursor)cursors[i];
      DEViseView destination = (DEViseView)leaves.at(0);
      cursor.setDst(destination);
      System.out.println(cursor);
    }
    if (m_theApp.showCursors()) {
      m_theApp.displayPanel().repaint();
    }
  }
  public void OnTDataNew() {
    CatalogBrowser dlg = new CatalogBrowser();
    String tdataName = dlg.getSelection();
    try {
      DummyCatalog.DummyCatalogEntry entry 
        = DummyCatalog.getCatalog().findTable(tdataName);
      if (m_session.myTDatas().find(tdataName) != null) {
        LMError.report("Table already active");
        return;
      }
      DEViseSchema schema 
        = (DEViseSchema)m_session.mySchemas().find(entry.getSchemaName());
      if (schema == null) {
        schema = new DEViseSchema(LMgr.SCHEMA_DIR + entry.getSchemaName());
        System.out.println("!!!!!!!!!!!!!!NEW SCHEMA!!!!!!!!!!!!!!!!");
        m_session.mySchemas().add(schema);
      } 
      DEViseTData tdata 
        = new DEViseTData(tdataName, schema, entry.getDataFileName());
      DEViseTDataPeer peer
        = new DEViseTDataPeer(this, tdata);
      m_session.myTDatas().add(tdata);
      m_tdataPeerTable.put(tdataName, peer);
      m_theApp.tdataPanel().addItem(peer);
    } catch (DummyCatalog.DummyCatalogException e) {
      LMError.report("Error in reading catalog" + e.getMessage());
    }
  }
  public void OnMappingNew() {
    SchemaBrowser dlg = new SchemaBrowser();
    String schemaName = dlg.getSelection();
    DEViseSchema schema = (DEViseSchema) m_session.mySchemas().find(schemaName);
    if (schema == null) {
      schema = new DEViseSchema(LMgr.SCHEMA_DIR + schemaName);
      System.out.println("!!!!!!!!!!!!!!NEW SCHEMA!!!!!!!!!!!!!!!!");
      m_session.mySchemas().add(schema);
    }
    MappingDlg mapdlg = new MappingDlg(schema);
    if (mapdlg.doModal() == 1) {
      return;
    } 
    DEViseMapping mapping = mapdlg.getMapping();
    DEViseMappingPeer peer = new DEViseMappingPeer(this, mapping);
    m_session.myMappings().add(mapping);
    m_mappingPeerTable.put(mapping.getName(), peer);
    m_theApp.mappingPanel().addItem(peer);
  }

  public void OnTDataDelete() {
    Object tdatas[] = m_theApp.tdataPanel().getSelectedTDatas();
    if (tdatas == null) {
      LMError.report("No tdatas were selected");
      return;
    }
    for (int i = 0; i < tdatas.length; i++) {
      DEViseTData tdata = (DEViseTData)tdatas[i];
      m_tdataPeerTable.remove(tdata.getName());
      m_theApp.tdataPanel().delItem(tdata);
      m_session.myTDatas().destroy(tdata);
    }
  }
  public void OnMappingDelete() {
    Object mappings[] = m_theApp.mappingPanel().getSelectedMappings();
    if (mappings == null) {
      LMError.report("No mappings were selected");
      return;
    }
    for (int i = 0; i < mappings.length; i++) {
      DEViseMapping mapping = (DEViseMapping)mappings[i];
      m_mappingPeerTable.remove(mapping.getName());
      m_theApp.mappingPanel().delItem(mapping);
      m_session.myMappings().destroy(mapping);
    }
  }

  public void OnSetViewTData() {
    Object tdatas[] = m_theApp.tdataPanel().getSelectedTDatas();
    if (tdatas == null){
      LMError.report("No tdata was selected");
      return;
    }
    if (tdatas.length != 1) {
      LMError.report("Select exactly one tdata");
    }
    DList views = getSelectedLeafViews();
    if (views.size() == 0) {
      return;
    }
    for (DListIterator d = views.begin(); d.hasMoreElements(); ) {
      DEViseView view = (DEViseView)d.nextElement();
      if (!view.setTData((DEViseTData)tdatas[0])) {
        LMError.report("Could not set tdata for " + view.getName() + " : incompatible with mapping");
      }
    }
    if (m_theApp.showTDatas()) {
      m_theApp.displayPanel().repaint();
    }
  }

  public void OnSetViewMapping() {
    Object mappings[] = m_theApp.mappingPanel().getSelectedMappings();
    if (mappings == null){
      LMError.report("No mapping was selected");
      return;
    }
    if (mappings.length != 1) {
      LMError.report("Select exactly one mapping");
      return;
    }
    DList views = getSelectedLeafViews();
    if (views.size() == 0) {
      return;
    }
    for (DListIterator d = views.begin(); d.hasMoreElements(); ) {
      DEViseView view = (DEViseView)d.nextElement();
      if (!view.setMapping((DEViseMapping)mappings[0])) {
        LMError.report("Could not set mapping for " + view.getName() + " : incompatible with tdata");
      }
    }
    if (m_theApp.showMappings()) {
      m_theApp.displayPanel().repaint();
    }
  }

  public void OnUnsetViewTData() {
    DList views = getSelectedLeafViews();
    for (DListIterator d = views.begin(); d.hasMoreElements(); ) {
      DEViseView view = (DEViseView)d.nextElement();
      view.unsetTData();
    }
    if (m_theApp.showTDatas()) {
      m_theApp.displayPanel().repaint();
    }
  }
  public void OnUnsetViewMapping() {
    DList views = getSelectedLeafViews();
    for (DListIterator d = views.begin(); d.hasMoreElements(); ) {
      DEViseView view = (DEViseView)d.nextElement();
      view.unsetMapping();
    }
    if (m_theApp.showMappings()) {
      m_theApp.displayPanel().repaint();
    }
  }
   
  public void OnLayoutEdit() {
    if (m_currentView == null) {
      LMError.report("Please select a view by double clicking on it");
      return;
    }
    m_theApp.changeMode(LMgr.MODE_LAYOUT);
  }

  public void OnLayoutPile() {
    DEVisePileLayout layout 
      = new DEVisePileLayout(m_currentView.getDEViseLayout());
    m_currentView.setDEViseLayout(layout);
    updateLayout(m_currentView);
    m_theApp.displayPanel().repaint();
    m_theApp.updateUI(m_theApp);
  }
  public void OnLayoutGrid() {
    DEViseGridLayout layout = new DEViseGridLayout();
    m_currentView.setDEViseLayout(layout);
    updateLayout(m_currentView);
    m_theApp.displayPanel().repaint();
    m_theApp.updateUI(m_theApp);
  }
  public void OnLayoutCustom() {
    DEViseCustomLayout layout = new DEViseCustomLayout();
    m_currentView.setDEViseLayout(layout);
    updateLayout(m_currentView);
    m_theApp.displayPanel().repaint();
    m_theApp.updateUI(m_theApp);
  }
  public void OnLayoutUnpile() {
    DEVisePileLayout layout = (DEVisePileLayout)m_currentView.getDEViseLayout(); 
    DEViseLayout origLayout = layout.getOrigLayout();
    if (origLayout instanceof DEViseCustomLayout) {
      m_currentView.setDEViseLayout(new DEViseGridLayout());
    } else {
      m_currentView.setDEViseLayout(origLayout);
    }
    updateLayout(m_currentView);
    m_theApp.displayPanel().repaint();
    m_theApp.updateUI(m_theApp);
  }

  public void OnLayoutFlipForward() {
    if (m_currentView == null) {
      LMError.report("No current view selected");
      return;
    }
    m_currentView.flipForward();
    updateLayout(m_currentView);
    m_theApp.displayPanel().repaint();
  }

  public void OnLayoutFlipBackward() {
    if (m_currentView == null) {
      LMError.report("No current view selected");
      return;
    }
    m_currentView.flipBackward();
    updateLayout(m_currentView);
    m_theApp.displayPanel().repaint();
  }

  public void OnLayoutSwap() {
    Object obj[] = m_selector.getSelectedObjects();
    if (obj == null) {
      LMError.report("No views selected");
      return;
    }
    if (obj.length != 2) {
      LMError.report("Select exactly 2 views");
      return;
    }
    DEViseViewPeer peer1 = (DEViseViewPeer)obj[0]; 
    DEViseViewPeer peer2 = (DEViseViewPeer)obj[1];
    DEViseViewPeer parent = (DEViseViewPeer)peer1.getParent();
    if (peer2.getParent() != parent) {
      LMError.report("The views to swap should have the same parent");
      return;
    }
    parent.exchange(peer1, peer2);
    updateLayout(parent);
    m_theApp.displayPanel().repaint();
  }

  public void showVisualLinks(Graphics g) {
    if (!m_theApp.showVisualLinks()) {
      return;
    }
    for (Enumeration e = m_session.myVisualLinks().list(); 
         e.hasMoreElements();){
      DEViseVisualLink vlink = (DEViseVisualLink)e.nextElement();
      ((DEViseVisualLinkPeer)m_visualLinkPeerTable.get(vlink.getName())).paint(g); 
    }
  }

  public void showRecordLinks(Graphics g) {
    if (!m_theApp.showRecordLinks()) {
      return;
    }
    for (Enumeration e = m_session.myRecordLinks().list(); e.hasMoreElements();){
      DEViseRecordLink rlink = (DEViseRecordLink)e.nextElement();
      ((DEViseRecordLinkPeer)m_recordLinkPeerTable.get(rlink.getName())).paint(g);
    }
  }

  public void showCursors(Graphics g){
    if (!m_theApp.showCursors()) {
      return;
    }
    for (Enumeration e = m_session.myCursors().list(); e.hasMoreElements(); ) {
      DEViseCursor cursor = (DEViseCursor)e.nextElement();
      ((DEViseCursorPeer)m_cursorPeerTable.get(cursor.getName())).paint(g);
    }
  }

  public void showTDataMap(Graphics g) {
    boolean showTData = m_theApp.showTDatas();
    boolean showMapping = m_theApp.showMappings();
    if (!showTData && !showMapping) {
      return;
    }
    for (Enumeration e = m_session.myViews().list(); e.hasMoreElements(); ) {
      DEViseView view = (DEViseView)e.nextElement();
      DEViseViewPeer peer = (DEViseViewPeer)m_viewPeerTable.get(view.getName());
      Rectangle peerBounds = peer.getBounds();
      Color clr = view.getFgColor().getColor();
      if (showTData) {
        DEViseTData tdata = view.getTData();
        if (tdata != null) {
          DEViseTDataPeer tpeer 
            = (DEViseTDataPeer) m_tdataPeerTable.get(tdata.getName());
          tpeer.paint(g, peerBounds.x + 5, peerBounds.y + 10, clr);
        } 
      }
      if (showMapping){
        DEViseMapping mapping = view.getMapping();
        if (mapping != null) {
          DEViseMappingPeer mpeer 
            = (DEViseMappingPeer)m_mappingPeerTable.get(mapping.getName());
          mpeer.paint(g, peerBounds.x + 5, peerBounds.y + 20, clr);
        }
      }
    }
  }


  public String getSessionFile() {
    return m_sessionFile;
  }

  public void open(String fileName) throws IOException, ClassNotFoundException {
    System.out.println("open(" + fileName + ")");
    FileInputStream fs = new FileInputStream(fileName);

    ReadObjectInput rs = new ReadObjectInput(fs);
    // skip comments in file header and rest of file
    m_session = new DEViseSession();
    m_session.readAscii(rs);
    
    DEViseView display = (DEViseView)m_session.myViews().find("Display");
    if (display == null) {
      throw new IOException("No display view ...");
    }
    createPeerHierarchy(null, display);
    loadLayout(rs);
    System.out.println("Done with loadLayoutAndCreatePeers");
    System.out.println("Printing current session:");
    System.out.println(m_session);

    loadSidePanel();
    m_sessionFile = fileName;
    rs.close();
  }
  
  public void loadSidePanel() {
    // record links
    RecordLinkPanel rpanel = m_theApp.recordLinkPanel();
    for (Enumeration d = m_session.myRecordLinks().list(); d.hasMoreElements();){
      DEViseRecordLink link = (DEViseRecordLink)d.nextElement();
      DEViseRecordLinkPeer peer = new DEViseRecordLinkPeer(this, link);
      m_recordLinkPeerTable.put(link.getName(), peer);
      rpanel.addItem(peer);
    }
    CursorPanel cpanel = m_theApp.cursorPanel();
    for (Enumeration d = m_session.myCursors().list(); d.hasMoreElements(); ) {
      DEViseCursor cursor = (DEViseCursor)d.nextElement();
      DEViseCursorPeer peer = new DEViseCursorPeer(this, cursor);
      m_cursorPeerTable.put(cursor.getName(), peer);
      cpanel.addItem(peer);
    }
    for (Enumeration d = m_session.myVisualLinks().list(); 
         d.hasMoreElements(); ) {
      DEViseVisualLink link = (DEViseVisualLink)d.nextElement();
      DEViseVisualLinkPeer peer = new DEViseVisualLinkPeer(this, link);
      m_visualLinkPeerTable.put(link.getName(), peer);
    }
    MappingPanel mpanel = m_theApp.mappingPanel();
    for (Enumeration d = m_session.myMappings().list(); d.hasMoreElements();) {
      DEViseMapping mapping = (DEViseMapping)d.nextElement();
      DEViseMappingPeer peer = new DEViseMappingPeer(this, mapping);
      m_mappingPeerTable.put(mapping.getName(), peer);
      mpanel.addItem(peer);
    }
    TDataPanel tpanel = m_theApp.tdataPanel();
    for (Enumeration d = m_session.myTDatas().list(); d.hasMoreElements();) {
      DEViseTData tdata = (DEViseTData)d.nextElement();
      DEViseTDataPeer peer = new DEViseTDataPeer(this, tdata);
      m_tdataPeerTable.put(tdata.getName(), peer);
      tpanel.addItem(peer);
    }
  }
  public void clearSidePanel() {
    // no need to clear the visual link panel as it has only types and no
    // instances
    for (Enumeration d = m_session.myVisualLinks().list(); 
         d.hasMoreElements();) {
      m_visualLinkPeerTable.remove(((DEViseVisualLink)d.nextElement()).getName());
    }
    // record links
    RecordLinkPanel rpanel = m_theApp.recordLinkPanel();
    for (Enumeration d = m_session.myRecordLinks().list(); d.hasMoreElements();){
      DEViseRecordLink rlink = (DEViseRecordLink)d.nextElement();
      m_recordLinkPeerTable.remove(rlink.getName());
      rpanel.delItem(rlink); 
    }
    // cursors
    CursorPanel cpanel = m_theApp.cursorPanel();
    for (Enumeration d = m_session.myCursors().list(); d.hasMoreElements(); ) {
      DEViseCursor cursor = (DEViseCursor)d.nextElement();
      m_cursorPeerTable.remove(cursor.getName());
      cpanel.delItem(cursor);
    }
    TDataPanel tpanel = m_theApp.tdataPanel();
    for (Enumeration d = m_session.myTDatas().list(); d.hasMoreElements(); ) {
      DEViseTData tdata = (DEViseTData)d.nextElement();
      m_tdataPeerTable.remove(tdata.getName());
      tpanel.delItem(tdata);
    }
    MappingPanel mpanel = m_theApp.mappingPanel();
    for (Enumeration d = m_session.myMappings().list(); d.hasMoreElements(); ) {
      DEViseMapping mapping = (DEViseMapping)d.nextElement();
      m_mappingPeerTable.remove(mapping.getName());
      mpanel.delItem(mapping);
    }
  }

  public void save(String fileName) throws IOException {
    System.out.println("save(" + fileName +")");
    FileOutputStream fs = new FileOutputStream(fileName);
    
    PrintObjectOutput ps = new PrintObjectOutput(fs);

    // first write version number and save date
    ps.writeChars("#DEVise Session \n");
    ps.writeChars("#Version : Layout Manager " + LMgr.VERSION + "\n");
    ps.writeChars("#Date : " + Calendar.getInstance().getTime() + "\n");
    
    // write core devise session stuff
    m_session.writeAscii(ps);
    
    // write layout stuff used by LM only
    saveLayout(m_Display, ps);
    m_sessionFile = fileName;
    ps.flush();
    ps.close();
  }

  private void saveLayout(DEViseViewPeer view, 
                          PrintObjectOutput ps) throws IOException{
    DEViseView v = view.getDEViseView();
    ps.writeChars(v.getName() + " ");
    ps.writeChars(view.getDEViseLayout().getClass().getName() + " ");
    view.getDEViseLayout().writeAscii(ps);
    ps.writeChars("\n");

    Component comp[] = view.getComponents();
    int count = view.getComponentCount();
    for (int i = 0; i < count; i++) {
      saveLayout((DEViseViewPeer)comp[i], ps);
    }
  }
  
  public void createPeerHierarchy(DEViseViewPeer parentPeer, DEViseView view) {
    System.out.println("createPeerHierarchy(" + view.getName() + "\n");
    DEViseViewPeer viewPeer = createPeer(view, null, parentPeer);
    if (parentPeer == null) {
      m_Display = viewPeer;
    }
    for (DListIterator d = view.childList(); d.hasMoreElements();) {
      createPeerHierarchy(viewPeer, (DEViseView)d.nextElement());
    }
  }
  public DEViseViewPeer findViewPeerByName(String name) {
    return (DEViseViewPeer)m_viewPeerTable.get(name);
  }
  /*
  public DEViseViewPeer findViewPeerByName(DEViseViewPeer parent,String name) {
    if (parent == null) {
      return null;
    }
    if (parent.getDEViseView().getName().equals(name)) {
      return parent;
    }
    Component comp[] = parent.getComponents();
    int count = parent.getComponentCount();
    for (int i = 0; i < count; i++) {
      DEViseViewPeer p = findViewPeerByName((DEViseViewPeer)comp[i], name);
      if (p != null) {
        return p;
      }
    }
    return null;
  }
  */
  public void loadLayout(ReadObjectInput rs) throws IOException
  {
    for (int i = 0; i < m_viewPeerTable.size(); i++) {
	    String viewName = rs.readUTF();
	    DEViseViewPeer peer = (DEViseViewPeer)m_viewPeerTable.get(viewName);
	    if (peer == null) {
	      LMError.report("The session file is invalid - view " + viewName + " not found\n");
	      return;
	    } 
	    String layoutMode = rs.readUTF();
	    System.out.println("Read " + viewName + " " + layoutMode);
	    try {
	      DEViseLayout layout = (DEViseLayout)Class.forName(layoutMode).newInstance();
	      layout.readAscii(rs);
	      peer.setDEViseLayout(layout);
	    } catch (IllegalAccessException e) {
	      System.out.println(e);
	      return;
	    } catch (ClassNotFoundException e) {
	      System.out.println(e);
	      return;
	    } catch (InstantiationException e) {
	      System.out.println(e);
	    }
    }
  }

  private DList getViews() {
    DList views = new DList();
    m_Display.getViews(views);
    return views;
  }
  
  public DList getSelectedLeafViews() {
    DList leaves = new DList();
    Object obj[] = m_selector.getSelectedObjects();
    if (obj != null) {
	    for (int i = 0; i < obj.length; i++){
	      if (((DEViseViewPeer)obj[i]).getComponentCount() == 0) {
	        leaves.add(((DEViseViewPeer)obj[i]).getDEViseView());
	        System.out.println("Selected view" + (DEViseViewPeer)obj[i]);
	      }
	    }
    }
	  if (leaves.size() == 0) {
	    LMError.report("There are no selected *leaf* views");
	  }
    return leaves;
  }
  class CurrentViewSelector extends MouseAdapter {
    public void mousePressed(MouseEvent e) {
      Object c = e.getSource();
      Object obj[] = m_selector.getSelectedObjects();
      if (obj == null) {
        return;
      }
      if (obj.length == 1) {
        setCurrentView((DEViseViewPeer)obj[0]);
      } else {
        System.out.println("....!!!!!!!!!GOCH!!!!!!!!!!");
        setCurrentView(null);
      }
    }
  }
  class ViewSelector extends ItemSelector { 
    public ViewSelector(Panel view) {
      super(view);
    }
    public Enumeration componentList() {
      return new Enumeration() {
        private DListIterator views = getViews().begin();
        public boolean hasMoreElements() {
          return views.hasMoreElements();
        }
        public Object nextElement() {
          return views.nextElement();
        }
      };
    }
  }
  public void repaint() {
    m_theApp.displayPanel().repaint();
  }
}
