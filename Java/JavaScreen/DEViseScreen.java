import java.awt.*;
import java.awt.event.*;
import java.util.*;

public class DEViseScreen extends Panel
{
    jsdevisec jsc = null;
    Dimension screenDim = UIGlobals.SCREENDIM3;
    Dimension screenEdge = new Dimension(4, 4);
    Dimension screenSize = null;
    Color screenColor = UIGlobals.uibgcolor;
    Font screenFont = UIGlobals.uifont;
    boolean isDimChanged = false;
    boolean isUpdate = false;
    public boolean isGrid = false;
    Vector allWindows = new Vector();
    DEViseWindow currentWindow = null;
     
    public DEViseScreen(jsdevisec what)
    {   
        if (what == null)
            YDebugInfo.println("Null argument received during DEViseScreen Construction!");
                 
        jsc = what;
        setLayout(null);
        
        setBackground(screenColor);
        setForeground(screenColor);
        setFont(screenFont);                
        screenSize = new Dimension(screenDim.width + 2 * screenEdge.width, screenDim.height + 2 * screenEdge.height);
    }
    
    public DEViseScreen(jsdevisec what, Dimension dim, Color color, Font font)
    {
        if (jsc == what || dim == null || color == null || font == null)
            YDebugInfo.println("Null argument received during DEViseScreen Construction!");

        jsc = what;
        screenDim = dim;
        screenColor = color;
        screenFont = font;
        
        setLayout(null);
        setBackground(screenColor);
        setForeground(screenColor);
        setFont(screenFont);
        screenSize = new Dimension(screenDim.width + 2 * screenEdge.width, screenDim.height + 2 *screenEdge.height);
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
    public synchronized void setScreenDim(Dimension dim)
    {
        if (dim == null) {
            YDebugInfo.println("Null argument received in setScreenDim-DEViseScreen!");
            return;
        }
            
        screenDim = dim;
        screenSize = new Dimension(screenDim.width + 2 * screenEdge.width, screenDim.height + 2 * screenEdge.height);
        isDimChanged = true;
        repaint();
    }
    
    public synchronized void addWindow(DEViseWindow win)
    {   
        if (win == null) {
            YDebugInfo.println("Null argument received in addWindow-DEViseScreen!");
            return;
        }
        
        allWindows.addElement(win);
    }
    
    public synchronized void removeWindow(String name)
    {   
        if (name == null) {
            YDebugInfo.println("Null argument received in removeWindow-DEViseScreen!");
            return;
        } 
        
        DEViseWindow win = getWindow(name);
        if (win != null)
            allWindows.removeElement(win);
    }
    
    public synchronized void updateWindow(String name, Image img)
    {   
        if (img == null || name == null) {
            YDebugInfo.println("Null argument received in updateWindow-DEViseScreen!");
            return;
        }
            
        DEViseWindow win = getWindow(name);
        if (win == null)
            return;
        
        win.setImage(img);
    }
    
    public DEViseWindow getWindow(String name)
    {
        if (name == null) {
            YDebugInfo.println("Null argument received in getWindow-DEViseScreen!");
            return null;
        } 

        int number = allWindows.size();
        DEViseWindow win = null;
        
        for (int i = 0; i < number; i++) {
            win = (DEViseWindow)allWindows.elementAt(i);
            if (win.getName().equals(name))
                break;
        }
        
        return win;
    }
    
    public DEViseWindow getCurrentWindow()
    {
        return currentWindow;
    } 
    
    public synchronized void setCurrentWindow(DEViseWindow win)
    {
        if (win == null) {
            YDebugInfo.println("Null argument received in setCurrentWindow-DEViseScreen!");
            return;
        }
        
        if (currentWindow != null)
            currentWindow.setCurrent(false);
            
        currentWindow = win;
        currentWindow.setCurrent(true);
    }
    
    public Vector getAllWindows()
    {
        return allWindows;
    }
    
    public DEViseView getView(String name)
    {
        if (name == null) {
            YDebugInfo.println("Null argument received in getView-DEViseScreen!");
            return null;
        }
         
        int number = allWindows.size();
        DEViseWindow win = null;
        DEViseView view = null;
        
        for (int i = 0; i < number; i++) {
            win = (DEViseWindow)allWindows.elementAt(i);
            view = win.getView(name);
            if (view != null) 
                break;
        }
        
        return view;        
    }    
    
    public synchronized void updateScreen(boolean flag)
    {
        removeAll();
        
        if (flag) {
            int number = allWindows.size();
            for (int i = 0; i < number; i++)
                add((DEViseWindow)allWindows.elementAt(i)); 
            
            isUpdate = true;
            jsc.isSessionOpened = true;
            repaint();
        } else {
            int number = allWindows.size();
            
            //for (int i = 0; i < number; i++) 
            //    allWindows.elementAt(i) = null;
            
            allWindows.removeAllElements();
            currentWindow = null;
            jsc.isSessionOpened = false;
            jsc.viewInfo.updateInfo();
            jsc.viewControl.updateControl();
            repaint();
        }
    }
    
    public void paint(Graphics g)
    {
        if (isDimChanged) {
            isDimChanged = false;
            setSize(screenSize);
            //jsc.doLayout();
            //jsc.validate();
            jsc.pack();
            jsc.repaint();
        }
        
        if (isUpdate) {
            int number = allWindows.size();
            DEViseWindow win = null;
            Rectangle bound = null;
            for (int i = 0; i < number; i++) {
                win = (DEViseWindow)allWindows.elementAt(i);
                bound = win.getLoc();
                bound.x += screenEdge.width;
                bound.y += screenEdge.height;
                if (bound != null)
                    win.setBounds(bound);    
            }
            
            isUpdate = false;
        }
        
        super.paint(g);
        
        Color oldColor = g.getColor();
        g.setColor(Color.white);
        for (int i = 0; i < screenEdge.width; i++) {
            g.drawLine(i, 0, i, screenSize.height - 1);
            g.drawLine(screenSize.width - i - 1, 0, screenSize.width - i - 1, screenSize.height - 1);
        }
        for (int i = 0; i < screenEdge.height; i++) {
            g.drawLine(0, i, screenSize.width - 1, i);
            g.drawLine(0, screenSize.height - i - 1, screenSize.width - 1, screenSize.height - i - 1);
        }
        g.setColor(oldColor);    
    }    
}