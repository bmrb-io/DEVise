/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsn-Madison.
Author : Shilpa Lawande
class TestEditor.java
****************************************************************************/

//Packages 

import java.lang.*;
import java.awt.*;
import COM.objectspace.jgl.*;
import AwtUtil.*;
import LayoutEditor.*;

// Class
/**
 * The <CODE>TestEditor</CODE> class
 *
 * @author  Shilpa Lawande
 */


public class TestEditor extends Frame 
{
  /* number of instances */
  private static int m_numEditors = 0;
  private ScrollPane m_scrollPane;
  private EditCanvas m_canvas;
  private MenuMaker  m_control;  // stuff that creates and maintains all menus
  private MenuBar    m_menubar;  // the menu bar

  public static void main(String[] args)
  {
    new TestEditor();
  }

  /* Constructor */
  public TestEditor() {
    this("TestEditor.res");
  }
  public TestEditor(String resFile) {
    super("TestEditor");
    setSize(400,400);
    m_numEditors++;

    m_scrollPane = new ScrollPane();
    m_scrollPane.setSize(300, 300);
    this.add(m_scrollPane,"Center");

    m_canvas = new EditCanvas(500, 500);
    m_canvas.setBoundingBox(10,10, 400,400);
    m_scrollPane.add(m_canvas);

    m_control = new MenuMaker(this, resFile); // create menus
    m_menubar = m_control.createMenuBar(null);  // make a menubar out of them
    this.setMenuBar(m_menubar);
    this.pack();
    this.show();
  }

  // methods for menu commands
  public void OnFileNew() {
    new TestEditor();
  }
  public void OnFileClose() {
    if (--m_numEditors == 0) {
      System.exit(0);
    } else {
      TestEditor.this.dispose();
    } 
  }
  public void OnDrawViewSingle() {
    m_canvas.setMode(EditCanvas.DRAWRECTSINGLE);
  }
  public void OnDrawViewMultiple(CheckboxMenuItem cbmi) {
    if (m_canvas.getMode() == EditCanvas.DRAWRECTMULTI) {
      m_canvas.setMode(EditCanvas.EDITLAYOUT);
    } else {
      m_canvas.setMode(EditCanvas.DRAWRECTMULTI);
    }
  }
  public void OnDrawGroup() { 
    m_canvas.group();
  }
  public void OnDrawUngroup() {
    m_canvas.ungroup();
  }
  public void OnDrawRefresh() {
    m_canvas.repaint();
  }
  public void OnDrawClear() {
    m_canvas.clear();
  }
  public void OnDrawDelete() {
    m_canvas.deleteSelections();
  }
  public void OnDrawCopy() {
    m_canvas.copySelection();
  }
  public void OnEditAlignLeft() {
    m_canvas.align(Box.ALIGN_LEFT);
  }
  public void OnEditAlignRight() {
    m_canvas.align(Box.ALIGN_RIGHT);
  }
	public void OnEditAlignTop() {
    m_canvas.align(Box.ALIGN_TOP);
	}
	public void OnEditAlignBottom() {
    m_canvas.align(Box.ALIGN_BOTTOM);
	}
	public void OnEditEqualizeWidth() {
    m_canvas.equalizeDimension(Box.DIMENSION_WIDTH);
	}
	public void OnEditEqualizeHeight() {
    m_canvas.equalizeDimension(Box.DIMENSION_HEIGHT);
	}
	public void OnEditEdgesAlign() {
    m_canvas.alignEdges();
	}
	public void OnEditEdgesRivet() {
    m_canvas.rivetEdges();
  }
	public void OnEditEdgesUnRivet() {
    m_canvas.unrivetEdges();
	}
  public void OnEditDistributeHorizontally() {
    m_canvas.distribute(Box.DIMENSION_WIDTH);
  }
  public void OnEditDistributeVertically() {
    m_canvas.distribute(Box.DIMENSION_HEIGHT);
  }
}
