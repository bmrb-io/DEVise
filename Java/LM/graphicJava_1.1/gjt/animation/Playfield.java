package gjt.animation;

import java.awt.*;
import java.util.*;
import gjt.*;

/**
 * An extension of gjt.DoubleBufferedContainer, a playfield keeps track
 * of sprites, which it animates continuously.<p>
 *
 * Each Playfield comes complete with two collision detectors:
 * an edge collision detector and a sprite collision detector.
 *
 * @version 1.0, Apr 1 1996
 * @version 1.1, Feb 1997
 *
 *	Refactored to extend DoubleBufferedContainer.
 *
 * @author  David Geary
 * @see     gjt.DoubleBufferedContainer
 * @see     gjt.animation.CollisionArena
 * @see     gjt.animation.Sprite
 * @see     gjt.animation.SpriteCollisionDetector
 * @see     gjt.animation.EdgeCollisionDetector
 * @see     gjt.test.SimpleAnimationTest
 * @see     gjt.test.BumpAnimationTest
 * @see     gjt.test.TwoDrinkersAnimationTest
 */
public class Playfield extends DoubleBufferedContainer
                               implements Runnable, 
                                          CollisionArena {
    private   boolean running = false;
    private   Thread  animationThread;
	private   CollisionDetector spriteDetector, edgeDetector;

    public Playfield() {
        edgeDetector   = new EdgeCollisionDetector(this);
        spriteDetector = new SpriteCollisionDetector(this);
		setLayout(new BulletinLayout());
    }
	public void spriteCollision(Sprite sprite, Sprite other) { }
	public void edgeCollision  (Sprite sprite, Orientation o){ }

    public void    stop   ()  { running = false;  }
    public boolean running()  { return running;   }

    public void start() {
        animationThread = new Thread(this);
        running = true;
        animationThread.start();
    }
    public void run() {
        while(running == true) {
            edgeDetector.detectCollisions  ();
            spriteDetector.detectCollisions();

            animateSprites();
			try { Thread.currentThread().sleep(50); }
			catch(Exception e) { e.printStackTrace(); }
        }
        animationThread = null;
    }
	public Vector getSprites() {
		int         ncomps = getComponentCount();
		Component   comp;
		Vector      vector = new Vector();

        for(int i=0; i < ncomps; ++i) {
			if((comp = getComponent(i)) instanceof Sprite)
				vector.addElement(comp);
        }
		return vector;
	}
    protected void animateSprites() {
		int         ncomps = getComponentCount();
		Component   comp;

        for(int i=0; i < ncomps; ++i) 
			if((comp = getComponent(i)) instanceof Sprite)
				((Sprite)comp).animate();
    }
}
