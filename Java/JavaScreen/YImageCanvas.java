import java.awt.*;
import java.awt.event.*;

public final class YImageCanvas extends Canvas
{
    Image image = null, offScrImg = null;
    int imageWidth, imageHeight; 
    Dimension size = null;
    
    public YImageCanvas(Image img) throws YException
    {
        if (img == null) 
            throw new YException("Null Image!");
        
        image = img;
        imageWidth = image.getWidth(this);
        imageHeight = image.getHeight(this);
        if (imageWidth <= 0 || imageHeight <= 0)
            throw new YException("Invalid Image!");
        
        size = new Dimension(imageWidth, imageHeight);                
    }
    
    public Dimension getPreferredSize()
    {
        return size;
    }

    public Dimension getMinimumSize()
    {
        return size;
    }
    
    public boolean setImage(Image img) 
    {
        if (img == null)
            return false;
        
        if (img.getWidth(this) != imageWidth || img.getHeight(this) != imageHeight)
            return false;
        
        image = img;
        offScrImg = null;
        
        repaint();
        
        return true;    
    }

    // Enable double-buffering
    public void update(Graphics g)
    {
        if (offScrImg == null)
            offScrImg = createImage(imageWidth, imageHeight);

        Graphics og = offScrImg.getGraphics();
        paint(og);
        g.drawImage(offScrImg, 0, 0, this);
        og.dispose();
    }

    public void paint(Graphics g)
    {
        if (image != null) {
            g.drawImage(image, 0, 0, this);
        }
    }
}        