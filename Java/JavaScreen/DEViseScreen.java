// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// $Id$

// $Log$

// ========================================================================

// DEViseScreen.java

import java.awt.*;
import java.awt.event.*;
import java.util.*;

public class DEViseScreen extends Panel
{
    jsdevisec jsc = null;

    Dimension screenDim = new Dimension(600, 400);
    Dimension screenSize = null;

    Vector allCanvas = new Vector();
    Vector allViews = new Vector();
    Hashtable viewTable = new Hashtable();
    Vector javaGDatas = new Vector();

    DEViseView currentView = null;
    public DEViseView lastActionView = null;
    public boolean guiAction = false;

    boolean isDimChanged = false;

    public Image offScrImg = null;

    private boolean isGDataAdded = false, isGDataDeleted = false, isCanvasAdded = false, isCanvasDeleted = false;
    private DEViseCanvas whichCanvasAdded = null, whichCanvasDeleted = null;
    private int whichCanvasAddedWhere = 0;
    private Vector whichGDataAdded = null, whichGDataDeleted = null;

    public DEViseScreen(jsdevisec what)
    {
        jsc = what;

        screenSize = new Dimension(DEViseGlobals.screenSize.width, DEViseGlobals.screenSize.height);
        screenDim = new Dimension(DEViseGlobals.screenSize.width - 2 * DEViseGlobals.screenEdge.width, DEViseGlobals.screenSize.height - 2 * DEViseGlobals.screenEdge.height);

        setLayout(null);
        setBackground(DEViseGlobals.bg);
        setForeground(DEViseGlobals.fg);
        setFont(DEViseGlobals.font);

        addMouseMotionListener(new MouseMotionAdapter()
            {
                public void mouseMoved(MouseEvent event)
                {
                    Point p = event.getPoint();
                    if (jsc.dispatcher.getStatus() != 0) {
                        setCursor(DEViseGlobals.waitCursor);
                    } else {
                        setCursor(DEViseGlobals.defaultCursor);
                    }

                    if (currentView != null) {
                        jsc.viewInfo.updateInfo();

                        setCurrentView(null);
                    }
                }
            });
    }

    public Dimension getPreferredSize()
    {
        return screenSize;
    }

    public Dimension getMinimumSize()
    {
        return screenSize;
    }

    public Dimension getScreenDim()
    {
        return screenDim;
    }

    // At this time, you can not change the size of the screen after a session
    // is opened. If you really want to change the screen size, you will need
    // to close the session, re-set the size of the screen and reopen the
    // session
    public void resetScreenDim()
    {
        screenSize = new Dimension(DEViseGlobals.screenSize.width, DEViseGlobals.screenSize.height);
        screenDim = new Dimension(DEViseGlobals.screenSize.width - 2 * DEViseGlobals.screenEdge.width, DEViseGlobals.screenSize.height - 2 * DEViseGlobals.screenEdge.height);

        isDimChanged = true;

        repaint();
    }

    public void setScreenDim(int width, int height)
    {
        if (width < 1 || height < 1)
            return;

        screenDim.width = width;
        screenDim.height = height;
        screenSize.width = screenDim.width + 2 * DEViseGlobals.screenEdge.width;
        screenSize.height = screenDim.height + 2 * DEViseGlobals.screenEdge.height;

        isDimChanged = true;

        repaint();
    }

    public void addView(DEViseView view)
    {
        if (view == null)
            return;

        // view already exists
        if (viewTable.get(view.viewName) != null)
            return;

        if (view.parentView != null) {
            view.parentView.addChild(view);
        }
        
        if (view.piledView != null) {
            view.piledView.addPile(view);
        }
        
        allViews.addElement(view);
        viewTable.put(view.viewName, view);
    }

    public void updateViewImage(String name, Image image)
    {
        if (name == null || image == null)
            return;

        DEViseView view = getView(name);

        if (view == null)
            return;

        if (view.canvas == null) {
            DEViseCanvas canvas = new DEViseCanvas(view, image);
            view.canvas = canvas;

            whichCanvasAddedWhere = 0;
            for (int i = 0; i < allCanvas.size(); i++) {
                DEViseCanvas c = (DEViseCanvas)allCanvas.elementAt(i);
                if (c.view.viewZ < canvas.view.viewZ) {
                    whichCanvasAddedWhere = i;
                    break;
                }
            }

            allCanvas.insertElementAt(canvas, whichCanvasAddedWhere);

            isCanvasAdded = true;
            whichCanvasAdded = canvas;
        } else {
            view.canvas.updateImage(image);
        }

        repaint();
    }

    public void removeChildViews(String name)
    {
        if (name == null)
            return;

        removeChildViews(getView(name));
    }

    public void removeChildViews(DEViseView view)
    {
        if (view == null)
            return;

        Vector child = view.viewChilds;
        while (child.size() > 0) {
            removeView(view.getChild(0));
        }
        // Can not use the for iteration, because removeView also remove that view
        // from its parents, the the size of the viewChilds of the parentView is
        // actually changing on the fly
        //for (int i = 0; i < child.size(); i++) {
        //    removeView(view.getChild(i));
        //}

        repaint();
    }

    public void removeView(String name)
    {
        if (name == null)
            return;

        removeView(getView(name));
    }

    public synchronized void removeView(DEViseView view)
    {
        if (view != null) {
            removeChildViews(view);

            if (currentView == view) {
                currentView = null;
            }

            if (view.parentView == null) {
                // Only top level view will have gdata
                Vector gdata = view.viewGDatas;
                if (gdata.size() > 0) {
                    for (int i = 0; i < gdata.size(); i++) {
                        DEViseGData data = view.getGData(i);
                        if (data.isJavaSymbol) {
                            javaGDatas.removeElement(view.getGData(i));
                        }   
                    }

                    view.removeAllGData();

                    isGDataDeleted = true;
                    whichGDataDeleted = gdata;
                }
            } else {
                view.parentView.removeChild(view);
            }
            
            if (view.piledView != null) {
                view.piledView.removePile(view);
            }
            
            if (view.canvas != null) {
                allCanvas.removeElement(view.canvas);
                isCanvasDeleted = true;
                whichCanvasDeleted = view.canvas;
            }

            allViews.removeElement(view);
            viewTable.remove(view.viewName);

            repaint();
        }
    }

    public DEViseView getView(String name)
    {
        if (name == null)
            return null;

        return (DEViseView)viewTable.get(name);
    }

    public DEViseView getCurrentView()
    {
        return currentView;
    }

    public void setCurrentView(DEViseView view)
    {
        if (currentView != null) {
            if (currentView.getCanvas() != null) {
                currentView.getCanvas().repaint();
            }
        }

        currentView = view;

        //if (currentView != null) {
        //    currentView.isFirstTime = true;
        //}

        if (!jsc.isShowingMsg() && currentView != null) {
            requestFocus();
            if (currentView.getCanvas() != null)
                currentView.getCanvas().requestFocus();
        }
    }

    public synchronized void updateGData(String name, Vector vect)
    {
        DEViseView view = getView(name);

        if (view != null) {
            // remove old GData if any
            // Only top level view will have gdata
            Vector gdata = view.viewGDatas;
            whichGDataDeleted = new Vector();
            if (gdata.size() > 0) {
                for (int i = 0; i < gdata.size(); i++) {
                    //remove(gdata[i].symbol);
                    DEViseGData data = view.getGData(i);
                    if (data.isJavaSymbol) {
                        javaGDatas.removeElement(data);
                        whichGDataDeleted.addElement(data);
                    }    
                }

                view.removeAllGData();

                isGDataDeleted = true;
                //whichGDataDeleted = gdata;
            }

            if (vect != null && vect.size() > 0) {
                for (int i = 0; i < vect.size(); i++) {
                    DEViseGData g = (DEViseGData)vect.elementAt(i);
                    if (g.isJavaSymbol) {
                        javaGDatas.addElement(g);
                    }
                                 
                    view.addGData(g);
                }

                isGDataAdded = true;
                whichGDataAdded = vect;
            }

            repaint();
        } else {
            jsc.pn("Invalid view name in DEViseScreen::updateGData");
        }
    }

    public void updateCursor(String name, DEViseCursor cursor)
    {
        DEViseView view = getView(name);

        if (view != null) {
            if (cursor != null) {                
                if (view.addCursor(cursor)) {
                    repaint();
                }
            }
        } else {
            jsc.pn("Invalid view name in DEViseScreen::updateCursor");
        }
    }

    public void removeCursor(String name, String viewname)
    {
        DEViseView view = getView(viewname);

        if (view != null) {
            if (view.removeCursor(name)) {
                repaint();
            }
        }
    }

    public void updateViewDataRange(String name, String axis, double min, double max)
    {
        DEViseView view = getView(name);

        if (view != null && axis != null) {
            view.updateDataRange(axis, min, max);
        }
    }
    
    public void setLastAction()
    {
        if (guiAction) {
            guiAction = false;
            if (currentView != null) {
                if (lastActionView != currentView) {
                    if (lastActionView != null) {
                        lastActionView.isFirstTime = true;
                    } 
                    
                    lastActionView = currentView;
                    if (currentView.isFirstTime) {
                        currentView.isFirstTime = false;
                    }
                }
            }
        }                
    }
            
    public synchronized void updateScreen(boolean flag)
    {
        if (flag) {
            jsc.isSessionOpened = true;
        } else {
            removeAll();

            allCanvas.removeAllElements();
            allViews.removeAllElements();
            javaGDatas.removeAllElements();
            viewTable = new Hashtable();

            currentView = null;
            lastActionView = null;

            jsc.isSessionOpened = false;

            jsc.viewInfo.updateInfo();
            jsc.viewInfo.updateImage(0, 0);
            jsc.viewInfo.updateCount(0);
            
            jsc.lastCursor = DEViseGlobals.defaultCursor;
            setCursor(jsc.lastCursor);
            
            offScrImg = null;

            repaint();
        }
    }

    /*
    public void moveView(DEViseView view, int x, int y, boolean isFinal)
    {
        if (view == null)
            return;

        if (view.parentView != null)
            return;

        if (!isFinal) {
            Rectangle loc = view.viewLoc;
            newViewLoc.x = loc.x + x;
            newViewLoc.y = loc.y + y;
            newViewLoc.width = loc.width;
            newViewLoc.height = loc.height;
            isDrawPos = true;
            offScrImg = null;
            repaint();
        } else {
            view.setLoc(new Rectangle(newViewLoc.x, newViewLoc.y, newViewLoc.width, newViewLoc.height));
            view.setBounds(view.getLocInScreen());
            offScrImg = null;
        }
    }
    */

    // Enable double-buffering
    public void update(Graphics g)
    {
        if (g == null) {
            jsc.pn("Null graphics received in screen update");
            return;
        }

        if (offScrImg == null) {
            offScrImg = createImage(screenSize.width, screenSize.height);
            if (offScrImg == null) {
                jsc.pn("Can not create empty offScrImg in screen update!");
                return;
            }
        }

        Graphics og = offScrImg.getGraphics();
        paint(og);
        g.drawImage(offScrImg, 0, 0, this);
        og.dispose();
    }

    public void paint(Graphics g)
    {
        if (isDimChanged) {
            isDimChanged = false;
            setSize(screenSize);
            if (DEViseGlobals.inBrowser) {
                jsc.validate();
            } else {
                jsc.parentFrame.pack();
            }
            jsc.repaint();
        }

        if (isCanvasDeleted && whichCanvasDeleted != null) {
            remove(whichCanvasDeleted);
            isCanvasDeleted = false;
            whichCanvasDeleted = null;
        }

        if (isCanvasAdded && whichCanvasAdded != null) {
            add(whichCanvasAdded, whichCanvasAddedWhere);
            whichCanvasAdded.setBounds(whichCanvasAdded.getBoundsInScreen());
            isCanvasAdded = false;
            whichCanvasAdded = null;
        }

        if (isGDataDeleted && whichGDataDeleted != null) {
            for (int i = 0; i < whichGDataDeleted.size(); i++) {
                DEViseGData gdata = (DEViseGData)whichGDataDeleted.elementAt(i);
                if (gdata.isJavaSymbol) {
                    remove(gdata.symbol);
                }    
            }

            isGDataDeleted = false;
            whichGDataDeleted = null;
        }

        if (isGDataAdded && whichGDataAdded != null) {
            for (int i = 0; i < whichGDataAdded.size(); i++) {
                DEViseGData gdata = (DEViseGData)whichGDataAdded.elementAt(i);
                if (gdata.isJavaSymbol) {
                    Component gs = gdata.symbol;
                    add(gs);
                    gs.setBounds(gdata.getBoundsInScreen());
                }    
            }

            isGDataAdded = false;
            whichGDataAdded = null;
        }

        super.paint(g);

        /*
        if (isDrawPos) {
            Color oldColor = g.getColor();
            g.setColor(Color.white);
            g.drawRect(newViewLoc.x, newViewLoc.y, newViewLoc.width, newViewLoc.height);
            g.setColor(oldColor);
            isDrawPos = false;
        }
        */

        Color oldColor = g.getColor();
        g.setColor(Color.white);
        for (int i = 0; i < DEViseGlobals.screenEdge.width; i++) {
            g.drawLine(i, 0, i, screenSize.height - 1);
            g.drawLine(screenSize.width - i - 1, 0, screenSize.width - i - 1, screenSize.height - 1);
        }
        for (int i = 0; i < DEViseGlobals.screenEdge.height; i++) {
            g.drawLine(0, i, screenSize.width - 1, i);
            g.drawLine(0, screenSize.height - i - 1, screenSize.width - 1, screenSize.height - i - 1);
        }
        g.setColor(oldColor);
    }
}
