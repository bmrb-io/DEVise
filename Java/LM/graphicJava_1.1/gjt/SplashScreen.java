package gjt;

import java.awt.*;

/**
 * A borderless window containing an image.
 *
 * @version 1.0, Apr 1 1997
 * @author  David Geary
 */
public class SplashScreen extends Window {
	private Toolkit toolkit = 
							Toolkit.getDefaultToolkit();
	private Window window;
	private Image  image;

	public SplashScreen(Frame frame, Image image) {
		super(frame);
		Util.waitForImage(this, image);
		
		this.image = image;
		add("Center", makeImageContainer(image));

		Dimension   scrnSize  = toolkit.getScreenSize();
		int         imgWidth  = image.getWidth(frame),
		            imgHeight = image.getHeight(frame);
	}
	public Component makeImageContainer(Image image) {
		return new ImageCanvas(image);
	}
	public void setVisible(boolean b) {
		if(b) {
			Dimension scrnSize  = toolkit.getScreenSize();
			int       imgWidth  = image.getWidth(this),
		          	  imgHeight = image.getHeight(this);

			setLocation(scrnSize.width/2  - (imgWidth/2),
		                scrnSize.height/2 - (imgHeight/2));
			pack();
			toFront();
		}
		super.setVisible(b);
	}
}
