/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DEViseEditCanvas.java
****************************************************************************/

//Packages 

package LayoutMgr.Application;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import COM.objectspace.jgl.*;
import LayoutEditor.*;
import LayoutMgr.MetaData.*;
import AwtUtil.*;

// Class
/**
 * The <CODE>DEViseEditCanvas</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class DEViseEditCanvas extends EditCanvas
{
  private LMgr m_theApp; // the layout manager app this is part of

  private double m_xscale, m_yscale;   // scale used for displaying stuff
  private DEViseViewPeer m_currentView;  // the view whose layout is being 
                                         // edited.
  /* Public members */
  public DEViseEditCanvas(LMgr theApp, int width, int height) {
    super(width, height);
    m_theApp = theApp;
    m_currentView = null;
  }

  public DListIterator getBoxes() { return m_layoutState.boxList(); }

  public void Init(DEViseViewPeer currentView) {
    if (currentView == null) {
      LMError.report("No current view selected");
      return;
    }
    m_currentView = currentView;
    // determine the scale to draw the parent rectangle
    // try to scale to the height of the display and adjust width 
    // according to aspect ratio
    DEViseView view = currentView.getDEViseView(); 
    m_yscale = LMgr.getDisplayHeight() * LMgr.getGlobalScaleX() / (double)view.getHeight(); 
    m_xscale = m_yscale;
    System.out.println("xscale = " + m_xscale + " yscale = " + m_yscale);
    clear();  
    setSize((int)(view.getWidth() * m_xscale),
            (int)(view.getHeight() * m_yscale));
    setBoundingBox(0, 0,
                  (int)(view.getWidth() * m_xscale),
                  (int)(view.getHeight() * m_yscale));
    m_layoutState = currentView.getLayoutState();

    if (m_layoutState == null) {
      System.out.println("Creating new layout state");
      m_layoutState = new DEViseLayoutState();
    } else {
      System.out.println("Restoring old layout state");
    }
    for (DListIterator children = view.childList();
         children.hasMoreElements(); ) { 
      DEViseView child = (DEViseView)children.nextElement();
      int x = (int)(child.getX() * m_xscale);
      int y = (int)(child.getY() * m_yscale);
      int width = (int) (child.getWidth() * m_xscale);
      int height = (int) (child.getHeight() * m_yscale);
      createBox(child, x, y, width, height); 
    }
    // Problem : if the parent's layout changes thus changing the
    // size of currentView, then the saved layout state is invalid
    // even though the rivets and group can be maintained !!
    // maybe restoring the layout State should involve updating the
    // positions and sizes of boxes while maintaining groups and rivets?
    repaint();
  }
  
  // allow creation of boxes which carry along their view identity
  // so that new ones can be distinguished!
  public void createBox(DEViseView view, int x, int y, int width, int height) {
    System.out.println("CreateBox " + view.getName() +
                        "  " + x +
                        "  " + y +
                        "  " + width + 
                        "  " + height);
    DEViseLayoutState layoutState = (DEViseLayoutState)m_layoutState;
    Box b = layoutState.findBox(view);
    if (b == null) {
      b = new Box(new Rectangle(x,y, width, height));
      b.setContainer(this);
	    layoutState.addBox(b, view);
      System.out.println("New box " + view.getName() + " " + b.getBounds());
    } else {
      System.out.println("Updating box " + view.getName());
      int x0 = x;
      int y0 = y;
      int x1 = x + width;
      int y1 = y + height;
      b.init(x0, y0, x1, y1);
    }
  }

  // Menu Handlers
  public void OnFileOpen() { }
  public void OnFileSave() { }
  public void OnFileSaveAs() { }
  public void OnApply() {
    if (m_currentView == null) {
      System.out.println("Something is wrong!!");
    }
    // Apply changes
    DEViseSessionPeer session = m_currentView.getSession();
    DEViseLayoutState lstate = (DEViseLayoutState)m_layoutState;
    System.out.println(lstate);
    for (DListIterator d = lstate.getPairs(); d.hasMoreElements(); ) {
      Pair p = (Pair)d.nextElement();
      if ((p.first == null) && (p.second == null)) {
        System.out.println("!!!!!!!!!!!Something fishy!!!!!!!!!!!!!!!");
      }
      Box b = (Box)p.first;
      DEViseView view = (DEViseView)p.second;
      if (b == null) {
        boolean recursive = false;
        if (view.numChildren() != 0) {
          MessageDialog dlg = new MessageDialog("View Deletion Question?",
           "Deleted View -" + view.getName() + "- has children - delete recursively ? ", MessageDialog.YESNO, 0); 
          if (dlg.doModal() == 0) {
            recursive = true;
          } 
        }
        session.deleteView(view, recursive);
        lstate.markForDeletion(p);
      } else {
        if (view == null) {
          DEViseViewPeer viewP = session.createView(m_currentView);
          if (viewP == null) {
          System.out.println("Something is wrong!!");
          }
          view = viewP.getDEViseView();
          p.second = view;
        } 
        Rectangle r = b.getBounds();
        System.out.println("Apply change: b = " + r);
        double x = (double)r.x / (double) m_xscale;
        double y = (double)r.y / (double) m_yscale;
        double width = (double) r.width / (double) m_xscale;
        double height = (double) r.height / (double) m_yscale;
        System.out.println(" x = " + x +
                           " y = " + y +
                           " w = " + width + 
                           " h = " + height);
        view.setX((int)x);
        view.setY((int)y);
        view.setWidth((int)width);
        view.setHeight((int)height);
      }
    }
    lstate.deleteMarkedEntries();
    m_currentView.setLayoutState(m_layoutState);
    System.out.println("LayoutState: " + m_layoutState);
    m_currentView = null;
    m_layoutState = null;
    m_theApp.changeMode(LMgr.MODE_DISPLAY);  
    m_theApp.repaint();
  }

  public void OnViewNew() { setMode(EditCanvas.DRAWRECTSINGLE); }
  // special toolbar functions
  public  void OnViewNewMultiOn() { setMode(EditCanvas.DRAWRECTMULTI); }
  public void OnViewNewMultiOff() { setMode(EditCanvas.EDITLAYOUT); }

  public void OnViewGroup() { group(); }
  public void OnViewUnGroup() { ungroup(); }

  public void OnCut() { }
  public void OnCopy() { copySelection(); }
  public void OnPaste() { }
  public void OnViewDelete() { deleteSelections(); }
  public void OnRefresh() { repaint(); }
  public void OnClear() { clear(); }

  public void OnAlignLeft() { align(Box.ALIGN_LEFT); }
  public void OnAlignCenter() { align(Box.ALIGN_CENTER); }
  public void OnAlignRight() { align(Box.ALIGN_RIGHT); }
	public void OnAlignTop() { align(Box.ALIGN_TOP); } 
  public void OnAlignMiddle() { align(Box.ALIGN_MIDDLE); }
	public void OnAlignBottom() { align(Box.ALIGN_BOTTOM); }

	public void OnEqualizeWidth() { equalizeDimension(Box.DIMENSION_WIDTH); }
	public void OnEqualizeHeight() { equalizeDimension(Box.DIMENSION_HEIGHT); }
  public void OnEqualizeSize() {
    equalizeDimension(Box.DIMENSION_WIDTH);
    equalizeDimension(Box.DIMENSION_HEIGHT);
  }

	public void OnAlignEdges() { alignEdges(); }

	public void OnRivet() { rivetEdges(); }
	public void OnUnrivet() { unrivetEdges(); }
  public void OnDistributeHorizontally() { distribute(Box.DIMENSION_WIDTH); }
  public void OnDistributeVertically() { distribute(Box.DIMENSION_HEIGHT); }
  public void OnConfigureRbndColors() { configureRbndColors(); }
  public void OnConfigureEditColors() { configureEditColors(); }

}
