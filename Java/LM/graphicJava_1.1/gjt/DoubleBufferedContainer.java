package gjt;

import java.awt.*;
import java.awt.event.*;

/**
 * A double-buffered container that can be used for animation
 * (see Playfield) or for dragging and dropping lightweight
 * components.  

 * A DoubleBufferedContainer has 2 offscreen buffers: one for
 * the background (alone), and a workplace buffer for drawing
 * into offscreen.
 * 
 * DoubleBufferedContainer provides a number of methods for
 * controlling blitting between background, workplace and
 * the screen itself.  For instance, blitBackgroundToWorkplace()
 * blits the entire background to the workpace buffer, but
 * not to the screen.  Later on, one could invoke
 * blitWorkPlaceToScreen(Rectangle), and the workplace would
 * be blitted to the screen, clipped by the specified rectangle.
 * <p>
 * 
 *
 * @version 1.0, January, 1997
 * @author  David Geary
 * @see		gjt.animation.Playfield
 * @see 	gjt.test.DoubleBufferedContainerTest
 */
public class DoubleBufferedContainer extends Container {
	protected OffscreenBuffer buffers = new OffscreenBuffer();
	private   Image wallpaperImage;

	public DoubleBufferedContainer() {
		this(null);
	}
	public DoubleBufferedContainer(Image wallpaperImage) {
		setWallpaperImage(wallpaperImage);

		addComponentListener(new ComponentAdapter() {
			public void componentResized(ComponentEvent event) {
				buffers.update();
			}
		});
	}
	public void setWallpaperImage(Image image) {
		wallpaperImage = image;
	}
	public void update(Graphics g) { 
		paint(g); 
	}
	public void paint(Graphics g) {
		Rectangle clip = g.getClipBounds();

		if(windowDamaged(g)) {
        	buffers.blitWorkplaceToScreen(clip);
		}
		else {
			Graphics  wpg  = buffers.getWorkplaceGraphics();
        	Dimension size = getSize();

			paintBackground();
			wpg.setClip(0,0,size.width,size.height);
			super.paint(wpg);
			buffers.blitWorkplaceToScreen();
			wpg.dispose();
		}		
	}
	public void blitWorkplaceToScreen() {
		buffers.blitWorkplaceToScreen();
	}
	public void blitBackgroundToWorkplace() {
		buffers.blitBackgroundToWorkplace();
	}
	public void blitWorkplaceToScreen(Rectangle clip) {
		buffers.blitWorkplaceToScreen(clip);
	}
	public void blitBackgroundToWorkplace(Rectangle clip) {
		buffers.blitBackgroundToWorkplace(clip);
	}
	public void paintComponents(Rectangle clip, boolean update) {
		Graphics wpg = buffers.getWorkplaceGraphics();

		wpg.setClip(clip);
		super.paint(wpg);
		
		if(update) {
			buffers.blitWorkplaceToScreen(clip);
		}
		wpg.dispose();
	}
	public void paintComponent(Component comp) {
		paintComponent(comp, true);
	}
	public void eraseComponent(Component comp) {
		eraseComponent(comp, true);
	}
	public void paintComponent(Component comp, boolean update) {
		Graphics  wpg    = buffers.getWorkplaceGraphics(); 
		Rectangle bounds = comp.getBounds();
		Graphics  compGraphics;

		compGraphics = wpg.create(bounds.x,     bounds.y, 
		                          bounds.width, bounds.height);
		comp.paint(compGraphics);

		if(update)
			buffers.blitWorkplaceToScreen(bounds);

		wpg.dispose();
	}
	public void eraseComponent(Component comp, boolean update) {
		Rectangle bounds = comp.getBounds();

		buffers.blitBackgroundToWorkplace(bounds);
		paintOverlappingComponents(comp);

		if(update)
			buffers.blitWorkplaceToScreen(bounds);
	}
	public void moveComponent(Component comp, Point newLoc) {
		Rectangle oldBounds = comp.getBounds();

		eraseComponent(comp, false);  // erase - no screen update
		comp.setLocation(newLoc);     // move component
		paintComponent(comp, false);  // paint comp - no update

		buffers.blitWorkplaceToScreen(
			oldBounds.union(comp.getBounds()));
	}
	protected boolean windowDamaged(Graphics g) {
		Rectangle clip = g.getClipBounds();
		Dimension size = getSize();

		return ((clip.x != 0 || clip.y != 0) ||
		        (clip.width  < size.width    ||
				 clip.height < size.height));
	}
	protected void paintOverlappingComponents(Component comp) {
		Graphics  wpg    = buffers.getWorkplaceGraphics();
		Rectangle bounds = comp.getBounds();

		wpg.setClip    (bounds); // set offscreen clip to bounds
		comp.setVisible(false);  // so comp won't be drawn below
		super.paint    (wpg);    // draw all but comp
		comp.setVisible(true);   // reset comp visibility
		wpg.dispose();
	}
    protected void paintBackground() {
		paintBackground((Rectangle)null);
	}
	protected void paintBackground(Rectangle clip) {
		Graphics g = buffers.getBackgroundGraphics();
		if(clip != null) {
			g.setClip(clip);
		}
		paintBackground(g);
		buffers.blitBackgroundToWorkplace();
		g.dispose();
    }
	protected void paintBackground(Graphics g) {           
		if(wallpaperImage != null) {
			Util.wallPaper(this, g, wallpaperImage);
		}
	}

	class OffscreenBuffer {
		private Image     workplaceBuffer, backgroundBuffer;
		private Dimension offscreenSize;

		public Image getWorkplaceBuffer() {
			return workplaceBuffer;
		}
		public Image getBackground() {
			return backgroundBuffer;
		}
		public Graphics getWorkplaceGraphics() {
			return workplaceBuffer.getGraphics();
		}
		public Graphics getBackgroundGraphics() {
			return backgroundBuffer.getGraphics();
		}
		public void blitWorkplaceToScreen() {
			blitWorkplaceToScreen(null);
		}
		public void blitBackgroundToWorkplace() {
			blitBackgroundToWorkplace(null);
		}
		public void blitWorkplaceToScreen(Rectangle clip) {
			Graphics screenGraphics = getGraphics();

			if(screenGraphics != null) {
				if(clip != null) {
					screenGraphics.setClip(clip);
				}
				screenGraphics.drawImage(workplaceBuffer, 0,0,
								 DoubleBufferedContainer.this);
				screenGraphics.dispose();
			}
		}
		public void blitBackgroundToWorkplace(Rectangle clip) {
			Graphics wpg = getWorkplaceGraphics();

			if(clip != null) {
				wpg.setClip(clip);
			}
			wpg.drawImage(backgroundBuffer, 0,0,
			              DoubleBufferedContainer.this);
			wpg.dispose();
		}
		void update() {
			if(needNewOffscreenBuffer()) {
				createBuffers();
			}
		}
 		private boolean needNewOffscreenBuffer() {
			Dimension size = getSize();
   	      	return (workplaceBuffer == null ||
   	             	size.width     != offscreenSize.width ||
   	             	size.height    != offscreenSize.height);
   		}
   	 	private void createBuffers() {
		  offscreenSize    = getSize();
   	   	  workplaceBuffer  = createOffscreenImage(offscreenSize);
		  backgroundBuffer = createOffscreenImage(offscreenSize);
   	 	}
   	 	private Image createOffscreenImage(Dimension size) {
   	   	  Image image = createImage(size.width, size.height);
   	   	  Util.waitForImage(DoubleBufferedContainer.this, image);
   	   	  return image;
   	 	}	
	}
}
