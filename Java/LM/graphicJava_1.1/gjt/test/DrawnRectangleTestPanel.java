package gjt.test;

import java.awt.*;
import java.awt.event.*;
import java.applet.Applet;
import gjt.*;

public class DrawnRectangleTestPanel extends Panel implements
MouseListener {
	private Applet          applet;
    private DrawnRectangle  drawnFilledOrange, 
                            drawnFilledBlue, drawnBlue;
    private EtchedRectangle etchedOut, 
                            etchedIn, etchedFilledCyan;
    private ThreeDRectangle thinRaised, 
                            thinInset, thickRaised, thickInset;

    public DrawnRectangleTestPanel(Applet applet) {
		this.applet = applet;

        drawnFilledOrange = 
            new DrawnRectangle (this, 10,  10,  100, 100);
        drawnFilledBlue = 
            new DrawnRectangle (this, 135, 135, 100, 100);
        drawnBlue = 
            new DrawnRectangle (this, 505, 135, 100, 100);
        etchedFilledCyan = 
            new EtchedRectangle(this, 10,  135, 100, 100);

        etchedIn = new EtchedRectangle(this, 385, 10, 100, 100);
        etchedOut= new EtchedRectangle(this, 505, 10, 100, 100);

        thinRaised = 
            new ThreeDRectangle(this, 135, 10,  100, 100);
        thinInset  = 
            new ThreeDRectangle(this, 260, 10,  100, 100);
        thickRaised = 
            new ThreeDRectangle(this, 385, 135, 100, 100);
        thickInset = 
            new ThreeDRectangle(this, 260, 135, 100, 100);

        drawnFilledOrange.setLineColor(Color.black);

        drawnFilledBlue.setLineColor(Color.yellow);
        drawnFilledBlue.setThickness(3);

        drawnBlue.setLineColor(Color.blue);
        drawnBlue.setThickness(5);

        thickRaised.setThickness(5);
        thickInset.setThickness (5);

		addMouseListener(this);
    }
    public Dimension getPreferredSize() {
        return new Dimension(610, 270);
    }
    public void paint(Graphics g) {
        drawnFilledOrange.paint();
        drawnFilledOrange.fill (Color.orange);

        drawnFilledBlue.paint  ();
        drawnFilledBlue.fill   (Color.blue);

        drawnBlue.paint         ();

        etchedIn.paintEtchedIn  ();
        etchedOut.paintEtchedOut();

        etchedFilledCyan.paintEtchedIn();
        etchedFilledCyan.fill(Color.cyan);

        thinRaised.paintRaised  ();
        thinInset.paintInset    ();

        thickRaised.paintRaised ();

        thickInset.paintInset  ();
        thickInset.fill      (Color.red);
    }
	public void mouseClicked (MouseEvent event) { }
	public void mouseReleased(MouseEvent event) { }
	public void mouseEntered (MouseEvent event) { }
	public void mouseExited  (MouseEvent event) { }

    public void mousePressed(MouseEvent event) {
		int x = event.getPoint().x;
		int y = event.getPoint().y;

        if(drawnFilledOrange.contains(x,y)) 
            show(drawnFilledOrange);

        if(drawnFilledBlue.contains(x,y))  show(drawnFilledBlue);
        if(drawnBlue.contains(x,y))        show(drawnBlue);
        if(etchedIn.contains(x,y))         show(etchedIn);
        if(etchedOut.contains(x,y))        show(etchedOut);
        if(etchedFilledCyan.contains(x,y)) show(etchedFilledCyan);
        if(thinRaised.contains(x,y))       show(thinRaised);
        if(thickRaised.contains(x,y))      show(thickRaised);
        if(thinInset.contains(x,y))        show(thinInset);
        if(thickInset.contains(x,y))       show(thickInset);
    }
    private void show(DrawnRectangle drawnRectangle) {
        applet.showStatus(drawnRectangle.toString());
    }
}
