/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class SideCardPanel.java
****************************************************************************/

//Packages 

package LayoutMgr.Application;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import COM.objectspace.jgl.*;
import AwtUtil.*;
import gjt.*;

// Class
/**
 * The <CODE>SideCardPanel</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class SideCardPanel extends Panel 
{
  public static int DEFAULT_WD = 20;
  public static int DEFAULT_HT = 20;
  public static final String VLINK_GIF = "/u/s/s/ssl/Work/LM/Icons/gifs/XYLink.gif";
  public static final String RLINK_GIF = "/u/s/s/ssl/Work/LM/Icons/gifs/RLink.gif";
  public static final String CURSOR_GIF = "/u/s/s/ssl/Work/LM/Icons/gifs/Cursor.gif";
  public static final String MAPPING_GIF = "/u/s/s/ssl/Work/LM/Icons/gifs/Mapping.gif";
  public static final String TABLE_GIF = "/u/s/s/ssl/Work/LM/Icons/gifs/TData.gif";
  
  private LMgr m_theApp;
  private int m_width, m_height;
  private NicerCardPanel m_panel = new NicerCardPanel(new Insets(0,0,0,0), 
                                                    BorderStyle.RAISED);
  private VisualLinkPanel m_visualLinkPanel;
  private RecordLinkPanel m_recordLinkPanel;
  private CursorPanel m_cursorPanel;
  //private MappingPanel m_mappingPanel;
  //private TDataPanel m_tdataPanel;
  private TDataMapPanel m_tdatamapPanel;

  /* Constructor */
  public SideCardPanel(LMgr theApp, int width, int height) {
    super(new BorderLayout());   
    m_theApp = theApp;

    m_width = width;
    m_height = height;

    Image mapGif 
      = createImage(ImageItem.getImageProducer(MAPPING_GIF)).getScaledInstance(DEFAULT_WD, DEFAULT_HT, Image.SCALE_AREA_AVERAGING);
    m_tdatamapPanel = new TDataMapPanel(m_theApp, m_width, m_height);
    m_panel.addImageButton(mapGif, "TData/Map", m_tdatamapPanel);

    Image vlinkGif = createImage(ImageItem.getImageProducer(VLINK_GIF)).getScaledInstance(DEFAULT_WD, DEFAULT_HT, Image.SCALE_AREA_AVERAGING);
    m_visualLinkPanel = new VisualLinkPanel(m_theApp, m_width, m_height);
    m_panel.addImageButton(vlinkGif, "Visual Link", m_visualLinkPanel); 
                           
    Image rlinkGif = createImage(ImageItem.getImageProducer(RLINK_GIF)).getScaledInstance(DEFAULT_WD, DEFAULT_HT, Image.SCALE_AREA_AVERAGING);
    m_recordLinkPanel = new RecordLinkPanel(m_theApp, m_width, m_height);
    m_panel.addImageButton(rlinkGif, "Record Link", m_recordLinkPanel);

    Image cursorGif = createImage(ImageItem.getImageProducer(CURSOR_GIF)).getScaledInstance(DEFAULT_WD, DEFAULT_HT, Image.SCALE_AREA_AVERAGING);
    m_cursorPanel = new CursorPanel(m_theApp, m_width, m_height);
    m_panel.addImageButton(cursorGif, "Cursor", m_cursorPanel);
    
    /*
    Image mapGif = createImage(ImageItem.getImageProducer(MAPPING_GIF)).getScaledInstance(DEFAULT_WD, DEFAULT_HT, Image.SCALE_AREA_AVERAGING);
    m_mappingPanel = new MappingPanel(m_theApp, m_width, m_height);
    m_panel.addImageButton(mapGif, "Mapping", m_mappingPanel);

    Image tdataGif = createImage(ImageItem.getImageProducer(TABLE_GIF)).getScaledInstance(DEFAULT_WD, DEFAULT_HT, Image.SCALE_AREA_AVERAGING);
    m_tdataPanel =  new TDataPanel(m_theApp, m_width, m_height);
    m_panel.addImageButton(tdataGif, "TData", m_tdataPanel);
    */


    add("Center", m_panel);
  }
  
  public Dimension getPreferredSize() {
    return new Dimension(m_width, m_height);
  }
  public VisualLinkPanel visualLinkPanel() {
    return m_visualLinkPanel;
  }
  public RecordLinkPanel recordLinkPanel() {
    return m_recordLinkPanel;
  }
  public CursorPanel cursorPanel() {
    return m_cursorPanel;
  }
  public MappingPanel mappingPanel() {
    return m_tdatamapPanel.mappingPanel();
  }
  public TDataPanel tdataPanel() {
    return m_tdatamapPanel.tdataPanel(); 
  }
  public boolean isRecordLinkMode() {
    return m_panel.getMode().equals("Record Link");
  }
  public boolean isVisualLinkMode() {
    return m_panel.getMode().equals("Visual Link");
  }
  public boolean isCursorMode() {
    return m_panel.getMode().equals("Cursor");
  }
  public boolean isTDataMapMode() {
    return m_panel.getMode().equals("TData/Map");
  }

  class NicerCardPanel extends NiceCardPanel implements ActionListener {
    public NicerCardPanel() {
      super();
    }
    public NicerCardPanel(Insets insets) {
      super(insets);
    }
    public NicerCardPanel(Insets insets, BorderStyle borderstyle) {
      super(insets, borderstyle);
    }
    public void actionPerformed(ActionEvent e) {
      super.actionPerformed(e);
      m_theApp.updateUI(m_theApp);
    }
  }
}
