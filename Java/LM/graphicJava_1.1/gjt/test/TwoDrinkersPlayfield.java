package gjt.test;

import java.applet.Applet;
import java.net.URL;
import gjt.*;
import gjt.animation.*;
import java.awt.*;
import java.awt.event.*;

public class TwoDrinkersPlayfield extends Playfield 
                                  implements ItemListener {
    private Applet   applet;
	private Choice   collisionChoice;
	private boolean  collisionsEnabled = true;
    private URL      cb;
    private Sprite   moveFastSpinSlow, moveSlowSpinFast;
    private Sequence fastSpinSequence, 
                     slowSpinSequence,
                     fastBumpSequence,
                     slowBumpSequence;

    public TwoDrinkersPlayfield(Applet applet) {
        this.applet = applet;
        cb          = applet.getCodeBase();
        makeSequencesAndSprites();
		add(collisionChoice = new Choice());
		collisionChoice.add("Collision Detection On");
		collisionChoice.add("Collision Detection Off");

		collisionChoice.addItemListener(this);

		addMouseListener(new MouseAdapter() {
    		public void mousePressed (MouseEvent event) {
        		if(running() == true) stop ();
        		else                  start();
    		}
		});
    }
    public void paintBackground(Graphics g) {
        Image bg = applet.getImage(cb, "gifs/background.gif");
        Util.wallPaper(this, g, bg);
    }
	public void itemStateChanged(ItemEvent event) {
		if(event.getSource() == collisionChoice) {
			if(collisionChoice.getSelectedIndex() == 0) {
				collisionsEnabled = true;
			}
			else {
				collisionsEnabled = false;
			}
		}
	}
    public void spriteCollision(Sprite sprite, Sprite sprite2) {
		if(collisionsEnabled) {
          	if(moveSlowSpinFast.getSequence() != 
		  			fastBumpSequence) {
            	sprite.reverse();
            	sprite2.reverse();

            	moveSlowSpinFast.play(fastBumpSequence, 3);
            	moveFastSpinSlow.play(slowBumpSequence, 3);
        	}  	 
		}
    }
    public void edgeCollision(Sprite      sprite, 
                              Orientation orientation) {
        if(orientation == Orientation.RIGHT || 
           orientation == Orientation.LEFT) 
            sprite.reverseX();
        else 
            sprite.reverseY();
    }
    private void makeSequencesAndSprites() {
        String  file;
        Image[] spinImages = new Image[19];
        Image[] bumpImages = new Image[6];
        Image[] volleyball = new Image[4];

        for(int i=0; i < spinImages.length; ++i) {
            file = "gifs/spin";

            if(i < 10) file += "0" + i + ".gif";
            else       file += i + ".gif";

            spinImages[i] = applet.getImage(cb, file);
        }
        for(int i=0; i < bumpImages.length; ++i) {
            file = "gifs/bump0" + i + ".gif";
            bumpImages[i] = applet.getImage(cb, file);
        }
        fastSpinSequence = new Sequence(this, spinImages);
        slowSpinSequence = new Sequence(this, spinImages);

        fastBumpSequence = new Sequence(this, bumpImages);
        slowBumpSequence = new Sequence(this, bumpImages);

        moveFastSpinSlow = 
            new Sprite(this, 
                slowSpinSequence, new Point(25, 75));

        moveSlowSpinFast = 
            new Sprite(this, 
                fastSpinSequence, new Point(250,250));

        fastSpinSequence.setAdvanceInterval(50);
        slowSpinSequence.setAdvanceInterval(300);

        fastBumpSequence.setAdvanceInterval(25);
        slowBumpSequence.setAdvanceInterval(200);

        moveFastSpinSlow.setMoveVector(new Point(2,3));
        moveSlowSpinFast.setMoveVector(new Point(-1,-1));

        moveSlowSpinFast.setMoveInterval(100);

        add(moveFastSpinSlow);
        add(moveSlowSpinFast);
    }
}
