import java.awt.*;

public class DEViseGData 
{   
    float x0 = 0, y0 = 0, size = 0;
    public int x = 0, y = 0, width = 0, height = 0;
    String label = null;
   
    public DEViseGData(String data, float xm, float xo, float ym, float yo) throws YException
    {
        if (data == null) 
            throw new YException("Null GData!");
        
        String[] value = YGlobals.Yparsestr(data);
        if (value == null || value.length != 18) 
            throw new YException("Invalid GData!");
        
        try {
            x0 = (Float.valueOf(value[0])).floatValue();
            x = (int)(x0 * xm + xo);
            y0 = (Float.valueOf(value[1])).floatValue();
            y = (int)(y0 * ym + yo);  
            size = (Float.valueOf(value[4])).floatValue();
            width = (int)(size * xm);
            height = (int)(size * ym); 
            if (width < 0)
                width = -width;
            if (height < 0) 
                height = -height;
            x = x - width / 2;    
            y = y - height / 2; 
            if (x < 0)
                x = 0;
            if (y < 0)
                y = 0;    
        } catch (NumberFormatException e) {
            throw new YException("Invalid GData!");
        } 
        
        label = value[10];
    }
        
    public float getSize()
    {
        return size;
    }
    
    public float getX0()
    {
        return x0;
    }
    
    public float getY0()
    {
        return y0;
    }
    
    public Rectangle getBounds()
    {
        return new Rectangle(x, y, width, height);
    } 
    
    public Rectangle getBounds(DEViseWindow win)
    {
        if (win != null) {
            Rectangle r = new Rectangle(x + win.windowLoc.x, y + win.windowLoc.y, width, height);
            if (r.x < win.windowLoc.x) {
                r.width = r.width + r.x - win.windowLoc.x;
                r.x = win.windowLoc.x;
            } else if (r.x > win.windowLoc.x + win.windowLoc.width - r.width) {
                r.width = r.width + r.x - win.windowLoc.x - win.windowLoc.width + r.width;
                r.x = win.windowLoc.x + win.windowLoc.width - r.width;
            }
            if (r.y < win.windowLoc.y) {
                r.height = r.height + r.y - win.windowLoc.y;
                r.y = win.windowLoc.y;
            } else if (r.y > win.windowLoc.y + win.windowLoc.height - r.height) {
                r.height = r.height + r.y - win.windowLoc.y - win.windowLoc.height + r.height;
                r.y = win.windowLoc.y + win.windowLoc.height - r.height;
            }
            
            return r; 
        } else {
            return new Rectangle(x, y, width, height);
        }
    }
    
    public String getLabel()
    {
        if (label == null) {
            return new String("");
        } else {
            return label;
        }
    }
}