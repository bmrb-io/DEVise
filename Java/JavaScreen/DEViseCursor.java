import java.awt.*;

public class DEViseCursor
{
    int x, y, width, height;
    
    public DEViseCursor(Rectangle rect) throws YException
    {
        if (rect == null)
            throw new YException("Invalid argument!");
            
        x = rect.x;
        y = rect.y;
        width = rect.width;
        height = rect.height;
    }
}