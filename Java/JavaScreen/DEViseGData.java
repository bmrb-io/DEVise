import java.awt.*;

public class DEViseGData 
{   
    float x0 = 0, y0 = 0, size = 0;
    public int x = 0, y = 0, width = 0, height = 0;
   
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
        } catch (NumberFormatException e) {
            throw new YException("Invalid GData!");
        }
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
    
    public Rectangle getRectangle()
    {
        return new Rectangle(x, y, width, height);
    }
}